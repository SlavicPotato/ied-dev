#include "pch.h"

#include "Controller.h"

#include "IConditionalVariableProcessor.h"
#include "IMaintenance.h"
#include "INodeOverride.h"

#include "IED/EngineExtensions.h"
#include "IED/Inventory.h"
#include "IED/NodeMap.h"
#include "IED/ReferenceLightController.h"
#include "IED/TransformOverrides.h"
#include "IED/UI/UIMain.h"
#include "IED/Util/Common.h"

#include "IED/UI/UIIntroBanner.h"

#include "Drivers/Input.h"
#include "Drivers/UI.h"

#include "IED/Parsers/JSONConfigStoreParser.h"

#include <ext/SKSEMessagingHandler.h>
#include <ext/SKSESerializationEventHandler.h>

//#define IED_ENABLE_STATS_G
//#define IED_ENABLE_STATS_T

namespace IED
{
	using namespace Util::Common;
	using namespace ::Util::Node;
	using namespace Data;

	Controller::Controller(
		const stl::smart_ptr<const ConfigINI>& a_config) :
		IEquipment(m_rngBase),
		IAnimationManager(m_configData.settings),
		KeyBindEventHandler(stl::make_smart_for_overwrite<KB::KeyBindDataHolder>()),
		m_iniconf(a_config),
		m_nodeOverrideEnabled(a_config->m_nodeOverrideEnabled),
		m_nodeOverridePlayerEnabled(a_config->m_nodeOverridePlayerEnabled),
		m_npcProcessingDisabled(a_config->m_disableNPCProcessing),
		m_forceDefaultConfig(a_config->m_forceDefaultConfig),
		m_forceFlushSaveData(a_config->m_forceFlushSaveData),
		m_bipedCache(
			a_config->m_bipedSlotCacheMaxSize,
			a_config->m_bipedSlotCacheMaxForms)
	{
		ODBEnableBackgroundLoading(a_config->m_odbBackgroundLoading);
		SetBackgroundCloneLevel(true, a_config->m_bgClonePlayer);
		SetBackgroundCloneLevel(false, a_config->m_bgCloneNPC);
	}

	void Controller::SinkInputEvents()
	{
		assert(m_iniconf);

		Drivers::Input::RegisterForKeyEvents(m_inputHandlers.playerBlock);
		Drivers::Input::RegisterForKeyEvents(static_cast<KB::KeyBindEventHandler&>(*this));

		if (m_iniconf->m_enableUI)
		{
			Drivers::Input::RegisterForPriorityKeyEvents(m_inputHandlers.uiOpen);
		}
	}

	void Controller::SinkSerializationEvents()
	{
		auto& seh = SKSESerializationEventHandler::GetSingleton();

		seh.GetDispatcher<SKSESerializationEvent>().AddSink(this);
		seh.GetDispatcher<SKSESerializationFormDeleteEvent>().AddSink(this);

		seh.RegisterForLoadEvent(SKSE_SERIALIZATION_TYPE_ID, this);
	}

	void Controller::SinkEventsT0()
	{
		SKSEMessagingHandler::GetSingleton().AddSink(this);

		ITaskPool::AddTaskFixed(this);

		SinkInputEvents();
		SinkSerializationEvents();
	}

	bool Controller::SinkEventsT1()
	{
		if (auto mm = MenuManager::GetSingleton())
		{
			mm->GetMenuOpenCloseEventDispatcher().AddEventSink(this);

			return true;
		}
		else
		{
			return false;
		}
	}

	bool Controller::SinkEventsT2()
	{
		if (auto edl = ScriptEventSourceHolder::GetSingleton())
		{
			edl->AddEventSink<TESInitScriptEvent>(this);
			edl->AddEventSink<TESObjectLoadedEvent>(this);
			edl->AddEventSink<TESEquipEvent>(this);
			edl->AddEventSink<TESContainerChangedEvent>(this);
			edl->AddEventSink<TESFurnitureEvent>(this);
			edl->AddEventSink<TESDeathEvent>(this);
			edl->AddEventSink<TESSwitchRaceCompleteEvent>(this);
			edl->AddEventSink<TESActorLocationChangeEvent>(this);
			//edl->AddEventSink<TESCombatEvent>(this);
			//edl->AddEventSink<TESSceneEvent>(this);
			//edl->AddEventSink<TESPackageEvent>(this);
			//edl->AddEventSink<TESQuestStartStopEvent>(this);

			return true;
		}
		else
		{
			return false;
		}
	}

	void Controller::InitializeData()
	{
		assert(m_iniconf);

		m_configData.settings = std::make_unique_for_overwrite<SettingHolder>();

		auto& settings = *m_configData.settings;

		settings.SetPath(PATHS::SETTINGS);

		if (!settings.Load())
		{
			if (Serialization::FileExists(PATHS::SETTINGS))
			{
				Error(
					"%s: failed loading settings: %s",
					PATHS::SETTINGS,
					settings.GetLastException().what());
			}
		}
		else
		{
			if (settings.HasErrors())
			{
				Warning("%s: settings loaded with errors", PATHS::SETTINGS);
			}
		}

		const auto& data = settings.data;

		if (data.logLevel)
		{
			gLog.SetLogLevel(*data.logLevel);
		}

		ISKSE::GetBacklog().SetLimit(std::clamp<std::uint32_t>(data.ui.logLimit, 1, 10000));

		SetODBLevel(data.odbLevel);

		InitializeSound();

		if (data.playerBlockKeys)
		{
			m_inputHandlers.playerBlock.SetKeys(
				data.playerBlockKeys->comboKey,
				data.playerBlockKeys->key);
		}
		else
		{
			m_inputHandlers.playerBlock.SetKeys(
				m_iniconf->m_toggleBlockKeys.GetComboKey(),
				m_iniconf->m_toggleBlockKeys.GetKey());
		}

		InitializeLocalization();
		InitializeConfig();
		LoadAnimationData();

		SetShaderProcessingEnabled(data.enableEffectShaders);
		SetProcessorTaskParallelUpdates(data.apParallelUpdates);

		m_cpuHasSSE41 = ::IsProcessorFeaturePresent(PF_SSE4_1_INSTRUCTIONS_AVAILABLE);

		if (m_cpuHasSSE41)
		{
			SetPhysicsProcessingEnabled(data.enableEquipmentPhysics);
			PHYSimComponent::SetMaxDiff(data.physics.maxDiff);
		}
		else
		{
			SetPhysicsProcessingEnabled(false);
		}

		auto& rlc = ReferenceLightController::GetSingleton();

		rlc.SetNPCLightUpdatesEnabled(data.lightEnableNPCUpdates);
	}

	void Controller::GetSDSInterface()
	{
		auto result = PluginInterfaceBase::query_interface<PluginInterfaceSDS>();

		SetPluginInterface(result.intfc, result.error);

		if (!result)
		{
			Warning("SDS interface not found: %s", PluginInterfaceBase::get_error_string(result.error));
			return;
		}

		auto pluginVersion = result.intfc->GetPluginVersion();

		Debug(
			"Found SDS interface [%s %u.%u.%u, interface ver: %u]",
			result.intfc->GetPluginName(),
			GET_PLUGIN_VERSION_MAJOR(pluginVersion),
			GET_PLUGIN_VERSION_MINOR(pluginVersion),
			GET_PLUGIN_VERSION_REV(pluginVersion),
			result.intfc->GetInterfaceVersion());

		result.intfc->RegisterForPlayerShieldOnBackEvent(this);

		if (result.intfc->GetPluginVersion() >= 2)
		{
			m_weaponNodeSharingDisabled = result.intfc->IsWeaponNodeSharingDisabled();

			if (m_weaponNodeSharingDisabled)
			{
				Debug("Weapon node share killer patch enabled in SDS");
			}
		}
	}

	static void UpdateSoundPairFromINI(
		const std::optional<ConfigForm>& a_src,
		std::optional<Game::FormID>&     a_dst)
	{
		if (a_src && !a_dst)
		{
			Game::FormID tmp;

			if (IData::GetPluginInfo().ResolveFormID(*a_src, tmp))
			{
				a_dst.emplace(tmp);
			}
		}
	}

	static void UpdateSoundPairFromINI(
		const ConfigSound<ConfigForm>::soundPair_t& a_src,
		ConfigSound<Game::FormID>::soundPair_t&     a_dst)
	{
		UpdateSoundPairFromINI(a_src.first, a_dst.first);
		UpdateSoundPairFromINI(a_src.second, a_dst.second);
	}

	void Controller::InitializeSound()
	{
		assert(m_iniconf);
		assert(m_configData.settings);

		auto& settings = m_configData.settings->data.sound;

		for (auto& e : m_iniconf->m_sound.data)
		{
			auto r = settings.data.try_emplace(e.first);
			UpdateSoundPairFromINI(e.second, r.first->second);
		}

		UpdateSoundForms();

		SetPlaySound(settings.enabled);
		SetPlaySoundNPC(settings.npc);
	}

	void Controller::UpdateSoundForms()
	{
		assert(m_configData.settings);

		ClearSounds();

		for (auto& e : m_configData.settings->data.sound.data)
		{
			AddSound(e.first, MakeSoundPair(e.second));
		}
	}

	void Controller::InitializeInputHandlers()
	{
		assert(m_iniconf);

		m_inputHandlers.playerBlock.SetLambda([this] {
			ITaskPool::AddTask(
				[this] {
					TogglePlayerBlock();
				});
		});

		m_inputHandlers.playerBlock.SetProcessPaused(false);

		if (m_iniconf->m_enableUI)
		{
			m_inputHandlers.uiOpen.SetLambda(
				[this] {
					UIOpen();
				});

			m_inputHandlers.uiOpen.SetProcessPaused(
				m_iniconf->m_enableInMenus);
		}
	}

	void Controller::InitializeUI()
	{
		assert(m_iniconf);
		assert(m_configData.settings);

		UIInitialize(*this);

		const auto& config = m_configData.settings->data;

		if (auto& renderTask = UIGetRenderTask())
		{
			renderTask->SetControlLock(config.ui.enableControlLock);
			renderTask->SetFreezeTime(config.ui.enableFreezeTime);
			renderTask->SetWantCursor(true);
			renderTask->SetEnabledInMenu(m_iniconf->m_enableInMenus);
			renderTask->EnableRestrictions(config.ui.enableRestrictions);
		}

		if (m_iniconf->m_forceUIOpenKeys &&
		    m_iniconf->m_UIOpenKeys)
		{
			m_inputHandlers.uiOpen.SetKeys(
				m_iniconf->m_UIOpenKeys.GetComboKey(),
				m_iniconf->m_UIOpenKeys.GetKey());

			m_iniKeysForced = true;
		}
		else
		{
			if (config.ui.openKeys)
			{
				m_inputHandlers.uiOpen.SetKeys(
					config.ui.openKeys->comboKey,
					config.ui.openKeys->key);
			}
			else if (m_iniconf->m_UIOpenKeys)
			{
				m_inputHandlers.uiOpen.SetKeys(
					m_iniconf->m_UIOpenKeys.GetComboKey(),
					m_iniconf->m_UIOpenKeys.GetKey());
			}
		}

		if (!m_inputHandlers.uiOpen.Enabled())
		{
			m_inputHandlers.uiOpen.SetKeys(
				0,
				DIK_BACKSPACE);
		}

		UIEnableNotifications(config.ui.enableNotifications);
		UISetLogNotificationThreshold(config.ui.notificationThreshold);
	}

	bool Controller::DispatchIntroBanner()
	{
		auto task = make_timed_ui_task<UI::UIIntroBanner>(7000000, m_iniconf->m_introBannerVOffset);

		task->SetControlLock(false);
		task->SetFreezeTime(false);
		task->SetWantCursor(false);
		task->SetEnabledInMenu(true);
		task->EnableRestrictions(false);

		return Drivers::UI::AddTask(INTRO_BANNER_TASK_ID, task);
	}

	void Controller::InitializeConfig()
	{
		assert(m_iniconf);

		m_configData.initial = std::make_unique_for_overwrite<Data::configStore_t>();

		bool defaultConfLoaded = false;

		if (Serialization::FileExists(PATHS::DEFAULT_CONFIG_USER))
		{
			defaultConfLoaded = LoadConfigStore(
				PATHS::DEFAULT_CONFIG_USER,
				*m_configData.initial);
		}

		if (!defaultConfLoaded)
		{
			defaultConfLoaded = LoadConfigStore(
				PATHS::DEFAULT_CONFIG,
				*m_configData.initial);
		}

		if (!defaultConfLoaded)
		{
			Warning("No default configuration could be loaded");
		}

		FillGlobalSlotConfig(m_configData.initial->slot);
		IMaintenance::CleanConfigStore(*m_configData.initial);

		m_configData.active = std::make_unique<Data::configStore_t>(*m_configData.initial);

		GetKeyBindDataHolder()->SetFromConfig(m_configData.active->keybinds);

		if (IsDefaultConfigForced())
		{
			Message("Default configuration is forced");
		}
	}

	void Controller::InitializeLocalization()
	{
		assert(m_configData.settings);

		auto& settings = *m_configData.settings;
		auto& clang    = settings.data.language;

		stl::optional<stl::fixed_string> defaultLang;

		if (clang.empty())
		{
			settings.set(clang, defaultLang.try_insert(Localization::LocalizationDataManager::DEFAULT_LANG));
		}

		if (!SetLanguageImpl(clang))
		{
			if (clang != defaultLang.try_insert(Localization::LocalizationDataManager::DEFAULT_LANG))
			{
				settings.set(clang, std::move(*defaultLang));
				SetLanguageImpl(clang);
			}
		}
	}

	void Controller::LoadAnimationData()
	{
		assert(m_iniconf);

		if (m_iniconf->m_agManualMode >= 2)
		{
			SetAnimationInfo(m_iniconf->m_agInfo);
		}
		else
		{
			try
			{
				SetAnimationInfo(ExtractAnimationInfoFromPEX());
			}
			catch (const std::exception& e)
			{
				Error("Couldn't extract animation info: %s", e.what());
			}

			if (!GetAnimationInfo() &&
			    m_iniconf->m_agManualMode == 1)
			{
				SetAnimationInfo(m_iniconf->m_agInfo);
			}
		}

		if (auto& info = GetAnimationInfo())
		{
			Debug(
				"XP32 aa: crc:[%d], xpe:[sword:%d, axe:%d, dagger:%d, mace:%d, 2hsword:%d, 2haxe:%d, bow:%d, bowatk:%d, bowidle:%d]",
				info->crc,
				info->get_base(AnimationWeaponType::Sword),
				info->get_base(AnimationWeaponType::Axe),
				info->get_base(AnimationWeaponType::Dagger),
				info->get_base(AnimationWeaponType::Mace),
				info->get_base(AnimationWeaponType::TwoHandedSword),
				info->get_base(AnimationWeaponType::TwoHandedAxe),
				info->get_base(AnimationWeaponType::Bow),
				info->get_base_extra(AnimationExtraGroup::BowAttack),
				info->get_base_extra(AnimationExtraGroup::BowIdle));
		}
	}

	void Controller::InitializeBSFixedStringTable()
	{
		assert(StringCache::IsInitialized());

		const stl::lock_guard lock(m_lock);

		InitializeAnimationStrings();
	}

	void Controller::InitializeControllerPostDataLoad()
	{
		const stl::lock_guard lock(m_lock);

		InitializeFPStateData();
		InitializeData();

		ASSERT(SinkEventsT1());
		ASSERT(SinkEventsT2());

		if (Drivers::UI::IsImInitialized())
		{
			InitializeUI();
		}

		if (IFPV_Detected())
		{
			Debug("IFPV detector plugin found");
		}

		Debug(
			"ODB: background load: %hhu",
			ODBGetBackgroundLoadingEnabled());

		Debug(
			"OM: background clone (player/npc): %hhu / %hhu",
			GetBackgroundCloneLevel(true),
			GetBackgroundCloneLevel(false));

		StartAPThreadPool();
		SetProcessorTaskRunState(true);

		Debug("Data loaded, entered running state");
	}

	void Controller::OnDataLoaded()
	{
		InitializeControllerPostDataLoad();

		if (Drivers::UI::IsImInitialized())
		{
			SetupUI();
		}

		m_iniconf.reset();

		m_safeToOpenUI = true;
	}

	void Controller::Evaluate(
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		const stl::lock_guard lock(m_lock);

		EvaluateImpl(a_actor, a_handle, a_flags);
	}

	std::pair<std::size_t, std::size_t> Controller::GetNumSimComponents() const noexcept
	{
		std::pair<std::size_t, std::size_t> result{ 0, 0 };

		for (auto& e : m_actorMap.getvec())
		{
			const auto sz = e->second.GetSimComponentListSize();

			result.first += sz;
			if (e->second.IsActive())
			{
				result.second += sz;
			}
		}

		return result;
	}

	std::size_t Controller::GetNumAnimObjects() const noexcept
	{
		std::size_t result = 0;

		for (auto& e : m_actorMap.getvec())
		{
			result += e->second.GetNumAnimObjects();
		}

		return result;
	}

	std::size_t Controller::GetNumQueuedCloningTasks() const noexcept
	{
		std::size_t result = 0;

		for (auto& e : m_actorMap.getvec())
		{
			result += e->second.GetNumQueuedCloningTasks();
		}

		return result;
	}

	void Controller::QueueSetEffectShaders(Actor* a_actor) noexcept
	{
		ITaskPool::QueuePLLoadedActorTask(
			a_actor,
			[this](
				Actor*            a_actor,
				Game::ActorHandle a_handle) noexcept {
				const stl::lock_guard lock(m_lock);

				if (ShaderProcessingEnabled())
				{
					auto it = m_actorMap.find(a_actor->formID);
					if (it != m_actorMap.end())
					{
						UpdateShaders(a_actor, 0.0f, it->second);
					}
				}
			});
	}

	void Controller::ClearBipedCache()
	{
		m_bipedCache.clear();
	}

	std::size_t Controller::RemoveBipedCacheEntries(
		std::function<bool(Game::FormID)> a_filter)
	{
		std::size_t result = 0;

		auto& data = m_bipedCache.data();

		for (auto it = data.begin(); it != data.end();)
		{
			if (a_filter(it->first))
			{
				it = data.erase(it);
				result++;
			}
			else
			{
				++it;
			}
		}

		return result;
	}

	void Controller::QueueClearPlayerRPC() noexcept
	{
		ITaskPool::AddPriorityTask([this] {
			const stl::lock_guard lock(m_lock);

			auto& data = GetActorMap();

			auto it = data.find(IData::GetPlayerRefID());
			if (it != data.end())
			{
				it->second.ClearRPC();
			}
		});
	}

	void Controller::EvaluateImpl(
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		auto root = a_actor->GetNiRootFadeNode(false);
		if (!root)
		{
			return;
		}

		auto npcroot = GetNodeByName(root, BSStringHolder::GetSingleton()->m_npcroot);
		if (!npcroot)
		{
			return;
		}

		EvaluateImpl(root, npcroot, a_actor, a_handle, a_flags);
	}

	void Controller::EvaluateImpl(
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		ActorObjectHolder&               a_holder,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		if (!IsREFRValid(a_actor))
		{
			return;
		}

		auto root = a_actor->GetNiRootFadeNode(false);
		if (!root)
		{
			Warning(
				"%s [%u]: %.8X: actor has no 3d",
				__FUNCTION__,
				__LINE__,
				a_actor->formID.get());

			return;
		}

		auto npcroot = GetNodeByName(root, BSStringHolder::GetSingleton()->m_npcroot);
		if (!npcroot)
		{
			return;
		}

		auto nrp = GetNPCRacePair(a_actor);
		if (!nrp)
		{
			return;
		}

		if (a_holder.m_root != root)
		{
			Warning(
				"%s [%u]: %.8X: skeleton root mismatch",
				__FUNCTION__,
				__LINE__,
				a_actor->formID.get());

			QueueReset(
				a_actor,
				ControllerUpdateFlags::kNone);
		}
		else
		{
			EvaluateImpl(
				root,
				npcroot,
				a_actor,
				nrp->npc,
				nrp->race,
				a_handle,
				a_holder,
				a_flags);
		}
	}

	bool Controller::RemoveActor(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		const stl::lock_guard lock(m_lock);

		return RemoveActorImpl(a_actor, a_handle, a_flags);
	}

	/*bool Controller::RemoveActor(
		TESObjectREFR*                   a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		const stl::lock_guard lock(m_lock);

		return RemoveActorImpl(a_actor->formID, a_flags);
	}*/

	bool Controller::RemoveActor(
		Game::FormID                     a_actor,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		const stl::lock_guard lock(m_lock);

		return RemoveActorImpl(a_actor, a_flags);
	}

	void Controller::QueueNiNodeUpdate(Game::FormID a_actor)
	{
		ITaskPool::AddTask([this, a_actor] {
			NiPointer<TESObjectREFR> refr;
			{
				const stl::lock_guard lock(m_lock);

				auto it = m_actorMap.find(a_actor);
				if (it == m_actorMap.end())
				{
					return;
				}

				if (!it->second.GetHandle().Lookup(refr))
				{
					return;
				}
			}

			if (auto actor = refr->As<Actor>())
			{
				if (IsREFRValid(actor))
				{
					actor->QueueNiNodeUpdate(true);
				}
			}
		});
	}

	void Controller::QueueEvaluate(
		TESObjectREFR*                   a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::QueueLoadedActorTask(
			a_actor,
			[this, a_flags](
				Actor*            a_actor,
				Game::ActorHandle a_handle) {
				Evaluate(
					a_actor,
					static_cast<Game::ObjectRefHandle>(a_handle),
					a_flags);
			});
	}

	void Controller::QueueEvaluate(
		Game::ActorHandle                a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::QueueLoadedActorTask(
			a_handle,
			[this, a_flags](
				Actor*            a_actor,
				Game::ActorHandle a_handle) {
				Evaluate(
					a_actor,
					static_cast<Game::ObjectRefHandle>(a_handle),
					a_flags);
			});
	}

	void Controller::QueueEvaluate(
		Game::FormID                     a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_actor, a_flags]() {
			const stl::lock_guard lock(m_lock);

			ActorLookupResult result;
			if (LookupTrackedActor(a_actor, result))
			{
				Evaluate(result.actor, result.handle, a_flags);
			}
		});
	}

	void Controller::QueueEvaluateNPC(
		Game::FormID                     a_npc,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_npc, a_flags]() {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_actorMap.getvec())
			{
				if (e->second.IsActorNPCOrTemplate(a_npc))
				{
					EvaluateImpl(e->second, a_flags);
				}
			}
		});
	}

	void Controller::QueueEvaluateRace(
		Game::FormID                     a_race,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_race, a_flags]() {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_actorMap.getvec())
			{
				if (e->second.IsActorRace(a_race))
				{
					EvaluateImpl(e->second, a_flags);
				}
			}
		});
	}

	void Controller::QueueEvaluateAll(
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_flags]() {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_actorMap.getvec())
			{
				EvaluateImpl(e->second, a_flags);
			}
		});
	}

	void Controller::QueueRequestEvaluateTransformsActor(
		Game::FormID a_actor,
		bool         a_noDefer) const noexcept
	{
		ITaskPool::AddTask([this, a_actor, a_noDefer]() {
			RequestEvaluateTransformsActor(a_actor, a_noDefer);
		});
	}

	void Controller::RequestEvaluateTransformsActor(
		Game::FormID a_actor,
		bool         a_noDefer) const noexcept
	{
		const stl::lock_guard lock(m_lock);

		if (!m_nodeOverrideEnabled)
		{
			return;
		}

		auto it = m_actorMap.find(a_actor);
		if (it != m_actorMap.end())
		{
			if (a_noDefer)
			{
				it->second.RequestTransformUpdate();
			}
			else
			{
				it->second.RequestTransformUpdateDefer();
			}
		}
	}

	void Controller::RequestEvaluateTransformsNPC(
		Game::FormID a_npc,
		bool         a_noDefer) const
	{
		const stl::lock_guard lock(m_lock);

		if (!m_nodeOverrideEnabled)
		{
			return;
		}

		for (auto& e : m_actorMap.getvec())
		{
			if (e->second.IsActorNPCOrTemplate(a_npc))
			{
				if (a_noDefer)
				{
					e->second.RequestTransformUpdate();
				}
				else
				{
					e->second.RequestTransformUpdateDefer();
				}
			}
		}
	}

	void Controller::RequestEvaluateTransformsRace(
		Game::FormID a_race,
		bool         a_noDefer) const
	{
		const stl::lock_guard lock(m_lock);

		if (!m_nodeOverrideEnabled)
		{
			return;
		}

		for (auto& e : m_actorMap.getvec())
		{
			if (e->second.IsActorRace(a_race))
			{
				if (a_noDefer)
				{
					e->second.RequestTransformUpdate();
				}
				else
				{
					e->second.RequestTransformUpdateDefer();
				}
			}
		}
	}

	void Controller::RequestEvaluateTransformsAll(
		bool a_noDefer) const
	{
		const stl::lock_guard lock(m_lock);

		if (!m_nodeOverrideEnabled)
		{
			return;
		}

		for (auto& e : m_actorMap.getvec())
		{
			if (a_noDefer)
			{
				e->second.RequestTransformUpdate();
			}
			else
			{
				e->second.RequestTransformUpdateDefer();
			}
		}
	}

	void Controller::QueueActorRemove(
		TESObjectREFR*                   a_actor,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		ITaskPool::QueueLoadedActorTask(
			a_actor,
			[this, a_flags](Actor* a_actor, Game::ActorHandle a_handle) {
				RemoveActor(
					a_actor,
					static_cast<Game::ObjectRefHandle>(a_handle),
					a_flags);
			});
	}

	void Controller::QueueReset(
		TESObjectREFR*                   a_actor,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		ITaskPool::QueueLoadedActorTask(
			a_actor,
			[this, a_flags](
				Actor*            a_actor,
				Game::ActorHandle a_handle) {
				const stl::lock_guard lock(m_lock);

				ActorResetImpl(
					a_actor,
					static_cast<Game::ObjectRefHandle>(a_handle),
					a_flags);
			});
	}

	void Controller::QueueReset(
		Game::FormID                     a_actor,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		ITaskPool::AddTask([this, a_actor, a_flags]() {
			const stl::lock_guard lock(m_lock);

			ActorLookupResult result;
			if (LookupTrackedActor(a_actor, result))
			{
				ActorResetImpl(result.actor, result.handle, a_flags);
			}
		});
	}

	void Controller::QueueResetNPC(
		Game::FormID                     a_npc,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_npc, a_flags]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t actors;
			CollectKnownActors(actors);

			for (auto& e : actors)
			{
				if (auto npc = e.second->GetActorBase())
				{
					if (npc->GetFirstNonTemporaryOrThis()->formID == a_npc)
					{
						ActorResetImpl(
							e.second,
							e.first,
							a_flags);
					}
				}
			}
		});
	}

	void Controller::QueueResetRace(
		Game::FormID                     a_race,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_race, a_flags]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t actors;
			CollectKnownActors(actors);

			for (auto& e : actors)
			{
				if (auto race = e.second->GetRace())
				{
					if (race->formID == a_race)
					{
						ActorResetImpl(
							e.second,
							e.first,
							a_flags);
					}
				}
			}
		});
	}

	void Controller::QueueReset(
		Game::FormID                     a_actor,
		stl::flag<ControllerUpdateFlags> a_flags,
		ObjectSlot                       a_slot)
	{
		ITaskPool::AddTask([this, a_actor, a_flags, a_slot]() {
			const stl::lock_guard lock(m_lock);

			ActorLookupResult result;
			if (LookupTrackedActor(a_actor, result))
			{
				ActorResetImpl(
					result.actor,
					result.handle,
					a_flags,
					a_slot);
			}
		});
	}

	void Controller::QueueResetNPC(
		Game::FormID                     a_npc,
		stl::flag<ControllerUpdateFlags> a_flags,
		ObjectSlot                       a_slot)
	{
		ITaskPool::AddTask([this, a_npc, a_flags, a_slot]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t actors;
			CollectKnownActors(actors);

			for (auto& e : actors)
			{
				if (auto npc = e.second->GetActorBase())
				{
					if (npc->GetFirstNonTemporaryOrThis()->formID == a_npc)
					{
						ActorResetImpl(
							e.second,
							e.first,
							a_flags,
							a_slot);
					}
				}
			}
		});
	}

	void Controller::QueueResetRace(
		Game::FormID                     a_race,
		stl::flag<ControllerUpdateFlags> a_flags,
		ObjectSlot                       a_slot)
	{
		ITaskPool::AddTask([this, a_race, a_flags, a_slot]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t actors;
			CollectKnownActors(actors);

			for (auto& e : actors)
			{
				if (auto race = e.second->GetRace())
				{
					if (race->formID == a_race)
					{
						ActorResetImpl(
							e.second,
							e.first,
							a_flags,
							a_slot);
					}
				}
			}
		});
	}

	void Controller::CollectKnownActors(
		actorLookupResultMap_t& a_out)
	{
		for (auto& e : m_actorMap.getvec())
		{
			NiPointer<TESObjectREFR> ref;
			auto                     handle = e->second.GetHandle();

			if (handle.Lookup(ref))
			{
				if (auto actor = ref->As<Actor>())
				{
					a_out.emplace(handle, actor);
				}
			}
		}
	}

	bool Controller::SetLanguageImpl(const stl::fixed_string& a_lang)
	{
		const stl::lock_guard lock(m_lock);

		const bool result = SetLanguage(a_lang);

		if (!result)
		{
			Error("String table for language '%s' not found", a_lang.c_str());
		}

		return result;
	}

	void Controller::GenerateRandomPlacementEntries(
		const ActorObjectHolder& a_holder)
	{
		auto anpc = a_holder.m_actor->GetActorBase();
		if (!anpc)
		{
			return;
		}

		auto npc = anpc->GetFirstNonTemporaryOrThis();

		if (npc->formID.IsTemporary())
		{
			return;
		}

		if (npc->formID == IData::GetPlayerBaseID())
		{
			return;
		}

		auto& config = GetActiveConfig();

		if (config
		        .transforms.GetActorData()
		        .contains(a_holder.m_actor->formID))
		{
			return;
		}

		if (config
		        .transforms.GetNPCData()
		        .contains(npc->formID))
		{
			return;
		}

		configNodeOverrideHolder_t tmp;

		tmp.flags.set(Data::NodeOverrideHolderFlags::RandomGenerated);

		for (auto& e : NodeOverrideData::GetRandPlacementData())
		{
			auto me = e.get_rand_entry();
			if (!me)
			{
				continue;
			}

			auto& pd = tmp.get_data<configNodeOverrideEntryPlacement_t>();

			for (auto& f : pd.try_emplace(e.node).first->second())
			{
				f.targetNode = me->node;
			}

			if (!e.leftNode.empty() && !me->nodeLeft.empty())
			{
				for (auto& f : pd.try_emplace(e.leftNode).first->second())
				{
					f.targetNode = me->nodeLeft;
				}
			}

			//Debug("%X: %s | %s", npc->formID, me->node.c_str(), me->nodeLeft.c_str());
		}

		if (tmp.placementData.empty())
		{
			return;
		}

		config
			.transforms.GetNPCData()
			.emplace(npc->formID, std::move(tmp));
	}

	void Controller::OnActorAcquire(ActorObjectHolder& a_holder) noexcept
	{
		if (!EngineExtensions::HasEarly3DLoadHooks() &&
		    EngineExtensions::GetTransformOverridesEnabled())
		{
			a_holder.ApplyXP32NodeTransformOverrides();
		}

		if (GetSettings().data.placementRandomization &&
		    !a_holder.m_movNodes.empty())
		{
			GenerateRandomPlacementEntries(a_holder);
		}

		//Debug("%s: acquired: %X", __FUNCTION__, a_holder.GetActorFormID());
	}

	bool Controller::WantGlobalVariableUpdateOnAddRemove() const noexcept
	{
		return !GetActiveConfig().condvars.empty();
	}

	void Controller::QueueResetAll(
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_flags] {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				ActorResetImpl(e.second, e.first, a_flags);
			}
		});
	}

	void Controller::QueueResetAll(
		stl::flag<ControllerUpdateFlags> a_flags,
		ObjectSlot                       a_slot)
	{
		ITaskPool::AddTask([this, a_flags, a_slot] {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				ActorResetImpl(e.second, e.first, a_flags, a_slot);
			}
		});
	}

	void Controller::QueueResetGear(
		Game::FormID                     a_actor,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		ITaskPool::AddTask([this, a_actor, a_flags] {
			const stl::lock_guard lock(m_lock);

			auto& objects = GetActorMap();
			auto  it      = objects.find(a_actor);

			if (it == objects.end())
			{
				return;
			}

			NiPointer<TESObjectREFR> ref;
			auto                     handle = it->second.GetHandle();

			if (!handle.Lookup(ref))
			{
				return;
			}

			if (auto actor = ref->As<Actor>())
			{
				ResetGearImpl(actor, handle, it->second, a_flags);
			}
		});
	}

	void Controller::QueueResetGearAll(
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_flags] {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_actorMap.getvec())
			{
				NiPointer<TESObjectREFR> ref;
				auto                     handle = e->second.GetHandle();

				if (!handle.Lookup(ref))
				{
					continue;
				}

				if (auto actor = ref->As<Actor>())
				{
					ResetGearImpl(actor, handle, e->second, a_flags);
				}
			}
		});
	}

	void Controller::QueueClearObjects()
	{
		ITaskPool::AddTask([this] {
			const stl::lock_guard lock(m_lock);

			ClearObjectsImpl();
		});
	}

	void Controller::QueueResetAAAll()
	{
		ITaskPool::AddTask([this] {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_actorMap.getvec())
			{
				ActorLookupResult result;
				if (LookupTrackedActor(e->first, result))
				{
					ResetAA(result.actor, e->second.GetAnimState());
				}
			}
		});
	}

	void Controller::QueueResetCustom(
		Game::FormID             a_actor,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_actor, a_class, a_pkey, a_vkey]() {
			const stl::lock_guard lock(m_lock);

			ActorLookupResult result;
			if (LookupTrackedActor(a_actor, result))
			{
				ResetCustomImpl(
					result.actor,
					result.handle,
					a_class,
					a_pkey,
					a_vkey);
			}
		});
	}

	void Controller::QueueResetCustomNPC(
		Game::FormID             a_npc,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_npc, a_class, a_pkey, a_vkey]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				if (auto npc = e.second->GetActorBase())
				{
					if (npc->GetFirstNonTemporaryOrThis()->formID == a_npc)
					{
						ResetCustomImpl(
							e.second,
							e.first,
							a_class,
							a_pkey,
							a_vkey);
					}
				}
			}
		});
	}

	void Controller::QueueResetCustomRace(
		Game::FormID             a_race,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_race, a_class, a_pkey, a_vkey]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				if (auto race = e.second->GetRace())
				{
					if (race->formID == a_race)
					{
						ResetCustomImpl(
							e.second,
							e.first,
							a_class,
							a_pkey,
							a_vkey);
					}
				}
			}
		});
	}

	void Controller::QueueResetCustom(
		Game::FormID             a_actor,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_actor, a_class, a_pkey]() {
			const stl::lock_guard lock(m_lock);

			ActorLookupResult result;
			if (LookupTrackedActor(a_actor, result))
			{
				ResetCustomImpl(
					result.actor,
					result.handle,
					a_class,
					a_pkey);
			}
		});
	}

	void Controller::QueueResetCustomNPC(
		Game::FormID             a_npc,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_npc, a_class, a_pkey]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				if (auto npc = e.second->GetActorBase())
				{
					if (npc->GetFirstNonTemporaryOrThis()->formID == a_npc)
					{
						ResetCustomImpl(
							e.second,
							e.first,
							a_class,
							a_pkey);
					}
				}
			}
		});
	}

	void Controller::QueueResetCustomRace(
		Game::FormID             a_race,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_race, a_class, a_pkey]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				if (auto race = e.second->GetRace())
				{
					if (race->formID == a_race)
					{
						ResetCustomImpl(
							e.second,
							e.first,
							a_class,
							a_pkey);
					}
				}
			}
		});
	}

	void Controller::QueueResetCustom(
		Game::FormID a_actor,
		ConfigClass  a_class)
	{
		ITaskPool::AddTask([this, a_actor, a_class]() {
			const stl::lock_guard lock(m_lock);

			ActorLookupResult result;
			if (LookupTrackedActor(a_actor, result))
			{
				ResetCustomImpl(
					result.actor,
					result.handle,
					a_class);
			}
		});
	}

	void Controller::QueueResetCustomNPC(
		Game::FormID a_npc,
		ConfigClass  a_class)
	{
		ITaskPool::AddTask([this, a_npc, a_class]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				if (auto npc = e.second->GetActorBase())
				{
					if (npc->GetFirstNonTemporaryOrThis()->formID == a_npc)
					{
						ResetCustomImpl(
							e.second,
							e.first,
							a_class);
					}
				}
			}
		});
	}

	void Controller::QueueResetCustomRace(
		Game::FormID a_race,
		ConfigClass  a_class)
	{
		ITaskPool::AddTask([this, a_race, a_class]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				if (auto race = e.second->GetRace())
				{
					if (race->formID == a_race)
					{
						ResetCustomImpl(
							e.second,
							e.first,
							a_class);
					}
				}
			}
		});
	}

	void Controller::QueueResetCustomAll(
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_class, a_pkey, a_vkey]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				ResetCustomImpl(
					e.second,
					e.first,
					a_class,
					a_pkey,
					a_vkey);
			}
		});
	}

	void Controller::QueueResetCustomAll(
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_class, a_pkey]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				ResetCustomImpl(
					e.second,
					e.first,
					a_class,
					a_pkey);
			}
		});
	}

	void Controller::QueueResetCustomAll(
		ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_class]() {
			const stl::lock_guard lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				ResetCustomImpl(
					e.second,
					e.first,
					a_class);
			}
		});
	}

	void Controller::QueueLookupFormInfo(
		Game::FormID                     a_formId,
		IForm::form_lookup_result_func_t a_func)
	{
		ITaskPool::AddTask([this, a_formId, func = std::move(a_func)] {
			auto result = IForm::LookupFormInfo(a_formId);

			const stl::lock_guard lock(m_lock);

			func(std::move(result));
		});
	}

	void Controller::QueueLookupFormInfoCrosshairRef(
		IForm::form_lookup_result_func_t a_func)
	{
		ITaskPool::AddTask([this, func = std::move(a_func)] {
			std::unique_ptr<FormInfoResult> result;

			if (const auto ref = LookupCrosshairRef())
			{
				result = IForm::LookupFormInfo(ref->formID);
			}

			const stl::lock_guard lock(m_lock);
			func(std::move(result));
		});
	}

	void Controller::QueueGetCrosshairRef(
		std::function<void(Game::FormID)> a_func)
	{
		ITaskPool::AddTask([this, func = std::move(a_func)] {
			Game::FormID result;

			if (const auto ref = LookupCrosshairRef())
			{
				result = ref->formID;
			}

			const stl::lock_guard lock(m_lock);
			func(result);
		});
	}

	void Controller::QueueUpdateSoundForms()
	{
		ITaskPool::AddTask([this] {
			const stl::lock_guard lock(m_lock);
			UpdateSoundForms();
		});
	}

	void Controller::QueueClearRand()
	{
		ITaskPool::AddTask([this] {
			const stl::lock_guard lock(m_lock);
			IMaintenance::ClearConfigStoreRand(GetActiveConfig());
		});
	}

	void Controller::QueueSendAnimationEventToActor(
		Game::FormID a_actor,
		std::string  a_event)
	{
		ITaskPool::AddTask([this, a_actor, ev = std::move(a_event)] {
			const stl::lock_guard lock(m_lock);

			auto& data = GetActorMap();

			auto it = data.find(a_actor);
			if (it == data.end())
			{
				return;
			}

			NiPointer<TESObjectREFR> refr;

			if (!it->second.GetHandle().Lookup(refr))
			{
				return;
			}

			auto actor = refr->As<Actor>();

			if (!IsREFRValid(actor))
			{
				return;
			}

			actor->NotifyAnimationGraph(ev.c_str());
		});
	}

	void Controller::AddActorBlock(
		Game::FormID             a_actor,
		const stl::fixed_string& a_key)
	{
		const stl::lock_guard lock(m_lock);

		auto r = m_actorBlockList.data.try_emplace(a_actor);
		r.first->second.keys.emplace(a_key);

		if (r.second)
		{
			auto it = m_actorMap.find(a_actor);
			if (it != m_actorMap.end())
			{
				EvaluateImpl(it->second, ControllerUpdateFlags::kSoundAll);
			}
		}
	}

	void Controller::RemoveActorBlock(
		Game::FormID             a_actor,
		const stl::fixed_string& a_key)
	{
		const stl::lock_guard lock(m_lock);

		auto it = m_actorBlockList.data.find(a_actor);

		if (it != m_actorBlockList.data.end())
		{
			it->second.keys.erase(a_key);

			if (it->second.keys.empty())
			{
				m_actorBlockList.data.erase(it);

				auto ita = m_actorMap.find(a_actor);
				if (ita != m_actorMap.end())
				{
					EvaluateImpl(ita->second, ControllerUpdateFlags::kSoundAll);
				}
			}
		}
	}

	void Controller::TogglePlayerBlock()
	{
		auto player = *g_thePlayer;
		if (!player)
		{
			return;
		}

		const stl::lock_guard lock(m_lock);

		m_actorBlockList.playerToggle = !m_actorBlockList.playerToggle;

		if (m_actorBlockList.playerToggle)
		{
			AddActorBlock(
				player->formID,
				StringHolder::GetSingleton().IED);
		}
		else
		{
			RemoveActorBlock(player->formID, StringHolder::GetSingleton().IED);
		}
	}

	bool Controller::IsActorBlocked(Game::FormID a_actor) const
	{
		const stl::lock_guard lock(m_lock);

		return IsActorBlockedImpl(a_actor);
	}

	bool Controller::IsActorBlockedImpl(Game::FormID a_actor) const
	{
		if (!m_actorBlockList.data.empty())
		{
			auto it = m_actorBlockList.data.find(a_actor);
			if (it == m_actorBlockList.data.end())
			{
				return false;
			}
			else
			{
				return !it->second.keys.empty();
			}
		}

		return false;
	}

	bool Controller::SkeletonCheck(Game::FormID a_actor)
	{
		auto it = m_actorMap.find(a_actor);
		if (it == m_actorMap.end())
		{
			return true;
		}

		return !it->second.HasHumanoidSkeleton() ||
		       it->second.HasXP32Skeleton();
	}

	bool Controller::DoItemUpdate(
		ProcessParams&            a_params,
		const configBaseValues_t& a_config,
		ObjectEntryBase&          a_entry,
		bool                      a_visible,
		TESForm*                  a_currentModelForm) noexcept
	{
		auto& state = a_entry.data.state;

		if (state->resetTriggerFlags !=
		    (a_config.flags & BaseFlags::kResetTriggerFlags))
		{
			return false;
		}

		if (!state->flags.test(ObjectEntryFlags::kIsGroup))
		{
			if (a_config.flags.test(BaseFlags::kPlaySequence))
			{
				if (state->anim.holder)
				{
					return false;
				}
			}
			else
			{
				if (!state->currentSequence.empty())
				{
					return false;
				}
			}

			if (a_currentModelForm != state->modelForm)
			{
				return false;
			}

			if (state->flags.test(ObjectEntryFlags::kHasCollisionObjectScale))
			{
				if (state->currentGeomTransformTag != a_config.geometryTransform &&
				    a_config.geometryTransform.scale != state->colliderScale)
				{
					return false;
				}
			}

			if (state->light && state->light->extraLightData != a_config.extraLightConfig.data)
			{
				return false;
			}
		}

		const bool isVisible = !state->flags.test(ObjectEntryFlags::kInvisible);

		if (a_visible)
		{
			if (!isVisible)
			{
				if (a_params.flags.test(ControllerUpdateFlags::kVisibilitySounds))
				{
					PlayEquipObjectSound(a_params, a_config, a_entry, true);
				}

				const auto ts = IPerfCounter::Query();

				state->visit_db_entries([&](auto& a_entry) noexcept {
					a_entry->accessed = ts;
				});
			}
		}
		else
		{
			if (a_params.flags.test(ControllerUpdateFlags::kVisibilitySounds))
			{
				if (isVisible)
				{
					PlayEquipObjectSound(a_params, a_config, a_entry, false);
				}
			}
		}

		const bool isLightHidden = state->flags.test(ObjectEntryFlags::kHideLight);

		state->UpdateFlags(a_config);

		if (isVisible != a_visible ||
		    isLightHidden != a_config.flags.test(BaseFlags::kHideLight))
		{
			if (!a_entry.SetObjectVisible(a_visible))
			{
				state->SetLightsVisible(a_visible);
			}

			a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
		}

		bool updateTransform = false;

		if (state->nodeDesc.name != a_config.targetNode.name)
		{
			if (!AttachNodeImpl(
					a_params.npcRoot,
					a_config.targetNode,
					a_config.flags.test(BaseFlags::kReferenceMode),
					a_entry))
			{
				return false;
			}

			updateTransform = true;
		}

		if (state->transform != static_cast<const configTransform_t&>(a_config))
		{
			state->transform.Update(a_config);

			updateTransform = true;
		}

		if (updateTransform)
		{
			INode::UpdateObjectTransform(
				state->transform,
				state->commonNodes.rootNode,
				state->refNode);

			a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
		}

		if (!state->flags.test(ObjectEntryFlags::kIsGroup))
		{
			if (const bool wantHidden = a_config.flags.test(BaseFlags::kHideGeometry);
			    wantHidden != state->commonNodes.object->IsHidden())
			{
				state->commonNodes.object->SetHidden(wantHidden);
			}

			if (state->currentGeomTransformTag != a_config.geometryTransform)
			{
				INode::UpdateObjectTransform(
					a_config.geometryTransform,
					state->commonNodes.object);

				state->currentGeomTransformTag.emplace(a_config.geometryTransform);

				a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
			}

			if (a_config.flags.test(BaseFlags::kPlaySequence))
			{
				state->UpdateAndPlayAnimationSequence(
					a_params.actor,
					a_config.niControllerSequence);
			}
			else if (state->anim.holder)
			{
				const auto& currentEvent =
					a_config.flags.test(Data::BaseFlags::kAnimationEvent) ?
						a_config.animationEvent :
						StringHolder::GetSingleton().weaponSheathe;

				state->anim.UpdateAndSendAnimationEvent(currentEvent);
			}

			if (state->sound.form)
			{
				if (!a_config.flags.test(BaseFlags::kPlayLoopSound))
				{
					if (state->sound.handle.IsValid())
					{
						state->sound.handle.StopAndReleaseNow();
					}
				}
				else
				{
					if (!state->sound.handle.IsValid())
					{
						TryInitializeAndPlayLoopSound(
							a_params.actor,
							state->sound);
					}
				}
			}
		}
		else
		{
			if (!a_config.flags.test(BaseFlags::kPlayLoopSound))
			{
				for (auto& e : state->groupObjects)
				{
					auto& object = e.second;

					if (!object.sound.form)
					{
						continue;
					}

					if (object.sound.handle.IsValid())
					{
						object.sound.handle.StopAndReleaseNow();
					}
				}
			}
			else
			{
				for (auto& e : state->groupObjects)
				{
					auto& object = e.second;

					if (!object.sound.form)
					{
						continue;
					}

					if (!object.sound.handle.IsValid())
					{
						TryInitializeAndPlayLoopSound(
							a_params.actor,
							object.sound);
					}
				}
			}
		}

		if (state->HasPhysicsNode())
		{
			auto& simComponent = state->simComponent;

			if (a_visible &&
			    a_config.physicsValues &&
			    !a_config.physicsValues->valueFlags.test(Data::ConfigNodePhysicsFlags::kDisabled))
			{
				const auto& conf = *a_config.physicsValues.data;

				if (!simComponent)
				{
					simComponent = a_params.objects.CreateAndAddSimComponent(
						state->physicsNode.get(),
						state->physicsNode->m_localTransform,
						conf);
				}
				else if (simComponent->GetConfig() != conf)
				{
					simComponent->UpdateConfig(conf);
				}
			}
			else if (simComponent)
			{
				a_params.objects.RemoveAndDestroySimComponent(simComponent);
			}
		}

		return true;
	}

	void Controller::ResetEffectShaderData(
		ProcessParams&   a_params,
		ObjectEntryBase& a_entry) noexcept
	{
		if (auto& data = a_entry.data.effectShaderData)
		{
			data->ClearEffectShaderDataFromTree(a_params.objects.m_root);
			data->ClearEffectShaderDataFromTree(a_params.objects.m_root1p);

			data.reset();

			a_params.SuspendReferenceEffectShaders();
		}
	}

	void Controller::ResetEffectShaderData(
		ProcessParams&   a_params,
		ObjectEntryBase& a_entry,
		NiAVObject*      a_object) noexcept
	{
		if (auto& data = a_entry.data.effectShaderData)
		{
			data->ClearEffectShaderDataFromTree(a_object);

			data.reset();

			a_params.SuspendReferenceEffectShaders();
		}
	}

	void Controller::UpdateObjectEffectShaders(
		ProcessParams&              a_params,
		const Data::configCustom_t& a_config,
		ObjectEntryCustom&          a_objectEntry) noexcept
	{
		if (!ShaderProcessingEnabled() ||
		    !a_objectEntry.data.state)
		{
			return;
		}

		auto es = a_config.get_effect_shader_sfp(
			{ a_objectEntry.data.state->form },
			a_params);

		if (es)
		{
			if (!a_objectEntry.data.effectShaderData)
			{
				a_objectEntry.data.effectShaderData = std::make_unique<EffectShaderData>(
					a_params.objects,
					*es);

				a_params.SuspendReferenceEffectShaders();
			}
			else
			{
				if (a_objectEntry.data.effectShaderData->UpdateIfChanged(
						a_params.objects,
						a_objectEntry.data.state->commonNodes.rootNode,
						*es))
				{
					a_params.SuspendReferenceEffectShaders();
				}
				else if (a_params.flags.test(ControllerUpdateFlags::kWantEffectShaderConfigUpdate))
				{
					if (!a_objectEntry.data.effectShaderData->UpdateConfigValues(*es))
					{
						a_objectEntry.data.effectShaderData->Update(
							a_params.objects,
							a_objectEntry.data.state->commonNodes.rootNode,
							*es);

						a_params.SuspendReferenceEffectShaders();
					}
				}
			}
		}
		else
		{
			ResetEffectShaderData(
				a_params,
				a_objectEntry,
				a_objectEntry.data.state->commonNodes.rootNode);
		}
	}

	void Controller::UpdateCustomGroup(
		ProcessParams&              a_params,
		const Data::configCustom_t& a_config,
		ObjectEntryCustom&          a_objectEntry) noexcept
	{
		for (auto& e : a_config.group.entries)
		{
			if (auto it = a_objectEntry.data.state->groupObjects.find(e.first);
			    it != a_objectEntry.data.state->groupObjects.end())
			{
				if (it->second.transform != e.second.transform)
				{
					it->second.transform.Update(e.second.transform);

					INode::UpdateObjectTransform(
						it->second.transform,
						it->second.commonNodes.rootNode,
						nullptr);

					a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
				}

				if (it->second.anim.holder)
				{
					if (e.second.flags.test(
							Data::ConfigModelGroupEntryFlags::kAnimationEvent))
					{
						it->second.anim.UpdateAndSendAnimationEvent(
							e.second.animationEvent);
					}
					else
					{
						it->second.anim.UpdateAndSendAnimationEvent(
							StringHolder::GetSingleton().weaponSheathe);
					}
				}
			}
		}
	}

	void Controller::RemoveSlotObjectEntry(
		ProcessParams&   a_params,
		ObjectEntrySlot& a_entry,
		bool             a_removeCloningTask,
		bool             a_noSound) noexcept
	{
		const auto flags =
			a_noSound ?
				a_params.flags.value & ~ControllerUpdateFlags::kPlayEquipSound :
				a_params.flags.value;

		if (RemoveObject(
				a_params.actor,
				a_params.handle,
				a_entry,
				a_params.objects,
				flags,
				false,
				a_removeCloningTask))
		{
			a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);

			a_params.mark_slot_presence_change(a_entry.slotid);
		}
	}

	void Controller::ProcessSlots(ProcessParams& a_params) noexcept
	{
		const auto& settings = GetSettings().data;

		a_params.collector.GenerateSlotCandidates(
			a_params.objects.IsPlayer(),
			a_params.objects.IsPlayer() &&
				!settings.removeFavRestriction);

		const auto equippedInfo = CreateEquippedItemInfo(a_params.actor);

		SaveLastEquippedItems(
			a_params,
			equippedInfo,
			a_params.objects);

		configStoreSlot_t::holderCache_t hc;

		const configSlotPriority_t* prio;

		const auto& activeConfig = GetActiveConfig();

		if (auto d = activeConfig.slot.GetActorPriority(
				a_params.actor->formID,
				a_params.npcOrTemplate->formID,
				a_params.race->formID,
				hc))
		{
			prio = std::addressof(d->get(a_params.get_sex()));
		}
		else
		{
			prio = nullptr;
		}

		std::uint32_t activeTypes = 0;

		using enum_type = std::underlying_type_t<ObjectType>;

		struct type_entry
		{
			struct slot_entry
			{
				ObjectSlot                                   slot;
				bool                                         equipped;
				TESForm*                                     equippedForm;
				mutable const configSlotHolder_t::data_type* slotConfig;
			};

			ObjectType type;
			bool       activeEquipped;
			slot_entry slots[2];
		};

		type_entry types[stl::underlying(ObjectType::kMax)];

		for (enum_type i = 0; i < stl::underlying(ObjectType::kMax); i++)
		{
			auto& e = types[i];

			const auto type = prio ?
			                      prio->translate_type_safe(i) :
			                      static_cast<ObjectType>(i);

			e.type           = type;
			e.activeEquipped = false;

			const auto mainSlot = ItemData::GetSlotFromType(type);

			if (const auto leftSlot = ItemData::GetLeftSlot(mainSlot);
			    leftSlot < ObjectSlot::kMax)
			{
				const auto& cer = a_params.objects.GetSlot(mainSlot);
				const auto& cel = a_params.objects.GetSlot(leftSlot);

				if (cel.slotState.lastEquipped &&
				    (!cer.slotState.lastEquipped ||
				     cer.slotState.lastSeenEquipped < cel.slotState.lastSeenEquipped))
				{
					e.slots[0].slot = leftSlot;
					e.slots[1].slot = mainSlot;
				}
				else
				{
					e.slots[0].slot = mainSlot;
					e.slots[1].slot = leftSlot;
				}
			}
			else
			{
				e.slots[0].slot = mainSlot;
				e.slots[1].slot = ObjectSlot::kMax;
			}

			bool typeActive = false;

			for (auto& slot : e.slots)
			{
				if (slot.slot >= ObjectSlot::kMax)
				{
					continue;
				}

				slot.slotConfig = nullptr;

				if (slot.slot == equippedInfo.leftSlot)
				{
					slot.equipped     = true;
					slot.equippedForm = equippedInfo.left;
				}
				else if (slot.slot == equippedInfo.rightSlot)
				{
					slot.equipped     = true;
					slot.equippedForm = equippedInfo.right;
				}
				else if (
					slot.slot == ObjectSlot::kAmmo &&
					a_params.collector.data.IsSlotEquipped(ObjectSlotExtra::kAmmo))
				{
					if (const auto* const biped = a_params.get_biped())
					{
						auto& o = biped->get_object(BIPED_OBJECT::kQuiver);

						slot.equippedForm = (o.item &&
						                     o.item != o.addon &&
						                     o.item->IsAmmo()) ?
						                        o.item :
						                        nullptr;
					}
					else
					{
						slot.equippedForm = nullptr;
					}

					slot.equipped = true;
				}
				else
				{
					slot.equipped     = false;
					slot.equippedForm = nullptr;
				}

				if (slot.equipped && prio && prio->flags.test(SlotPriorityFlags::kAccountForEquipped))
				{
					e.activeEquipped = true;
					typeActive       = true;
				}
			}

			if (typeActive)
			{
				activeTypes++;
			}
		}

		const auto limit = prio ?
		                       prio->limit :
		                       stl::underlying(ObjectType::kMax);

		for (const auto& e : types)
		{
			const auto equipmentFlag = ItemData::GetRaceEquipmentFlagFromType(e.type);

			auto& candidates = a_params.collector.GetCandidates(e.type);

			bool typeActive = false;

			for (auto& f : e.slots)
			{
				if (f.slot >= ObjectSlot::kMax)
				{
					continue;
				}

				auto& objectEntry = a_params.objects.GetSlot(f.slot);

				if (!e.activeEquipped && activeTypes >= limit)
				{
					RemoveSlotObjectEntry(
						a_params,
						objectEntry);

					continue;
				}

				auto entry = activeConfig.slot.GetActor(
					a_params.actor->formID,
					a_params.npcOrTemplate->formID,
					a_params.race->formID,
					f.slot,
					hc);

				f.slotConfig = entry;

				if (!entry)
				{
					RemoveSlotObjectEntry(
						a_params,
						objectEntry);

					continue;
				}

				auto& configEntry = entry->get(a_params.get_sex());

				if (!configEntry.run_filters(a_params))
				{
					RemoveSlotObjectEntry(
						a_params,
						objectEntry);

					continue;
				}

				const auto item = SelectSlotItem(
					a_params,
					configEntry,
					candidates,
					objectEntry);

				const auto configOverride =
					!item ? configEntry.get_equipment_override(
								a_params) :
							configEntry.get_equipment_override_fp(
								{ item->item->form, objectEntry.slotidex, objectEntry.slotid },
								a_params);

				const auto& usedBaseConf =
					configOverride ?
						static_cast<const configBaseValues_t&>(*configOverride) :
						configEntry;

				if (usedBaseConf.flags.test(BaseFlags::kDisabled) ||
				    (!a_params.is_player() &&
				     !usedBaseConf.flags.test(BaseFlags::kIgnoreRaceEquipTypes) &&
				     f.slot != ObjectSlot::kAmmo &&
				     !a_params.test_equipment_flags(equipmentFlag)))
				{
					RemoveSlotObjectEntry(
						a_params,
						objectEntry);

					continue;
				}

				if (f.equipped)
				{
					if (settings.hideEquipped &&
					    !configEntry.slotFlags.test(SlotFlags::kAlwaysUnload)

					    /*
							If something got equipped while the cloning task was still pending, 
							keep the cloned object since it may still be valid on next load
						*/

					    // && !objectEntry.data.cloningTask

					)
					{
						if (objectEntry.DeferredHideObject(2))
						{
							a_params.mark_slot_presence_change(objectEntry.slotid);
						}
					}
					else
					{
						RemoveSlotObjectEntry(
							a_params,
							objectEntry);
					}

					continue;
				}

				if (!item)
				{
					RemoveSlotObjectEntry(
						a_params,
						objectEntry);

					continue;
				}

				objectEntry.ResetDeferredHide();

				const bool visible = GetVisibilitySwitch(
					a_params.actor,
					usedBaseConf.flags,
					a_params);

				const auto itemData  = item->item;
				const auto modelForm = usedBaseConf.forceModel.get_form();

				if (objectEntry.data.state &&
				    objectEntry.data.state->form == itemData->form)
				{
					const bool isVisible = objectEntry.IsNodeVisible();

					if (DoItemUpdate(
							a_params,
							usedBaseConf,
							objectEntry,
							visible,
							modelForm))
					{
						objectEntry.data.state->UpdateArrows(
							usedBaseConf.flags.test(BaseFlags::kDynamicArrows) ?
								itemData->itemCount :
								BSStringHolder::NUM_DYN_ARROWS);

						if (visible)
						{
							itemData->consume_one();
						}

						item.consume(candidates);

						typeActive |= visible;

						if (visible != isVisible)
						{
							a_params.mark_slot_presence_change(objectEntry.slotid);
						}

						continue;
					}
				}

				RemoveSlotObjectEntry(
					a_params,
					objectEntry,
					false,
					true);

				const auto attachResult = LoadAndAttach(
					a_params,
					usedBaseConf,
					configEntry,
					objectEntry,
					itemData->form,
					modelForm,
					ItemData::IsLeftWeaponSlot(f.slot),
					visible,
					false,
					PhysicsProcessingEnabled());

				switch (attachResult)
				{
				case AttachObjectResult::kSucceeded:

					objectEntry.SetObjectVisible(visible);

					objectEntry.data.state->UpdateArrows(
						usedBaseConf.flags.test(BaseFlags::kDynamicArrows) ?
							itemData->itemCount :
							BSStringHolder::NUM_DYN_ARROWS);

					objectEntry.slotState.insert_last_slotted(
						objectEntry.data.state->form->formID,
						m_bipedCache.max_forms());

					objectEntry.slotState.lastOccupied = GetCounterValue();

					a_params.state.flags.set(
						ProcessStateUpdateFlags::kMenuUpdate |
						ProcessStateUpdateFlags::kObjectAttached);

					a_params.mark_slot_presence_change(objectEntry.slotid);

					[[fallthrough]];

				case AttachObjectResult::kPending:

					if (visible)
					{
						itemData->consume_one();
					}

					item.consume(candidates);

					typeActive |= visible;

					break;

				case AttachObjectResult::kFailed:

					objectEntry.data.TryCancelAndReleaseCloningTask();

					break;
				}

				// Debug("%X: (%.8X) attached | %u ", a_actor->formID, item->formID, slot);
			}

			if (!e.activeEquipped && typeActive)
			{
				activeTypes++;
			}
		}

		if (ShaderProcessingEnabled())
		{
			for (const auto& e : types)
			{
				for (auto& f : e.slots)
				{
					if (f.slot >= ObjectSlot::kMax)
					{
						continue;
					}

					auto& objectEntry = a_params.objects.GetSlot(f.slot);

					if (!f.slotConfig)
					{
						f.slotConfig = activeConfig.slot.GetActor(
							a_params.actor->formID,
							a_params.npcOrTemplate->formID,
							a_params.race->formID,
							f.slot,
							hc);

						if (!f.slotConfig)
						{
							ResetEffectShaderData(
								a_params,
								objectEntry);

							continue;
						}
					}

					auto& configEntry = f.slotConfig->get(a_params.get_sex());

					const configEffectShaderHolder_t* es       = nullptr;
					const auto                        equipped = static_cast<bool>(f.equippedForm);

					if (equipped)
					{
						es = configEntry.get_effect_shader_sfp(
							{ f.equippedForm },
							a_params);
					}
					else if (objectEntry.data.state)
					{
						es = configEntry.get_effect_shader_sfp(
							{ objectEntry.data.state->form, objectEntry.slotidex, objectEntry.slotid },
							a_params);
					}

					if (!es)
					{
						ResetEffectShaderData(
							a_params,
							objectEntry);

						continue;
					}

					if (!objectEntry.data.effectShaderData)
					{
						const auto bipedObject = ItemData::SlotToBipedObject(a_params.actor, f.slot);

						std::pair<NiNode*, NiNode*> nodes{ nullptr, nullptr };

						if (bipedObject != BIPED_OBJECT::kNone)
						{
							a_params.objects.GetSheathNodes(f.slot, nodes);
						}

						objectEntry.data.effectShaderData =
							std::make_unique<EffectShaderData>(
								a_params.objects,
								bipedObject,
								nodes.first,
								nodes.second,
								*es);

						a_params.SuspendReferenceEffectShaders();
					}
					else
					{
						if (objectEntry.data.effectShaderData->UpdateIfChanged(a_params.objects, *es))
						{
							a_params.SuspendReferenceEffectShaders();
						}
						else if (a_params.flags.test(ControllerUpdateFlags::kWantEffectShaderConfigUpdate))
						{
							if (!objectEntry.data.effectShaderData->UpdateConfigValues(*es))
							{
								objectEntry.data.effectShaderData->Update(
									a_params.objects,
									*es);

								a_params.SuspendReferenceEffectShaders();
							}
						}
					}

					objectEntry.data.effectShaderData->targettingEquipped = equipped;
				}
			}
		}
	}

	bool Controller::GetVisibilitySwitch(
		Actor*               a_actor,
		stl::flag<BaseFlags> a_flags,
		ProcessParams&       a_params) noexcept
	{
		if (a_flags.test(BaseFlags::kInvisible))
		{
			return false;
		}

		if (a_flags.test(BaseFlags::kHideIfUsingFurniture) &&
		    a_params.is_using_furniture())
		{
			return false;
		}

		if (a_flags.test(BaseFlags::kHideLayingDown) &&
		    a_params.get_laying_down())
		{
			return false;
		}

		return true;
	}

	bool Controller::LookupTrackedActor(
		Game::FormID       a_actor,
		ActorLookupResult& a_out) noexcept
	{
		auto it = m_actorMap.find(a_actor);
		if (it == m_actorMap.end())
		{
			return false;
		}

		return LookupTrackedActor(it->second, a_out);
	}

	bool Controller::LookupTrackedActor(
		const ActorObjectHolder& a_record,
		ActorLookupResult&       a_out) noexcept
	{
		auto handle = a_record.GetHandle();

		NiPointer<TESObjectREFR> refr;
		if (!handle.Lookup(refr))
		{
			return false;
		}

		if (auto actor = refr->As<Actor>())
		{
			a_out = { actor, handle };
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Controller::IsBlockedByChance(
		ProcessParams&        a_params,
		const configCustom_t& a_config,
		ObjectEntryCustom&    a_objectEntry) noexcept
	{
		if (a_config.customFlags.test(CustomFlags::kUseChance))
		{
			if (!a_objectEntry.cflags.test(CustomObjectEntryFlags::kProcessedChance))
			{
				if (a_config.probability >= 100.0f || GetRandomPercent() < a_config.probability)
				{
					a_objectEntry.cflags.clear(CustomObjectEntryFlags::kBlockedByChance);
				}
				else
				{
					a_objectEntry.cflags.set(CustomObjectEntryFlags::kBlockedByChance);
				}

				a_objectEntry.cflags.set(CustomObjectEntryFlags::kProcessedChance);
			}

			return a_objectEntry.cflags.test(CustomObjectEntryFlags::kBlockedByChance);
		}
		else
		{
			a_objectEntry.clear_chance_flags();

			return false;
		}
	}

	ActorObjectHolder* Controller::SelectCustomFormVariableSourceHolder(
		Game::FormID   a_id,
		ProcessParams& a_params) noexcept
	{
		if (a_id == a_params.objects.GetActorFormID())
		{
			return std::addressof(a_params.objects);
		}
		else
		{
			auto& data = GetActorMap();

			auto it = data.find(a_id);
			if (it != data.end())
			{
				return std::addressof(it->second);
			}
			else
			{
				return nullptr;
			}
		}
	}

	ActorObjectHolder* Controller::SelectCustomFormVariableSource(
		ProcessParams&              a_params,
		const Data::configCustom_t& a_config) noexcept
	{
		switch (a_config.varSource.source)
		{
		case VariableSource::kActor:

			return SelectCustomFormVariableSourceHolder(a_config.varSource.form, a_params);

		case VariableSource::kPlayerHorse:

			if (const auto& actor = a_params.get_last_ridden_player_horse())
			{
				return SelectCustomFormVariableSourceHolder(actor->formID, a_params);
			}

			break;

		case VariableSource::kMountingActor:

			if (const auto& actor = a_params.get_mounting_actor())
			{
				return SelectCustomFormVariableSourceHolder(actor->formID, a_params);
			}

			break;

		case VariableSource::kMountedActor:

			if (const auto& actor = a_params.get_mounted_actor())
			{
				return SelectCustomFormVariableSourceHolder(actor->formID, a_params);
			}

			break;

		case VariableSource::kSelf:

			return SelectCustomFormVariableSourceHolder(a_params.objects.GetActorFormID(), a_params);
		}

		return {};
	}

	const configCachedForm_t* Controller::SelectCustomForm(
		ProcessParams&        a_params,
		const configCustom_t& a_config) noexcept
	{
		if (a_config.customFlags.test(CustomFlags::kVariableMode))
		{
			const auto holder = SelectCustomFormVariableSource(a_params, a_config);
			if (!holder)
			{
				return nullptr;
			}

			auto& vars = holder->GetVariables();

			for (auto& e : a_config.formVars)
			{
				auto it = vars.find(e);
				if (it != vars.end())
				{
					if (it->second.type == ConditionalVariableType::kForm)
					{
						auto& form = it->second.form;
						if (form.get_id() &&
						    !form.get_id().IsTemporary())
						{
							return std::addressof(form);
						}
					}
				}
			}
		}
		else
		{
			auto& result = a_config.form;

			if (result.get_id() &&
			    !result.get_id().IsTemporary())
			{
				return std::addressof(result);
			}
		}

		return nullptr;
	}

	AttachObjectResult Controller::ProcessCustomEntry(
		ProcessParams&        a_params,
		const configCustom_t& a_config,
		ObjectEntryCustom&    a_objectEntry) noexcept
	{
		if (a_config.equipmentOverrides.empty() &&
		    a_config.flags.test(BaseFlags::kDisabled))
		{
			a_objectEntry.clear_chance_flags();
			return AttachObjectResult::kFailed;
		}

		if (a_config.customFlags.test(CustomFlags::kIgnorePlayer) &&
		    a_params.actor == *g_thePlayer)
		{
			a_objectEntry.clear_chance_flags();
			return AttachObjectResult::kFailed;
		}

		if (!a_config.run_filters(a_params))
		{
			a_objectEntry.clear_chance_flags();
			return AttachObjectResult::kFailed;
		}

		if (a_config.customFlags.test_any(CustomFlags::kIsInInventoryMask))
		{
			bool hasMinCount;

			auto it = CustomEntrySelectInventoryForm(
				a_params,
				a_config,
				a_objectEntry,
				hasMinCount);

			if (it == a_params.collector.data.forms.end())
			{
				a_objectEntry.clear_chance_flags();
				return AttachObjectResult::kFailed;
			}

			if (a_config.customFlags.test_any(CustomFlags::kEquipmentModeMask) &&
			    !hasMinCount &&
			    (!GetSettings().data.hideEquipped ||
			     a_config.customFlags.test(CustomFlags::kAlwaysUnload)))
			{
				a_objectEntry.clear_chance_flags();
				return AttachObjectResult::kFailed;
			}

			const auto& itemData = it->second;

			const auto configOverride =
				a_config.get_equipment_override_sfp(
					{ itemData.form },
					a_params);

			const auto& usedBaseConf =
				configOverride ?
					static_cast<const configBaseValues_t&>(*configOverride) :
					a_config;

			if (usedBaseConf.flags.test(BaseFlags::kDisabled))
			{
				a_objectEntry.clear_chance_flags();
				return AttachObjectResult::kFailed;
			}

			if (IsBlockedByChance(
					a_params,
					a_config,
					a_objectEntry))
			{
				return AttachObjectResult::kFailed;
			}

			const bool visible = GetVisibilitySwitch(
				a_params.actor,
				usedBaseConf.flags,
				a_params);

			const auto modelForm =
				usedBaseConf.forceModel.get_id() ?
					usedBaseConf.forceModel.get_form() :
					a_config.modelForm.get_form();

			if (a_objectEntry.data.state &&
			    a_objectEntry.data.state->form == itemData.form)
			{
				if (a_config.customFlags.test(CustomFlags::kGroupMode) ==
				    a_objectEntry.cflags.test(CustomObjectEntryFlags::kGroupMode))
				{
					const bool _visible = hasMinCount && visible;

					if (DoItemUpdate(
							a_params,
							usedBaseConf,
							a_objectEntry,
							_visible,
							modelForm))
					{
						a_objectEntry.data.state->UpdateArrows(
							usedBaseConf.flags.test(BaseFlags::kDynamicArrows) ?
								itemData.itemCount :
								BSStringHolder::NUM_DYN_ARROWS);

						if (_visible)
						{
							if (a_config.customFlags.test_any(CustomFlags::kEquipmentModeMask))
							{
								itemData.consume_one();
							}
						}

						UpdateObjectEffectShaders(
							a_params,
							a_config,
							a_objectEntry);

						if (a_objectEntry.cflags.test(CustomObjectEntryFlags::kGroupMode) &&
						    a_params.flags.test(ControllerUpdateFlags::kWantGroupUpdate))
						{
							UpdateCustomGroup(a_params, a_config, a_objectEntry);
						}

						return AttachObjectResult::kSucceeded;
					}
				}
			}

			if (!hasMinCount)
			{
				return AttachObjectResult::kFailed;
			}

			if (RemoveObject(
					a_params.actor,
					a_params.handle,
					a_objectEntry,
					a_params.objects,
					a_params.flags & ~ControllerUpdateFlags::kPlayEquipSound,
					false,
					false))
			{
				a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
			}

			AttachObjectResult result;

			if (a_config.customFlags.test(CustomFlags::kGroupMode))
			{
				result = LoadAndAttachGroup(
					a_params,
					usedBaseConf,
					a_config,
					a_config.group,
					a_objectEntry,
					itemData.form,
					a_config.customFlags.test(CustomFlags::kLeftWeapon),
					visible,
					a_config.customFlags.test(CustomFlags::kDisableHavok),
					PhysicsProcessingEnabled());

				a_objectEntry.cflags.set(CustomObjectEntryFlags::kGroupMode);
			}
			else
			{
				result = LoadAndAttach(
					a_params,
					usedBaseConf,
					a_config,
					a_objectEntry,
					itemData.form,
					modelForm,
					a_config.customFlags.test(CustomFlags::kLeftWeapon),
					visible,
					a_config.customFlags.test(CustomFlags::kDisableHavok),
					PhysicsProcessingEnabled());

				a_objectEntry.cflags.clear(CustomObjectEntryFlags::kGroupMode);
			}

			switch (result)
			{
			case AttachObjectResult::kSucceeded:

				a_objectEntry.data.state->UpdateArrows(
					usedBaseConf.flags.test(BaseFlags::kDynamicArrows) ?
						itemData.itemCount :
						BSStringHolder::NUM_DYN_ARROWS);

				a_objectEntry.SetObjectVisible(visible);

				UpdateObjectEffectShaders(
					a_params,
					a_config,
					a_objectEntry);

				a_params.state.flags.set(
					ProcessStateUpdateFlags::kMenuUpdate |
					ProcessStateUpdateFlags::kObjectAttached);

				[[fallthrough]];

			case AttachObjectResult::kPending:

				if (a_config.customFlags.test_any(CustomFlags::kEquipmentModeMask) && visible)
				{
					itemData.consume_one();
				}

				break;
			}

			return result;
		}
		else
		{
			const auto cform = SelectCustomForm(a_params, a_config);

			if (!cform)
			{
				a_objectEntry.clear_chance_flags();

				return AttachObjectResult::kFailed;
			}

			const auto form = cform->get_form();
			if (!form)
			{
				Debug(
					"%s: [%.8X] couldn't find form %.8X",
					__FUNCTION__,
					a_params.actor->formID.get(),
					cform->get_id().get());

				a_objectEntry.clear_chance_flags();

				return AttachObjectResult::kFailed;
			}

			auto configOverride =
				a_config.get_equipment_override_sfp(
					{ form },
					a_params);

			const auto& usedBaseConf =
				configOverride ?
					static_cast<const configBaseValues_t&>(*configOverride) :
					a_config;

			if (usedBaseConf.flags.test(BaseFlags::kDisabled))
			{
				a_objectEntry.clear_chance_flags();
				return AttachObjectResult::kFailed;
			}

			if (IsBlockedByChance(
					a_params,
					a_config,
					a_objectEntry))
			{
				return AttachObjectResult::kFailed;
			}

			const bool visible = GetVisibilitySwitch(
				a_params.actor,
				usedBaseConf.flags,
				a_params);

			const auto modelForm = usedBaseConf.forceModel.get_form();

			if (a_objectEntry.data.state &&
			    a_objectEntry.data.state->form == form)
			{
				if (a_config.customFlags.test(CustomFlags::kGroupMode) ==
				    a_objectEntry.cflags.test(CustomObjectEntryFlags::kGroupMode))
				{
					if (DoItemUpdate(
							a_params,
							usedBaseConf,
							a_objectEntry,
							visible,
							modelForm))
					{
						UpdateObjectEffectShaders(
							a_params,
							a_config,
							a_objectEntry);

						if (a_objectEntry.cflags.test(CustomObjectEntryFlags::kGroupMode) &&
						    a_params.flags.test(ControllerUpdateFlags::kWantGroupUpdate))
						{
							UpdateCustomGroup(a_params, a_config, a_objectEntry);
						}

						return AttachObjectResult::kSucceeded;
					}
				}
			}

			if (RemoveObject(
					a_params.actor,
					a_params.handle,
					a_objectEntry,
					a_params.objects,
					a_params.flags & ~ControllerUpdateFlags::kPlayEquipSound,
					false,
					false))
			{
				a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
			}

			AttachObjectResult result;

			if (a_config.customFlags.test(CustomFlags::kGroupMode))
			{
				result = LoadAndAttachGroup(
					a_params,
					usedBaseConf,
					a_config,
					a_config.group,
					a_objectEntry,
					form,
					a_config.customFlags.test(CustomFlags::kLeftWeapon),
					visible,
					a_config.customFlags.test(CustomFlags::kDisableHavok),
					PhysicsProcessingEnabled());

				a_objectEntry.cflags.set(CustomObjectEntryFlags::kGroupMode);
			}
			else
			{
				result = LoadAndAttach(
					a_params,
					usedBaseConf,
					a_config,
					a_objectEntry,
					form,
					modelForm,
					a_config.customFlags.test(CustomFlags::kLeftWeapon),
					visible,
					a_config.customFlags.test(CustomFlags::kDisableHavok),
					PhysicsProcessingEnabled());

				a_objectEntry.cflags.clear(CustomObjectEntryFlags::kGroupMode);
			}

			if (result == AttachObjectResult::kSucceeded)
			{
				a_objectEntry.SetObjectVisible(visible);

				UpdateObjectEffectShaders(
					a_params,
					a_config,
					a_objectEntry);

				a_params.state.flags.set(
					ProcessStateUpdateFlags::kMenuUpdate |
					ProcessStateUpdateFlags::kObjectAttached);
			}

			return result;
		}
	}

	void Controller::ProcessCustomEntryMap(
		ProcessParams&                       a_params,
		const configCustomHolder_t&          a_confData,
		ActorObjectHolder::customEntryMap_t& a_entryMap) noexcept
	{
		for (auto& f : a_confData.data)
		{
			auto it = a_entryMap.try_emplace(f.first).first;

			const auto result = ProcessCustomEntry(
				a_params,
				f.second(a_params.get_sex()),
				it->second);

			if (result == AttachObjectResult::kFailed)
			{
				if (RemoveObject(
						a_params.actor,
						a_params.handle,
						it->second,
						a_params.objects,
						a_params.flags,
						false))
				{
					a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
				}
			}
		}
	}

	void Controller::ProcessCustomMap(
		ProcessParams&                 a_params,
		const configCustomPluginMap_t& a_confPluginMap,
		ConfigClass                    a_class) noexcept
	{
		auto& pluginMap = a_params.objects.GetCustom(a_class);

		for (auto& e : a_confPluginMap)
		{
			if (e.second.empty())
			{
				continue;
			}

			auto it = pluginMap.try_emplace(e.first).first;

			ProcessCustomEntryMap(a_params, e.second, it->second);
		}
	}

	void Controller::ProcessCustom(ProcessParams& a_params) noexcept
	{
		const auto& cstore = GetActiveConfig().custom;

		auto& actorConfig = cstore.GetActorData();

		if (auto it = actorConfig.find(a_params.actor->formID);
		    it != actorConfig.end())
		{
			ProcessCustomMap(
				a_params,
				it->second,
				ConfigClass::Actor);
		}

		auto& npcConfig = cstore.GetNPCData();

		if (auto it = npcConfig.find(a_params.npcOrTemplate->formID);
		    it != npcConfig.end())
		{
			ProcessCustomMap(
				a_params,
				it->second,
				ConfigClass::NPC);
		}

		auto& raceConfig = cstore.GetRaceData();

		if (auto it = raceConfig.find(a_params.race->formID);
		    it != raceConfig.end())
		{
			ProcessCustomMap(
				a_params,
				it->second,
				ConfigClass::Race);
		}

		ProcessCustomMap(
			a_params,
			cstore.GetGlobalData()[0],
			ConfigClass::Global);
	}

	void Controller::EvaluateImpl(
		BSFadeNode*                      a_root,
		NiNode*                          a_npcroot,
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		if (!IsREFRValid(a_actor))
		{
			return;
		}

		if (m_npcProcessingDisabled &&
		    a_actor != *g_thePlayer)
		{
			return;
		}

		if (!a_actor->GetBiped1(false))
		{
			return;
		}

		auto nrp = GetNPCRacePair(a_actor);
		if (!nrp)
		{
			return;
		}

		IncrementCounter();

		auto& objects = GetOrCreateObjectHolder(
			a_actor,
			nrp->npc,
			nrp->race,
			a_root,
			a_npcroot,
			*this,
			a_handle,
			m_nodeOverrideEnabled,
			m_nodeOverridePlayerEnabled,
			GetSettings().data.syncTransformsToFirstPersonSkeleton,
			m_bipedCache.GetOrCreate(a_actor->formID, GetCounterValue()));

		if (a_handle != objects.GetHandle())
		{
			Warning(
				"%s [%u]: %.8X: handle mismatch (%.8X != %.8X)",
				__FUNCTION__,
				__LINE__,
				a_actor->formID.get(),
				a_handle.get(),
				objects.GetHandle().get());

			RemoveActorImpl(
				a_actor,
				a_handle,
				ControllerUpdateFlags::kNone);

			return;
		}

		if (objects.m_root != a_root)
		{
			Warning(
				"%s [%u]: %.8X: skeleton root mismatch",
				__FUNCTION__,
				__LINE__,
				a_actor->formID.get());

			if (RemoveActorImpl(
					a_actor,
					a_handle,
					ControllerUpdateFlags::kNone))
			{
				auto& objs = GetOrCreateObjectHolder(
					a_actor,
					nrp->npc,
					nrp->race,
					a_root,
					a_npcroot,
					*this,
					a_handle,
					m_nodeOverrideEnabled,
					m_nodeOverridePlayerEnabled,
					GetSettings().data.syncTransformsToFirstPersonSkeleton,
					m_bipedCache.GetOrCreate(a_actor->formID, GetCounterValue()));

				EvaluateImpl(
					a_root,
					a_npcroot,
					a_actor,
					nrp->npc,
					nrp->race,
					a_handle,
					objs,
					a_flags);
			}
		}
		else
		{
			EvaluateImpl(
				a_root,
				a_npcroot,
				a_actor,
				nrp->npc,
				nrp->race,
				a_handle,
				objects,
				a_flags);
		}
	}

	void Controller::EvaluateImpl(
		const CommonParams&              a_params,
		ActorObjectHolder&               a_holder,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		EvaluateImpl(
			a_params.root,
			a_params.npcRoot,
			a_params.actor,
			a_params.npc,
			a_params.race,
			a_holder.GetHandle(),
			a_holder,
			a_flags);
	}

	template <class... Args>
	constexpr ProcessParams& make_process_params(
		ActorObjectHolder&                     a_holder,
		std::optional<ProcessParams>&          a_paramsOut,
		const stl::flag<ControllerUpdateFlags> a_flags,
		Args&&... a_args) noexcept
	{
		if (a_flags.test(ControllerUpdateFlags::kUseCachedParams))
		{
			return a_holder.GetOrCreateProcessParams(
				a_flags,
				std::forward<Args>(a_args)...);
		}
		else
		{
			a_paramsOut.emplace(
				a_flags,
				std::forward<Args>(a_args)...);

			return *a_paramsOut;
		}
	}

	void Controller::EvaluateImpl(
		BSFadeNode*                      a_root,
		NiNode*                          a_npcroot,
		Actor*                           a_actor,
		TESNPC*                          a_npc,
		TESRace*                         a_race,
		Game::ObjectRefHandle            a_handle,
		ActorObjectHolder&               a_holder,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
#if defined(IED_ENABLE_STATS_G)
		PerfTimer pt;
		pt.Start();
#endif

		if (!a_flags.test(ControllerUpdateFlags::kFromProcessorTask))
		{
			a_holder.m_flags.set(ActorObjectHolderFlags::kWantVarUpdate);
		}

		std::optional<ProcessParams> ps;

		auto& params = make_process_params(
			a_holder,
			ps,
			a_flags,
			a_actor,
			a_handle,
			a_holder.GetTempData(),
			a_actor,
			a_npc,
			a_race,
			a_root,
			a_npcroot,
			a_holder,
			*this);

		params.flags = a_flags;

		RunUpdateBipedSlotCache(params);

		if (a_holder.m_flags.consume(ActorObjectHolderFlags::kWantVarUpdate))
		{
			RunVariableMapUpdate(params);
		}

		bool refreshVariableMap;

		if (!IsActorBlockedImpl(a_actor->formID))
		{
			DoObjectEvaluation(params);

			refreshVariableMap = params.slotPresenceChanges.test_any(ObjectSlotBits::kAll);
		}
		else
		{
			refreshVariableMap = RemoveActorGear(
				a_actor,
				a_handle,
				a_holder,
				a_flags);
		}

		if (refreshVariableMap)
		{
			RunVariableMapUpdate(params, true);
		}

		if (GetSettings().data.enableXP32AA)
		{
			UpdateAA(a_actor, a_holder.m_animState);
		}

#if defined(IED_ENABLE_OUTFIT)

		if (!params.is_player())
		{
			EvaluateOutfit(m_configData.active->outfit, params);
		}

#endif

		if (a_flags.test(ControllerUpdateFlags::kImmediateTransformUpdate))
		{
			a_holder.RequestTransformUpdate();
		}
		else
		{
			a_holder.RequestTransformUpdateDefer();
		}

		m_evalCounter++;

#if defined(IED_ENABLE_STATS_G)
		Debug("G: [%.8X]: %f", a_actor->formID.get(), pt.Stop());
#endif
	}

	void Controller::UpdateBipedSlotCache(
		ProcessParams&     a_params,
		ActorObjectHolder& a_holder) noexcept
	{
		/*PerfTimer pt;
		pt.Start();*/

		auto& data = a_holder.m_slotCache->biped;

		auto biped = a_params.get_biped();
		if (!biped)
		{
			std::for_each(
				data.begin(),
				data.end(),
				[](auto& a_v) noexcept [[msvc::forceinline]] {
					a_v.occupied = false;
				});

			return;
		}

		const auto skin = a_params.get_actor_skin();

		using enum_type = std::underlying_type_t<BIPED_OBJECT>;

		for (enum_type i = 0; i < stl::underlying(BIPED_OBJECT::kTotal); i++)
		{
			const auto& e = biped->objects[i];
			auto&       f = data[i];

			if (e.item &&
			    e.item != skin &&
			    e.item != e.addon)
			{
				const auto fid = e.item->formID;

				if (f.forms.empty() || f.forms.front() != fid)
				{
					auto it = std::find(f.forms.begin(), f.forms.end(), fid);
					if (it != f.forms.end())
					{
						f.forms.erase(it);
					}

					f.forms.emplace(f.forms.begin(), fid);

					if (f.forms.size() > m_bipedCache.max_forms())
					{
						f.forms.pop_back();
					}

					f.seen = GetCounterValue();
				}
				else if (!f.occupied)
				{
					f.seen = GetCounterValue();
				}

				f.occupied = true;
			}
			else
			{
				f.occupied = false;
			}
		}

		//_DMESSAGE("%f", pt.Stop());
	}

	void Controller::RunVariableMapUpdate(
		ProcessParams& a_params,
		bool           a_markAllForEval) noexcept
	{
		const auto& config = GetActiveConfig().condvars;

		if (config.empty())
		{
			return;
		}

		if (IConditionalVariableProcessor::UpdateVariableMap(
				a_params,
				config,
				a_params.objects.GetVariables()))
		{
			if (a_markAllForEval)
			{
				RequestHFEvaluateAll();
			}
			else
			{
				RequestHFEvaluateAll(a_params.objects.GetActorFormID());
			}
		}
	}

	void Controller::RunUpdateBipedSlotCache(
		ProcessParams& a_params) noexcept
	{
		if (!a_params.state.flags.test(ProcessStateUpdateFlags::kBipedDataUpdated))
		{
			IncrementCounter();

			UpdateBipedSlotCache(a_params, a_params.objects);

			a_params.state.flags.set(ProcessStateUpdateFlags::kBipedDataUpdated);
		}
	}

	void Controller::DoObjectEvaluation(
		ProcessParams& a_params) noexcept
	{
		if (!GetSettings().data.disableNPCSlots ||
		    a_params.objects.IsPlayer())
		{
			ProcessSlots(a_params);
		}

		ProcessCustom(a_params);

		if (a_params.state.flags.test_any(ProcessStateUpdateFlags::kUpdateMask))
		{
			if (a_params.state.flags.test(ProcessStateUpdateFlags::kForceUpdate))
			{
				INode::UpdateRoot(a_params.root);
			}
			else
			{
				INode::UpdateRootConditional(a_params.actor, a_params.root);
			}

			a_params.state.flags.clear(ProcessStateUpdateFlags::kUpdateMask);
		}
	}

	void Controller::EvaluateImpl(
		ActorObjectHolder&               a_holder,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		auto handle = a_holder.GetHandle();

		NiPointer<TESObjectREFR> refr;
		if (!handle.Lookup(refr))
		{
			Warning(
				"%s [%u]: %.8X: could not lookup by handle (%.8X)",
				__FUNCTION__,
				__LINE__,
				a_holder.GetActorFormID().get(),
				handle.get());

			return;
		}

		auto actor = refr->As<Actor>();
		if (!actor)
		{
			Warning(
				"%s [%u]: %.8X: not an actor (%.8X, %hhu)",
				__FUNCTION__,
				__LINE__,
				refr->formID.get(),
				handle.get(),
				refr->formType);

			return;
		}

		if (actor != a_holder.m_actor.get())
		{
			Warning(
				"%s [%u]: actor mismatch (%.8X != %.8X)",
				__FUNCTION__,
				__LINE__,
				refr->formID.get(),
				a_holder.m_actor->formID.get());

			return;
		}

		EvaluateImpl(actor, handle, a_holder, a_flags);
	}

	void Controller::EvaluateTransformsImpl(
		ActorObjectHolder&               a_holder,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		if (auto& params = a_holder.GetCurrentProcessParams())
		{
			if (ProcessTransformsImpl(
					params->root,
					params->npcRoot,
					params->actor,
					params->npc,
					params->race,
					a_holder,
					a_flags))
			{
				INode::UpdateRootConditional(params->actor, params->root);
			}
		}
		else if (const auto info = LookupCachedActorInfo(a_holder))
		{
			if (ProcessTransformsImpl(
					info->root,
					info->npcRoot,
					info->actor,
					info->npc,
					info->race,
					a_holder,
					a_flags))
			{
				INode::UpdateRootConditional(info->actor, info->root);
			}
		}
	}

	template <class... Args>
	constexpr nodeOverrideParams_t make_node_override_params(
		ActorObjectHolder& a_holder,
		Args&&... a_args) noexcept
	{
		if (auto& params = a_holder.GetCurrentProcessParams())
		{
			return nodeOverrideParams_t{
				a_holder.GetTempData().nc,
				static_cast<const CommonParams&>(*params)
			};
		}
		else
		{
			return nodeOverrideParams_t{
				a_holder.GetTempData().nc,
				std::forward<Args>(a_args)...
			};
		}
	}

	bool Controller::ProcessTransformsImpl(
		BSFadeNode*                      a_root,
		NiNode*                          a_npcRoot,
		Actor*                           a_actor,
		TESNPC*                          a_npc,
		TESRace*                         a_race,
		ActorObjectHolder&               a_holder,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
#if defined(IED_ENABLE_STATS_T)
		PerfTimer pt;
		pt.Start();
#endif

		if (a_holder.IsPlayer() &&
		    !m_nodeOverridePlayerEnabled)
		{
			return false;
		}

		if (a_holder.m_cmeNodes.empty() &&
		    a_holder.m_movNodes.empty() &&
		    a_holder.m_weapNodes.empty())
		{
			return false;
		}

		auto params = make_node_override_params(
			a_holder,
			a_actor,
			a_npc,
			a_race,
			a_root,
			a_npcRoot,
			a_holder,
			*this);

		const auto& activeConfig = GetActiveConfig();

		configStoreNodeOverride_t::holderCache_t hc;

		for (const auto& e : a_holder.m_weapNodes)
		{
			const auto r = activeConfig.transforms.GetActorPlacement(
				a_actor->formID,
				params.npcOrTemplate->formID,
				a_race->formID,
				e.nodeName,
				hc);

			if (r)
			{
				INodeOverride::ApplyNodePlacement(r->get(params.get_sex()), e, params);
			}
			else
			{
				INodeOverride::ResetNodePlacement(e, std::addressof(params), false);
			}
		}

		for (const auto e : a_holder.m_cmeNodes.getvec())
		{
			const auto r = activeConfig.transforms.GetActorTransform(
				a_actor->formID,
				params.npcOrTemplate->formID,
				a_race->formID,
				e->first,
				hc);

			e->second.cachedConfCME = r;

			if (r)
			{
				INodeOverride::ApplyNodeVisibility(
					e->second,
					r->get(params.get_sex()),
					params);
			}
		}

		for (const auto e : a_holder.m_cmeNodes.getvec())
		{
			if (const auto conf = e->second.cachedConfCME)
			{
				INodeOverride::ApplyNodeOverride(
					e->first,
					e->second,
					conf->get(params.get_sex()),
					params);
			}
			else
			{
				INodeOverride::ResetNodeOverrideImpl(e->second.thirdPerson);
				if (e->second.firstPerson)
				{
					INodeOverride::ResetNodeOverrideImpl(e->second.firstPerson);
				}
			}
		}

		if (PhysicsProcessingEnabled())
		{
			for (const auto e : a_holder.m_movNodes.getvec())
			{
				const auto r = activeConfig.transforms.GetActorPhysics(
					a_actor->formID,
					params.npcOrTemplate->formID,
					a_race->formID,
					e->first,
					hc);

				if (!r)
				{
					if (auto& node = e->second.thirdPerson.simComponent)
					{
						a_holder.RemoveAndDestroySimComponent(node);
					}

					if (auto& node = e->second.firstPerson.simComponent)
					{
						a_holder.RemoveAndDestroySimComponent(node);
					}
				}
				else
				{
					ProcessTransformsImplPhys(params, r, e->second);
				}
			}
		}

		if (GetSettings().data.enableXP32AA &&
		    a_holder.m_animState.flags.consume(ActorAnimationState::Flags::kNeedUpdate))
		{
			UpdateAA(a_actor, a_holder.m_animState);
		}

#if defined(IED_ENABLE_STATS_T)
		Debug("T: [%.8X]: %f", a_actor->formID.get(), pt.Stop());
#endif

		return true;
	}

	void Controller::ProcessTransformsImplPhys(
		nodeOverrideParams_t&                   a_params,
		const configNodeOverrideEntryPhysics_t* a_config,
		const MOVNodeEntry&                     a_entry) noexcept
	{
		auto& conf = INodeOverride::GetPhysicsConfig(a_config->get(a_params.get_sex()), a_params);

		ProcessTransformsImplPhysNode(a_params, conf, a_entry.thirdPerson);

		if (auto& node = a_entry.firstPerson)
		{
			ProcessTransformsImplPhysNode(a_params, conf, node);
		}
	}

	void Controller::ProcessTransformsImplPhysNode(
		nodeOverrideParams_t&                  a_params,
		const Data::configNodePhysicsValues_t& a_conf,
		const MOVNodeEntry::Node&              a_node) noexcept
	{
		if (auto& simComponent = a_node.simComponent)
		{
			if (a_conf.valueFlags.test(Data::ConfigNodePhysicsFlags::kDisabled) ||
			    (!a_params.has_pending_loads() && !a_node.parent_has_visible_geometry()))
			{
				a_params.objects.RemoveAndDestroySimComponent(simComponent);
			}
			else if (simComponent->GetConfig() != a_conf)
			{
				simComponent->UpdateConfig(a_conf);
			}
		}
		else if (
			!a_conf.valueFlags.test(Data::ConfigNodePhysicsFlags::kDisabled) &&
			a_node.parent_has_visible_geometry())
		{
			simComponent = a_params.objects.CreateAndAddSimComponent(
				a_node.node.get(),
				a_node.orig,
				a_conf);
		}
	}

	void Controller::ActorResetImpl(
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		RemoveActorImpl(
			a_actor,
			a_handle,
			a_flags);

		EvaluateImpl(
			a_actor,
			a_handle,
			a_flags | ControllerUpdateFlags::kImmediateTransformUpdate);
	}

	void Controller::ActorResetImpl(
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags,
		ObjectSlot                       a_slot)
	{
		if (a_slot >= ObjectSlot::kMax)
		{
			return;
		}

		auto it = m_actorMap.find(a_actor->formID);
		if (it == m_actorMap.end())
		{
			return;
		}

		RemoveObject(
			a_actor,
			a_handle,
			it->second.GetSlot(a_slot),
			it->second,
			a_flags,
			false);

		EvaluateImpl(
			a_actor,
			a_handle,
			it->second,
			a_flags | ControllerUpdateFlags::kImmediateTransformUpdate);
	}

	void Controller::ResetCustomImpl(
		Actor*                   a_actor,
		Game::ObjectRefHandle    a_handle,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		auto it = m_actorMap.find(a_actor->formID);
		if (it != m_actorMap.end())
		{
			auto& data = it->second.GetCustom(a_class);

			auto itp = data.find(a_pkey);
			if (itp != data.end())
			{
				auto ite = itp->second.find(a_vkey);
				if (ite != itp->second.end())
				{
					RemoveObject(
						a_actor,
						a_handle,
						ite->second,
						it->second,
						ControllerUpdateFlags::kNone,
						false);

					itp->second.erase(ite);

					if (itp->second.empty())
					{
						data.erase(itp);
					}
				}
			}
		}

		EvaluateImpl(
			a_actor,
			a_handle,
			ControllerUpdateFlags::kImmediateTransformUpdate);
	}

	void Controller::ResetCustomImpl(
		Actor*                   a_actor,
		Game::ObjectRefHandle    a_handle,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey)
	{
		auto it = m_actorMap.find(a_actor->formID);
		if (it != m_actorMap.end())
		{
			auto& data = it->second.GetCustom(a_class);

			auto itp = data.find(a_pkey);
			if (itp != data.end())
			{
				for (auto& e : itp->second)
				{
					RemoveObject(
						a_actor,
						a_handle,
						e.second,
						it->second,
						ControllerUpdateFlags::kNone,
						false);
				}

				data.erase(itp);
			}
		}

		EvaluateImpl(
			a_actor,
			a_handle,
			ControllerUpdateFlags::kImmediateTransformUpdate);
	}

	void Controller::ResetCustomImpl(
		Actor*                a_actor,
		Game::ObjectRefHandle a_handle,
		ConfigClass           a_class)
	{
		auto it = m_actorMap.find(a_actor->formID);
		if (it != m_actorMap.end())
		{
			auto& data = it->second.GetCustom(a_class);

			for (auto& e : data)
			{
				for (auto& f : e.second)
				{
					RemoveObject(
						a_actor,
						a_handle,
						f.second,
						it->second,
						ControllerUpdateFlags::kNone,
						false);
				}
			}

			data.clear();
		}

		EvaluateImpl(
			a_actor,
			a_handle,
			ControllerUpdateFlags::kImmediateTransformUpdate);
	}

	void Controller::ResetGearImpl(
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		ActorObjectHolder&               a_holder,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		RemoveActorGear(a_actor, a_handle, a_holder, a_flags);
		EvaluateImpl(a_actor, a_handle, a_holder, a_flags);
	}

	inline auto make_process_params(
		const Controller::cachedActorInfo_t& a_info,
		Controller&                          a_controller) noexcept
	{
		return ProcessParams{
			ControllerUpdateFlags::kNone,
			a_info.actor,
			a_info.handle,
			a_info.objects.GetTempData(),
			a_info.actor,
			a_info.npc,
			a_info.race,
			a_info.root,
			a_info.npcRoot,
			a_info.objects,
			a_controller
		};
	}

	const configBaseValues_t& Controller::GetConfigForActor(
		const cachedActorInfo_t& a_info,
		const configCustom_t&    a_config,
		const ObjectEntryCustom& a_entry)
	{
		assert(a_entry.data.state);

		auto params = make_process_params(
			a_info,
			*this);

		if (auto eo = a_config.get_equipment_override_sfp(
				{ a_entry.data.state->form },
				params))
		{
			return *eo;
		}

		return a_config;
	}

	const configBaseValues_t& Controller::GetConfigForActor(
		const cachedActorInfo_t& a_info,
		const configSlot_t&      a_config,
		const ObjectEntrySlot&   a_entry)
	{
		assert(a_entry.data.state);

		auto params = make_process_params(
			a_info,
			*this);

		if (auto eo = a_config.get_equipment_override_fp(
				{ a_entry.data.state->form,
		          a_entry.slotidex },
				params))
		{
			return *eo;
		}

		return a_config;
	}

	void Controller::UpdateCustomImpl(
		Game::FormID             a_actor,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey,
		UpdateActionfunc         a_func)
	{
		auto it = m_actorMap.find(a_actor);
		if (it != m_actorMap.end())
		{
			UpdateCustomImpl(it->second, a_class, a_pkey, a_vkey, a_func);
		}
	}

	void Controller::UpdateCustomNPCImpl(
		Game::FormID             a_npc,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey,
		UpdateActionfunc         a_func)
	{
		for (auto& e : m_actorMap.getvec())
		{
			if (e->second.IsActorNPCOrTemplate(a_npc))
			{
				UpdateCustomImpl(e->second, a_class, a_pkey, a_vkey, a_func);
			}
		}
	}

	void Controller::UpdateCustomRaceImpl(
		Game::FormID             a_race,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey,
		UpdateActionfunc         a_func)
	{
		for (auto& e : m_actorMap.getvec())
		{
			if (e->second.IsActorRace(a_race))
			{
				UpdateCustomImpl(e->second, a_class, a_pkey, a_vkey, a_func);
			}
		}
	}

	void Controller::UpdateCustomImpl(
		Game::FormID             a_actor,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		UpdateActionfunc         a_func)
	{
		auto it = m_actorMap.find(a_actor);
		if (it != m_actorMap.end())
		{
			UpdateCustomImpl(it->second, a_class, a_pkey, a_func);
		}
	}

	void Controller::UpdateCustomNPCImpl(
		Game::FormID             a_npc,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		UpdateActionfunc         a_func)
	{
		for (auto& e : m_actorMap.getvec())
		{
			if (e->second.IsActorNPCOrTemplate(a_npc))
			{
				UpdateCustomImpl(e->second, a_class, a_pkey, a_func);
			}
		}
	}

	void Controller::UpdateCustomRaceImpl(
		Game::FormID             a_race,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		UpdateActionfunc         a_func)
	{
		for (auto& e : m_actorMap.getvec())
		{
			if (e->second.IsActorRace(a_race))
			{
				UpdateCustomImpl(e->second, a_class, a_pkey, a_func);
			}
		}
	}

	void Controller::UpdateCustomImpl(
		Game::FormID     a_actor,
		ConfigClass      a_class,
		UpdateActionfunc a_func)
	{
		auto it = m_actorMap.find(a_actor);
		if (it != m_actorMap.end())
		{
			UpdateCustomImpl(it->second, a_class, a_func);
		}
	}

	void Controller::UpdateCustomNPCImpl(
		Game::FormID     a_npc,
		ConfigClass      a_class,
		UpdateActionfunc a_func)
	{
		for (auto& e : m_actorMap.getvec())
		{
			if (e->second.IsActorNPCOrTemplate(a_npc))
			{
				UpdateCustomImpl(e->second, a_class, a_func);
			}
		}
	}

	void Controller::UpdateCustomRaceImpl(
		Game::FormID     a_race,
		ConfigClass      a_class,
		UpdateActionfunc a_func)
	{
		for (auto& e : m_actorMap.getvec())
		{
			if (e->second.IsActorRace(a_race))
			{
				UpdateCustomImpl(e->second, a_class, a_func);
			}
		}
	}

	void Controller::UpdateCustomImpl(
		ActorObjectHolder&       a_record,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey,
		const UpdateActionfunc&  a_func)
	{
		auto info = LookupCachedActorInfo(a_record);
		if (!info)
		{
			return;
		}

		/*if (IsActorBlockedImpl(info.actor->formID))
		{
			return;
		}*/

		auto& data = a_record.GetCustom(a_class);
		auto& conf = GetActiveConfig().custom;

		switch (a_class)
		{
		case ConfigClass::Actor:
			{
				auto& cfgdata = conf.GetActorData();
				auto  it      = cfgdata.find(info->actor->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomImpl(
						*info,
						it->second,
						data,
						a_pkey,
						a_vkey,
						a_func);
				}
			}
			break;
		case ConfigClass::NPC:
			{
				auto& cfgdata = conf.GetNPCData();
				auto  it      = cfgdata.find(info->npcOrTemplate->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomImpl(
						*info,
						it->second,
						data,
						a_pkey,
						a_vkey,
						a_func);
				}
			}
			break;
		case ConfigClass::Race:
			{
				auto& cfgdata = conf.GetRaceData();
				auto  it      = cfgdata.find(info->race->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomImpl(
						*info,
						it->second,
						data,
						a_pkey,
						a_vkey,
						a_func);
				}
			}
			break;
		default:
			UpdateCustomImpl(
				*info,
				conf.GetGlobalData()[0],
				data,
				a_pkey,
				a_vkey,
				a_func);

			break;
		}

		if (a_func.evalDirty && !a_func.clean)
		{
			EvaluateImpl(
				info->root,
				info->npcRoot,
				info->actor,
				info->npc,
				info->race,
				info->handle,
				a_record,
				ControllerUpdateFlags::kNone);
		}
	}

	void Controller::UpdateCustomImpl(
		ActorObjectHolder&       a_record,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const UpdateActionfunc&  a_func)
	{
		auto info = LookupCachedActorInfo(a_record);
		if (!info)
		{
			return;
		}

		auto& data = a_record.GetCustom(a_class);
		auto& conf = GetActiveConfig().custom;

		switch (a_class)
		{
		case ConfigClass::Actor:
			{
				auto& cfgdata = conf.GetActorData();
				auto  it      = cfgdata.find(info->actor->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(*info, it->second, data, a_pkey, a_func);
				}
			}
			break;
		case ConfigClass::NPC:
			{
				auto& cfgdata = conf.GetNPCData();
				auto  it      = cfgdata.find(info->npcOrTemplate->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(*info, it->second, data, a_pkey, a_func);
				}
			}
			break;
		case ConfigClass::Race:
			{
				auto& cfgdata = conf.GetRaceData();
				auto  it      = cfgdata.find(info->race->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(*info, it->second, data, a_pkey, a_func);
				}
			}
			break;
		default:
			UpdateCustomAllImpl(
				*info,
				conf.GetGlobalData()[0],
				data,
				a_pkey,
				a_func);

			break;
		}

		if (!a_func.clean)
		{
			EvaluateImpl(
				info->root,
				info->npcRoot,
				info->actor,
				info->npc,
				info->race,
				info->handle,
				a_record,
				ControllerUpdateFlags::kNone);
		}
	}

	void Controller::UpdateCustomImpl(
		ActorObjectHolder&      a_record,
		ConfigClass             a_class,
		const UpdateActionfunc& a_func)
	{
		auto info = LookupCachedActorInfo(a_record);
		if (!info)
		{
			return;
		}

		auto& data = a_record.GetCustom(a_class);
		auto& conf = GetActiveConfig().custom;

		switch (a_class)
		{
		case ConfigClass::Actor:
			{
				auto& cfgdata = conf.GetActorData();
				auto  it      = cfgdata.find(info->actor->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(*info, it->second, data, a_func);
				}
			}
			break;
		case ConfigClass::NPC:
			{
				auto& cfgdata = conf.GetNPCData();
				auto  it      = cfgdata.find(info->npcOrTemplate->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(*info, it->second, data, a_func);
				}
			}
			break;
		case ConfigClass::Race:
			{
				auto& cfgdata = conf.GetRaceData();
				auto  it      = cfgdata.find(info->race->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(*info, it->second, data, a_func);
				}
			}
			break;
		default:
			{
				UpdateCustomAllImpl(
					*info,
					conf.GetGlobalData()[0],
					data,
					a_func);
			}
			break;
		}

		if (!a_func.clean)
		{
			EvaluateImpl(
				info->root,
				info->npcRoot,
				info->actor,
				info->npc,
				info->race,
				info->handle,
				a_record,
				ControllerUpdateFlags::kNone);
		}
	}

	void Controller::UpdateCustomImpl(
		cachedActorInfo_t&                    a_info,
		const configCustomPluginMap_t&        a_confPluginMap,
		ActorObjectHolder::customPluginMap_t& a_pluginMap,
		const stl::fixed_string&              a_pkey,
		const stl::fixed_string&              a_vkey,
		const UpdateActionfunc&               a_func)
	{
		auto itc = a_confPluginMap.find(a_pkey);
		if (itc == a_confPluginMap.end())
		{
			return;
		}

		auto itd = a_pluginMap.find(a_pkey);
		if (itd == a_pluginMap.end())
		{
			return;
		}

		UpdateCustomImpl(a_info, itc->second.data, itd->second, a_vkey, a_func);
	}

	void Controller::UpdateCustomAllImpl(
		cachedActorInfo_t&                    a_info,
		const configCustomPluginMap_t&        a_confPluginMap,
		ActorObjectHolder::customPluginMap_t& a_pluginMap,
		const stl::fixed_string&              a_pkey,
		const UpdateActionfunc&               a_func)
	{
		auto itc = a_confPluginMap.find(a_pkey);
		if (itc == a_confPluginMap.end())
		{
			return;
		}

		auto itd = a_pluginMap.find(a_pkey);
		if (itd == a_pluginMap.end())
		{
			return;
		}

		bool failed = false;
		bool ran    = false;

		for (auto& e : itd->second)
		{
			auto itce = itc->second.data.find(e.first);
			if (itce != itc->second.data.end())
			{
				ran = true;
				if (!a_func(a_info, itce->second, e.second))
				{
					failed = true;
				}
			}
		}

		a_func.clean = ran && !failed;
	}

	void Controller::UpdateCustomAllImpl(
		cachedActorInfo_t&                    a_info,
		const configCustomPluginMap_t&        a_confPluginMap,
		ActorObjectHolder::customPluginMap_t& a_pluginMap,
		const UpdateActionfunc&               a_func)
	{
		bool failed = false;
		bool ran    = false;

		for (auto& e : a_pluginMap)
		{
			auto itc = a_confPluginMap.find(e.first);
			if (itc != a_confPluginMap.end())
			{
				for (auto& f : e.second)
				{
					auto itc2 = itc->second.data.find(e.first);
					if (itc2 != itc->second.data.end())
					{
						ran = true;
						if (!a_func(a_info, itc2->second, f.second))
						{
							failed = true;
						}
					}
				}
			}
		}

		a_func.clean = ran && !failed;
	}

	void Controller::UpdateCustomImpl(
		cachedActorInfo_t&                   a_info,
		const configCustomEntryMap_t&        a_confEntryMap,
		ActorObjectHolder::customEntryMap_t& a_entryMap,
		const stl::fixed_string&             a_vkey,
		const UpdateActionfunc&              a_func)
	{
		auto itc = a_confEntryMap.find(a_vkey);
		if (itc == a_confEntryMap.end())
		{
			return;
		}

		auto itd = a_entryMap.find(a_vkey);
		if (itd == a_entryMap.end())
		{
			return;
		}

		a_func.clean = a_func(a_info, itc->second, itd->second);
	}

	auto Controller::LookupCachedActorInfo(
		Actor*             a_actor,
		ActorObjectHolder& a_holder) noexcept
		-> std::optional<cachedActorInfo_t>
	{
		auto npc = a_actor->GetActorBase();
		if (!npc)
		{
			return {};
		}

		auto race = a_actor->race;
		if (!race)
		{
			race = npc->race;

			if (!race)
			{
				return {};
			}
		}

		auto root = a_actor->GetNiRootFadeNode(false);
		if (!root)
		{
			Warning(
				__FUNCTION__ ": %.8X: actor has no 3D",
				a_actor->formID.get());

			return {};
		}

		if (root != a_holder.m_root)
		{
			Warning(
				__FUNCTION__ ": %.8X: skeleton root mismatch",
				a_actor->formID.get());

			QueueReset(a_actor, ControllerUpdateFlags::kNone);

			return {};
		}

		auto npcroot = GetNodeByName(root, BSStringHolder::GetSingleton()->m_npcroot);
		if (!npcroot)
		{
			return {};
		}

		return std::make_optional<cachedActorInfo_t>(
			a_actor,
			a_holder.GetHandle(),
			npc,
			npc->GetFirstNonTemporaryOrThis(),
			race,
			root,
			npcroot,
			a_holder.GetSex(),
			a_holder);
	}

	auto Controller::LookupCachedActorInfo2(
		Actor*             a_actor,
		ActorObjectHolder& a_holder) noexcept
		-> std::optional<cachedActorInfo2_t>
	{
		auto npc = a_actor->GetActorBase();
		if (!npc)
		{
			return {};
		}

		auto race = a_actor->race;
		if (!race)
		{
			race = npc->race;

			if (!race)
			{
				return {};
			}
		}

		auto root = a_actor->GetNiRootFadeNode(false);
		if (!root)
		{
			Warning(
				__FUNCTION__ ": %.8X: actor has no 3D",
				a_actor->formID.get());

			return {};
		}

		if (root != a_holder.m_root)
		{
			Warning(
				__FUNCTION__ ": %.8X: skeleton root mismatch",
				a_actor->formID.get());

			QueueReset(a_actor, ControllerUpdateFlags::kNone);

			return {};
		}

		auto npcroot = FindNodeTraverse(root, BSStringHolder::GetSingleton()->m_npcroot);
		if (!npcroot)
		{
			return {};
		}

		return std::make_optional<cachedActorInfo2_t>(
			npc,
			race,
			root,
			npcroot);
	}

	auto Controller::LookupCachedActorInfo(
		ActorObjectHolder& a_holder) noexcept
		-> std::optional<cachedActorInfo_t>
	{
		auto handle = a_holder.GetHandle();

		NiPointer<TESObjectREFR> refr;
		if (!handle.Lookup(refr))
		{
			Warning(
				__FUNCTION__ ": %.8X: could not lookup by handle (%.8X)",
				a_holder.m_actorid,
				handle.get());

			return {};
		}

		if (!IsREFRValid(refr))
		{
			return {};
		}

		auto actor = refr->As<Actor>();
		if (!actor)
		{
			Warning(
				__FUNCTION__ ": %.8X: not an actor (%u, %hhu)",
				refr->formID.get(),
				handle.get(),
				refr->formType);

			return {};
		}

		return LookupCachedActorInfo(actor, a_holder);
	}

	void Controller::SaveLastEquippedItems(
		ProcessParams&          a_params,
		const EquippedItemInfo& a_info,
		ActorObjectHolder&      a_objectHolder) noexcept
	{
		const auto c = IncrementCounter();

		if (a_info.rightSlot < ObjectSlot::kMax)
		{
			auto& slot = a_objectHolder.GetSlot(a_info.rightSlot);

			slot.slotState.lastEquipped     = a_info.right->formID;
			slot.slotState.lastSeenEquipped = c;
		}

		if (a_info.leftSlot < ObjectSlot::kMax)
		{
			auto& slot = a_objectHolder.GetSlot(a_info.leftSlot);

			slot.slotState.lastEquipped     = a_info.left->formID;
			slot.slotState.lastSeenEquipped = c;
		}

		if (const auto* const biped = a_params.get_biped())
		{
			auto& e = biped->get_object(BIPED_OBJECT::kQuiver);

			const auto* const item = e.item;

			if (item &&
			    item != e.addon &&
			    item->IsAmmo())
			{
				auto& slot = a_objectHolder.GetSlot(ObjectSlot::kAmmo);

				slot.slotState.lastEquipped     = item->formID;
				slot.slotState.lastSeenEquipped = c;
			}
		}
	}

	void Controller::Receive(const SKSESerializationEvent& a_evn)
	{
		switch (a_evn.type)
		{
		case SKSESerializationEventType::kSave:
			SaveGameHandler(a_evn.intfc);
			break;
		case SKSESerializationEventType::kRevert:
			RevertHandler(a_evn.intfc);
			break;
		}
	}

	void Controller::Receive(const SKSESerializationLoadEvent& a_evn)
	{
		ReadRecord(a_evn.intfc, a_evn.type, a_evn.version, a_evn.length);
	}

	void Controller::Receive(const SKSESerializationFormDeleteEvent& a_evn)
	{
		if (a_evn.handle.GetUpper() != 0xFFFF)
		{
			return;
		}

		const auto formid = a_evn.handle.GetFormID();

		if (!formid.IsTemporary())
		{
			return;
		}

		class DeletedFormHandlerTask :
			public TaskDelegate
		{
		public:
			constexpr explicit DeletedFormHandlerTask(
				Controller&  a_controller,
				Game::FormID a_form) noexcept :
				m_controller(a_controller),
				m_formid(a_form)
			{
			}

			virtual void Run() override
			{
				const stl::lock_guard lock(m_controller.m_lock);
				m_controller.m_bipedCache.data().erase(m_formid);
			}

			virtual void Dispose() override
			{
				delete this;
			}

		private:
			Controller&  m_controller;
			Game::FormID m_formid;
		};

		ITaskPool::AddPriorityTask<DeletedFormHandlerTask>(*this, formid);
	}

	void Controller::Receive(const SKSEMessagingEvent& a_evn)
	{
		switch (a_evn.message->type)
		{
		case SKSEMessagingInterface::kMessage_PostPostLoad:

			GetSDSInterface();

			break;
		case SKSEMessagingInterface::kMessage_InputLoaded:

			InitializeBSFixedStringTable();

			break;
		case SKSEMessagingInterface::kMessage_DataLoaded:

			OnDataLoaded();

			break;
		case SKSEMessagingInterface::kMessage_NewGame:

			Drivers::UI::QueueRemoveTask(INTRO_BANNER_TASK_ID);

			break;
		case SKSEMessagingInterface::kMessage_PreLoadGame:

			Drivers::UI::QueueRemoveTask(INTRO_BANNER_TASK_ID);

			StoreActiveHandles();

			break;
		case SKSEMessagingInterface::kMessage_PostLoadGame:

			if (static_cast<bool>(a_evn.message->data))
			{
				EvaluateStoredHandles(ControllerUpdateFlags::kNone);
			}
			else
			{
				ClearStoredHandles();
			}

			break;
		case SKSEMessagingInterface::kMessage_SaveGame:

			SaveSettings(false, true);

			break;
		}
	}

	void Controller::Receive(const SDSPlayerShieldOnBackSwitchEvent& a_evn)
	{
		QueueRequestEvaluate(*g_thePlayer, false, true);
	}

	auto Controller::ReceiveEvent(
		const TESObjectLoadedEvent* a_evn,
		BSTEventSource<TESObjectLoadedEvent>*)
		-> EventResult
	{
		if (a_evn && a_evn->loaded)
		{
			if (auto actor = a_evn->formId.As<Actor>())
			{
				QueueEvaluate(
					actor,
					ControllerUpdateFlags::kImmediateTransformUpdate);
			}
		}

		return EventResult::kContinue;
	}

	auto Controller::ReceiveEvent(
		const TESInitScriptEvent* a_evn,
		BSTEventSource<TESInitScriptEvent>*)
		-> EventResult
	{
		if (a_evn && a_evn->reference->IsActor())
		{
			QueueEvaluate(
				a_evn->reference,
				ControllerUpdateFlags::kImmediateTransformUpdate);
		}

		return EventResult::kContinue;
	}

	auto Controller::ReceiveEvent(
		const TESEquipEvent* a_evn,
		BSTEventSource<TESEquipEvent>*)
		-> EventResult
	{
		if (a_evn &&
		    a_evn->actor &&
		    a_evn->actor->IsActor() &&
		    a_evn->baseObject)
		{
			QueueRequestEvaluate(a_evn->actor->formID, false, true);
		}

		return EventResult::kContinue;
	}

	auto Controller::ReceiveEvent(
		const TESContainerChangedEvent* a_evn,
		BSTEventSource<TESContainerChangedEvent>*)
		-> EventResult
	{
		if (a_evn && a_evn->baseObj)
		{
			if (a_evn->oldContainer &&
			    a_evn->oldContainer.As<Actor>())
			{
				QueueRequestEvaluate(a_evn->oldContainer, true, false);
			}

			if (a_evn->newContainer &&
			    a_evn->oldContainer != a_evn->newContainer &&  // ?
			    a_evn->newContainer.As<Actor>())
			{
				QueueRequestEvaluate(a_evn->newContainer, true, false);
			}
		}

		return EventResult::kContinue;
	}

	auto Controller::ReceiveEvent(
		const TESFurnitureEvent* a_evn,
		BSTEventSource<TESFurnitureEvent>*)
		-> EventResult
	{
		if (a_evn)
		{
			QueueRequestEvaluate(a_evn->actor, true, false);
		}

		return EventResult::kContinue;
	}

	auto Controller::ReceiveEvent(
		const TESDeathEvent* a_evn,
		BSTEventSource<TESDeathEvent>*)
		-> EventResult
	{
		if (a_evn)
		{
			ITaskPool::QueueLoadedActorTask(
				a_evn->source,
				[this](Actor* a_actor, Game::ActorHandle a_handle) {
					const stl::lock_guard lock(m_lock);

					auto it = m_actorMap.find(a_actor->formID);
					if (it != m_actorMap.end())
					{
						RemoveInvisibleObjects(
							it->second,
							static_cast<Game::ObjectRefHandle>(a_handle));

						it->second.RequestEval();
						it->second.RequestTransformUpdate();
					}
				});
		}

		return EventResult::kContinue;
	}

	EventResult Controller::ReceiveEvent(
		const TESSwitchRaceCompleteEvent* a_evn,
		BSTEventSource<TESSwitchRaceCompleteEvent>*)
	{
		if (a_evn)
		{
			QueueReset(a_evn->refr, ControllerUpdateFlags::kPlayEquipSound);
		}

		return EventResult::kContinue;
	}

	auto Controller::ReceiveEvent(
		const MenuOpenCloseEvent* a_evn,
		BSTEventSource<MenuOpenCloseEvent>*)
		-> EventResult
	{
		if (a_evn && !a_evn->opening)
		{
			static constexpr UIStringHolder::STRING_INDICES menus[] = {
				UIStringHolder::STRING_INDICES::kinventoryMenu,
				UIStringHolder::STRING_INDICES::kcontainerMenu,
				UIStringHolder::STRING_INDICES::kgiftMenu,
				UIStringHolder::STRING_INDICES::kbarterMenu,
				UIStringHolder::STRING_INDICES::kmagicMenu,
				UIStringHolder::STRING_INDICES::kconsole,
				UIStringHolder::STRING_INDICES::kfavoritesMenu
			};

			auto uish = UIStringHolder::GetSingleton();

			for (auto& e : menus)
			{
				if (a_evn->menuName == uish->GetString(e))
				{
					if (auto player = *g_thePlayer)
					{
						QueueRequestEvaluate(player->formID, false, false, false);
					}

					break;
				}
			}
		}

		return EventResult::kContinue;
	}

	/*EventResult Controller::ReceiveEvent(
		const TESSceneEvent*           a_evn,
		BSTEventSource<TESSceneEvent>* a_dispatcher)
	{
		if (a_evn)
		{
			_DMESSAGE("%.8X: %u | %s", a_evn->scene, a_evn->state, a_evn->callback->GetCallbackName().c_str());
		}

		return EventResult::kContinue;
	}*/

	EventResult Controller::ReceiveEvent(
		const TESActorLocationChangeEvent* a_evn,
		BSTEventSource<TESActorLocationChangeEvent>*)
	{
		if (a_evn)
		{
			QueueRequestEvaluate(a_evn->actor, true, false);
		}

		return EventResult::kContinue;
	}

	auto Controller::GetNPCRacePair(
		Actor* a_actor) noexcept
		-> std::optional<NRP>
	{
		auto npc = a_actor->GetActorBase();
		if (!npc)
		{
			return {};
		}

		auto race = a_actor->race;
		if (!race)
		{
			race = npc->race;

			if (!race)
			{
				return {};
			}
		}

		return std::make_optional<NRP>(npc, race);
	}

	bool Controller::SaveCurrentConfigAsDefault(
		stl::flag<ExportFlags>                   a_exportFlags,
		stl::flag<ConfigStoreSerializationFlags> a_flags)
	{
		auto tmp = CreateFilteredConfigStore(
			GetActiveConfig(),
			a_exportFlags,
			a_flags);

		FillGlobalSlotConfig(tmp->slot);
		IMaintenance::CleanConfigStore(*tmp);

		if (!SaveConfigStore(PATHS::DEFAULT_CONFIG_USER, *tmp))
		{
			return false;
		}

		m_configData.initial = std::move(tmp);

		return true;
	}

	void Controller::SaveGameHandler(SKSESerializationInterface* a_intfc)
	{
		WriteRecord(
			a_intfc,
			SKSE_SERIALIZATION_TYPE_ID,
			stl::underlying(SerializationVersion::kCurrentVersion));
	}

	void Controller::RevertHandler(SKSESerializationInterface* a_intfc)
	{
		Debug("Reverting..");

		const stl::lock_guard lock(m_lock);

		if (IsDefaultConfigForced())
		{
			m_configData.stash.reset();

			GetKeyBindDataHolder()->ResetKeyToggleStates();
		}
		else
		{
			*m_configData.active = *m_configData.initial;

			GetKeyBindDataHolder()->Clear();
		}

		m_actorBlockList.clear();
		m_bipedCache.clear();

		ClearObjectsImpl();
		//ClearObjectDatabase();

		ResetCounter();
		m_evalCounter = 0;
	}

	std::size_t Controller::Store(
		boost::archive::binary_oarchive& a_out)
	{
		const stl::lock_guard lock(m_lock);

		a_out << m_actorBlockList;

		if (IsDefaultConfigForced() && m_configData.stash)
		{
			a_out << *m_configData.stash;
		}
		else
		{
			a_out << *m_configData.active;
		}

		a_out << *this;

		return 4;
	}

	std::size_t Controller::Load(
		SKSESerializationInterface*      a_intfc,
		std::uint32_t                    a_version,
		boost::archive::binary_iarchive& a_in)
	{
		if (m_forceFlushSaveData)
		{
			throw std::exception("flushing saved data");
		}

		if (a_version > stl::underlying(SerializationVersion::kCurrentVersion))
		{
			throw std::exception("unsupported version");
		}

		const stl::lock_guard lock(m_lock);

		actorBlockList_t blockList;
		auto             cfgStore = std::make_unique_for_overwrite<configStore_t>();

		a_in >> blockList;
		a_in >> *cfgStore;

		if (a_version < stl::underlying(SerializationVersion::kDataVersion9))
		{
			auto actorState = std::make_unique_for_overwrite<actorStateHolder_t>();
			a_in >> *actorState;
		}

		if (!IsDefaultConfigForced())
		{
			GetKeyBindDataHolder()->SetFromConfig(cfgStore->keybinds);
		}

		if (a_version >= stl::underlying(SerializationVersion::kDataVersion7))
		{
			a_in >> *this;
		}

		IMaintenance::CleanBlockList(blockList);

		FillGlobalSlotConfig(cfgStore->slot);
		IMaintenance::CleanConfigStore(*cfgStore);

		if (!GetSettings().data.placementRandomization)
		{
			IMaintenance::ClearConfigStoreRand(*cfgStore);
		}

		m_actorBlockList = std::move(blockList);

		if (IsDefaultConfigForced())
		{
			m_configData.stash = std::move(cfgStore);
		}
		else
		{
			m_configData.active = std::move(cfgStore);
		}

		return 4;
	}

	void Controller::FillGlobalSlotConfig(
		configStoreSlot_t& a_data)
	{
		auto& global = a_data.GetGlobalData();

		for (std::uint32_t i = 0; i < std::size(global); i++)
		{
			auto& g = global[i];

			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type j = 0; j < stl::underlying(ObjectSlot::kMax); j++)
			{
				auto slot = static_cast<ObjectSlot>(j);
				if (auto& e = g.get(slot); !e)
				{
					e = CreateDefaultSlotConfig(slot);

					if (slot == ObjectSlot::kAmmo)
					{
						e->visit([](auto& a_v) {
							a_v.flags.set(BaseFlags::kDisabled | BaseFlags::kDisableHavok);
						});
					}
				}
			}
		}
	}

	void Controller::SaveSettings(
		bool       a_defer,
		bool       a_dirtyOnly,
		const bool a_debug)
	{
		auto func = [this,
		             a_dirtyOnly,
		             a_debug,
		             sl = std::source_location::current()]() {
			const stl::lock_guard lock(m_lock);

			PerfTimer pt;

			if (a_debug)
			{
				pt.Start();
			}

			const bool result = a_dirtyOnly ?
			                        GetSettings().SaveIfDirty() :
			                        GetSettings().Save();

			if (a_debug && result)
			{
				Debug(
					"%s: %fs",
					sl.function_name(),
					pt.Stop());
			}
			else if (!result)
			{
				Error("Could not save settings: %s", GetSettings().GetLastException().what());
			}
		};

		if (a_defer)
		{
			ITaskPool::AddTask(std::move(func));
		}
		else
		{
			func();
		}
	}

	void Controller::OnUIOpen()
	{
		UpdateActorInfo(m_actorMap);
	}

	void Controller::OnUIClose()
	{
		ClearActorInfo();
	}

	void Controller::JSOnDataImport()
	{
		auto& activeConfig = GetActiveConfig();

		FillGlobalSlotConfig(activeConfig.slot);
		IMaintenance::CleanConfigStore(activeConfig);

		GetKeyBindDataHolder()->SetFromConfig(activeConfig.keybinds);

		QueueResetAll(ControllerUpdateFlags::kNone);

		if (auto& rt = UIGetRenderTask())
		{
			rt->QueueReset();
		}
	}

	void Controller::OnKBStateChanged()
	{
		ITaskPool::AddTask([this] {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_actorMap.getvec())
			{
				if (e->first == IData::GetPlayerRefID())
				{
					e->second.RequestEval();
				}
				else
				{
					e->second.m_wantHFUpdate = true;
				}
			}
		});
	}

	void Controller::OnAsyncModelClone(
		const NiPointer<ObjectCloningTask>& a_task)
	{
		const stl::lock_guard lock(m_lock);

		const auto& actorMap = GetActorMap();

		const auto it = actorMap.find(a_task->GetActor());
		if (it == actorMap.end())
		{
			return;
		}

		const auto handle = it->second.GetHandle();

		if (const auto refr = handle.get_ptr())
		{
			const auto actor = refr->As<Actor>();

			if (IsREFRValid(actor) &&
			    it->second.GetActor().get() == actor)
			{
				const auto cell = actor->GetParentCell();
				if (!cell || !cell->IsAttached())
				{
					EvaluateImpl(
						actor,
						handle,
						ControllerUpdateFlags::kPlayEquipSound |
							ControllerUpdateFlags::kImmediateTransformUpdate);

					return;
				}
			}
		}

		it->second.RequestEval();
	}

	bool Controller::IsWeaponNodeSharingDisabled() const
	{
		return m_weaponNodeSharingDisabled;
	}

	void Controller::QueueUpdateActorInfo(Game::FormID a_actor)
	{
		ITaskPool::AddTask([this, a_actor]() {
			const stl::lock_guard lock(m_lock);
			UpdateActorInfo(a_actor);
		});
	}

	void Controller::QueueUpdateActorInfo(
		Game::FormID              a_actor,
		std::function<void(bool)> a_callback)
	{
		ITaskPool::AddTask([this, a_actor, callback = std::move(a_callback)]() {
			const stl::lock_guard lock(m_lock);
			callback(UpdateActorInfo(a_actor));
		});
	}

	void Controller::QueueUpdateNPCInfo(Game::FormID a_npc)
	{
		ITaskPool::AddTask([this, a_npc]() {
			const stl::lock_guard lock(m_lock);
			UpdateNPCInfo(a_npc);
		});
	}

	void Controller::QueueUpdateNPCInfo(
		Game::FormID              a_npc,
		std::function<void(bool)> a_callback)
	{
		ITaskPool::AddTask([this, a_npc, callback = std::move(a_callback)]() {
			const stl::lock_guard lock(m_lock);
			callback(UpdateNPCInfo(a_npc));
		});
	}

	void Controller::StoreActiveHandles()
	{
		const stl::lock_guard lock(m_lock);

		m_activeHandles.clear();

		for (const auto& e : m_actorMap.getvec())
		{
			m_activeHandles.emplace_back(e->second.GetHandle());
		}
	}

	void Controller::EvaluateStoredHandles(ControllerUpdateFlags a_flags)
	{
		const stl::lock_guard lock(m_lock);

		for (const auto& e : m_activeHandles)
		{
			NiPointer<TESObjectREFR> ref;
			if (e.Lookup(ref))
			{
				QueueEvaluate(ref, a_flags);
			}
		}

		m_activeHandles.clear();
		m_activeHandles.shrink_to_fit();
	}

	void Controller::ClearStoredHandles()
	{
		const stl::lock_guard lock(m_lock);

		m_activeHandles.clear();
		m_activeHandles.shrink_to_fit();
	}

	void Controller::QueueObjectDatabaseClear()
	{
		ITaskPool::AddTask([this]() {
			const stl::lock_guard lock(m_lock);

			ClearObjectDatabase();
		});
	}

	void Controller::SetObjectDatabaseLevel(ObjectDatabaseLevel a_level)
	{
		const stl::lock_guard lock(m_lock);

		if (a_level != GetODBLevel())
		{
			SetODBLevel(a_level);
			QueueDatabaseCleanup();
		}
	}

	void Controller::QueueSetLanguage(const stl::fixed_string& a_lang)
	{
		ITaskPool::AddTask([this, a_lang] {
			SetLanguageImpl(a_lang);
			if (Drivers::UI::IsImInitialized())
			{
				Drivers::UI::QueueSetLanguageGlyphData(GetCurrentGlyphData());
			}
		});
	}

	void Controller::SetupUI()
	{
		const auto            uiLock = Drivers::UI::GetScopedLock();
		const stl::lock_guard lock(m_lock);

		const auto& config = GetSettings().data;

		if (config.ui.scale != 1.0f)
		{
			Drivers::UI::QueueSetScale(config.ui.scale);
		}

		if (!config.ui.font.empty())
		{
			Drivers::UI::QueueFontChange(config.ui.font);
		}

		if (config.ui.extraGlyphs.test_any(GlyphPresetFlags::kAll))
		{
			Drivers::UI::QueueSetExtraGlyphs(config.ui.extraGlyphs);
		}

		if (config.ui.fontSize)
		{
			Drivers::UI::QueueSetFontSize(*config.ui.fontSize);
		}

		Drivers::UI::SetStyle(config.ui.stylePreset);
		Drivers::UI::SetReleaseFontData(config.ui.releaseFontData);
		Drivers::UI::SetAlpha(config.ui.alpha);
		Drivers::UI::SetBGAlpha(config.ui.bgAlpha);
		Drivers::UI::QueueSetLanguageGlyphData(GetCurrentGlyphData());

		if (config.ui.showIntroBanner &&
		    !m_iniconf->m_disableIntroBanner)
		{
			DispatchIntroBanner();
		}
	}

	stl::smart_ptr<FontGlyphData> Controller::GetCurrentGlyphData()
	{
		const stl::lock_guard lock(m_lock);

		if (const auto table = GetCurrentLanguageTable())
		{
			return table->GetGlyphData();
		}
		else
		{
			return {};
		}
	}

}