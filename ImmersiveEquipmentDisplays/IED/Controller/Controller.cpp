#include "pch.h"

#include "Controller.h"

#include "IConditionalVariableProcessor.h"
#include "IMaintenance.h"
#include "INodeOverride.h"

#include "IED/EngineExtensions.h"
#include "IED/Inventory.h"
#include "IED/NodeMap.h"
#include "IED/TransformOverrides.h"
#include "IED/UI/UIMain.h"
#include "IED/Util/Common.h"

#include "IED/UI/UIIntroBanner.h"

#include "Drivers/Input.h"
#include "Drivers/UI.h"

#include "IED/Parsers/JSONConfigStoreParser.h"

#include <IED/ReferenceLightController.h>
#include <ext/SKSEMessagingHandler.h>
#include <ext/SKSESerializationEventHandler.h>

namespace IED
{
	using namespace Util::Common;
	using namespace ::Util::Node;
	using namespace Data;

	Controller::Controller(
		const std::shared_ptr<const ConfigINI>& a_config) :
		ActorProcessorTask(),
		IEquipment(m_rngBase),
		IAnimationManager(m_config.settings),
		m_iniconf(a_config),
		m_nodeOverrideEnabled(a_config->m_nodeOverrideEnabled),
		m_nodeOverridePlayerEnabled(a_config->m_nodeOverridePlayerEnabled),
		m_forceDefaultConfig(a_config->m_forceDefaultConfig),
		m_npcProcessingDisabled(a_config->m_disableNPCProcessing),
		m_enableCorpseScatter(a_config->m_enableCorpseScatter),
		//m_forceOrigWeapXFRM(a_config->m_forceOrigWeapXFRM),
		m_forceFlushSaveData(a_config->m_forceFlushSaveData),
		m_bipedCache(
			a_config->m_bipedSlotCacheMaxSize,
			a_config->m_bipedSlotCacheMaxForms)
	{
	}

	void Controller::SinkInputEvents()
	{
		assert(m_iniconf);

		Drivers::Input::RegisterForKeyEvents(m_inputHandlers.playerBlock);

		if (m_iniconf->m_enableUI)
		{
			Drivers::Input::RegisterForPriorityKeyEvents(m_inputHandlers.uiOpen);
		}
	}

	void Controller::SinkSerializationEvents()
	{
		auto& seh = SKSESerializationEventHandler::GetSingleton();

		seh.AddSink(this);
		ASSERT(seh.RegisterForLoadEvent(SKSE_SERIALIZATION_TYPE_ID, this));
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
		if (m_esif.test(EventSinkInstallationFlags::kT1))
		{
			return true;
		}

		if (auto mm = MenuManager::GetSingleton())
		{
			mm->GetMenuOpenCloseEventDispatcher().AddEventSink(this);

			m_esif.set(EventSinkInstallationFlags::kT1);

			return true;
		}
		else
		{
			return false;
		}
	}

	bool Controller::SinkEventsT2()
	{
		if (m_esif.test(EventSinkInstallationFlags::kT2))
		{
			return true;
		}

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

			m_esif.set(EventSinkInstallationFlags::kT2);

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

		m_config.settings.SetPath(PATHS::SETTINGS);

		if (!m_config.settings.Load())
		{
			if (Serialization::FileExists(PATHS::SETTINGS))
			{
				Error(
					"%s: failed loading settings: %s",
					PATHS::SETTINGS,
					m_config.settings.GetLastException().what());
			}
		}
		else
		{
			if (m_config.settings.HasErrors())
			{
				Warning("%s: settings loaded with errors", PATHS::SETTINGS);
			}
		}

		const auto& settings = m_config.settings.data;

		if (settings.logLevel)
		{
			gLog.SetLogLevel(*settings.logLevel);
		}

		ISKSE::GetBacklog().SetLimit(std::clamp<std::uint32_t>(settings.ui.logLimit, 1, 2000));

		SetODBLevel(settings.odbLevel);

		InitializeSound();

		if (settings.playerBlockKeys)
		{
			m_inputHandlers.playerBlock.SetKeys(
				settings.playerBlockKeys->comboKey,
				settings.playerBlockKeys->key);
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

		SetProcessorTaskRunAUState(settings.hkWeaponAnimations);
		SetShaderProcessingEnabled(settings.enableEffectShaders);
		SetProcessorTaskParallelUpdates(settings.apParallelUpdates);

		m_cpuHasSSE41 = ::IsProcessorFeaturePresent(PF_SSE4_1_INSTRUCTIONS_AVAILABLE);

		if (m_cpuHasSSE41)
		{
			SetPhysicsProcessingEnabled(settings.enableEquipmentPhysics);
			PHYSimComponent::SetMaxDiff(settings.physics.maxDiff);
		}
		else
		{
			SetPhysicsProcessingEnabled(false);
		}

		auto& rlc = ReferenceLightController::GetSingleton();

		rlc.SetNPCLightCellAttachFixEnabled(settings.lightNPCCellAttachFix);
		rlc.SetNPCLightUpdateFixEnabled(settings.lightNPCUpdateFix);
		rlc.SetNPCLightUpdatesEnabled(settings.lightEnableNPCUpdates);
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

		auto& settings = m_config.settings.data.sound;

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
		const auto& settings = m_config.settings.data.sound;

		ClearSounds();

		for (auto& e : settings.data)
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
					ITaskPool::AddTask([this] {
						UIOpen();
					});
				});

			m_inputHandlers.uiOpen.SetProcessPaused(
				m_iniconf->m_enableInMenus);
		}
	}

	void Controller::InitializeUI()
	{
		assert(m_iniconf);

		UIInitialize(*this);

		const auto& config = m_config.settings.data;

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

		if (!stl::is_equal(config.ui.scale, 1.0f))
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

		if (config.ui.showIntroBanner &&
		    !m_iniconf->m_disableIntroBanner)
		{
			DispatchIntroBanner();
		}

		UIEnableNotifications(config.ui.enableNotifications);
		UISetLogNotificationThreshold(config.ui.notificationThreshold);

		m_safeToOpenUI = true;
	}

	bool Controller::DispatchIntroBanner()
	{
		auto task = make_timed_ui_task<UI::UIIntroBanner>(7000000, m_iniconf->m_introBannerVOffset);

		task->SetControlLock(false);
		task->SetFreezeTime(false);
		task->SetWantCursor(false);
		task->SetEnabledInMenu(true);
		task->EnableRestrictions(false);

		return Drivers::UI::AddTask(-0xFFFF, task);
	}

	void Controller::InitializeConfig()
	{
		bool defaultConfLoaded = false;

		if (Serialization::FileExists(PATHS::DEFAULT_CONFIG_USER))
		{
			defaultConfLoaded = LoadConfigStore(
				PATHS::DEFAULT_CONFIG_USER,
				m_config.initial);
		}

		if (!defaultConfLoaded)
		{
			defaultConfLoaded = LoadConfigStore(
				PATHS::DEFAULT_CONFIG,
				m_config.initial);
		}

		if (!defaultConfLoaded)
		{
			Warning("No default configuration could be loaded");
		}

		FillGlobalSlotConfig(m_config.initial.slot);
		IMaintenance::CleanConfigStore(m_config.initial);

		m_config.active = m_config.initial;

		if (m_forceDefaultConfig)
		{
			Message("Note: ForceDefaultConfig is enabled");
		}
	}

	void Controller::InitializeLocalization()
	{
		auto& settings = m_config.settings;
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
				auto info = ExtractAnimationInfoFromPEX();
				SetAnimationInfo(info);
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
		ASSERT(StringCache::IsInitialized());

		const stl::lock_guard lock(m_lock);

		InitializeAnimationStrings();
	}

	void Controller::OnDataLoaded()
	{
		const stl::lock_guard lock(m_lock);

		InitializeFPStateData();
		InitializeData();

		if (Drivers::UI::IsImInitialized())
		{
			InitializeUI();
		}

		ASSERT(SinkEventsT1());
		ASSERT(SinkEventsT2());

		if (IFPV_Detected())
		{
			Debug("IFPV detector plugin found");
		}

		SetProcessorTaskRunState(true);

		m_iniconf.reset();

		Debug("Data loaded, entered running state");
	}

	void Controller::Evaluate(
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		const stl::lock_guard lock(m_lock);

		EvaluateImpl(a_actor, a_handle, a_flags);
	}

	void Controller::ClearActorPhysicsDataImpl()
	{
		for (auto& e : m_objects)
		{
			e.second.ClearAllPhysicsData();
		}
	}

	void Controller::QueueClearActorPhysicsData()
	{
		ITaskPool::AddPriorityTask([this]() {
			const stl::lock_guard lock(m_lock);

			ClearActorPhysicsDataImpl();
		});
	}

	std::size_t Controller::GetNumSimComponents() const noexcept
	{
		std::size_t result = 0;

		for (auto& e : m_objects)
		{
			result += e.second.GetSimComponentListSize();
		}

		return result;
	}

	std::size_t Controller::GetNumAnimObjects() const noexcept
	{
		std::size_t result = 0;

		for (auto& e : m_objects)
		{
			result += e.second.GetNumAnimObjects();
		}

		return result;
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

		auto npcroot = FindNode(root, BSStringHolder::GetSingleton()->m_npcroot);
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

		auto root = a_actor->GetNiRootNode(false);
		if (!root)
		{
			Warning(
				"%s [%u]: %.8X: actor has no 3d",
				__FUNCTION__,
				__LINE__,
				a_actor->formID.get());

			return;
		}

		auto npcroot = FindNode(root, BSStringHolder::GetSingleton()->m_npcroot);
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

				auto it = m_objects.find(a_actor);
				if (it == m_objects.end())
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
				if (IsActorValid(actor))
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

			actorLookupResult_t result;
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

			for (auto& e : m_objects)
			{
				if (e.second.IsActorNPCOrTemplate(a_npc))
				{
					EvaluateImpl(e.second, a_flags);
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

			for (auto& e : m_objects)
			{
				if (e.second.IsActorRace(a_race))
				{
					EvaluateImpl(e.second, a_flags);
				}
			}
		});
	}

	void Controller::QueueEvaluateAll(
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_flags]() {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_objects)
			{
				EvaluateImpl(e.second, a_flags);
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

		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
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

		for (auto& e : m_objects)
		{
			if (e.second.IsActorNPCOrTemplate(a_npc))
			{
				if (a_noDefer)
				{
					e.second.RequestTransformUpdate();
				}
				else
				{
					e.second.RequestTransformUpdateDefer();
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

		for (auto& e : m_objects)
		{
			if (e.second.IsActorRace(a_race))
			{
				if (a_noDefer)
				{
					e.second.RequestTransformUpdate();
				}
				else
				{
					e.second.RequestTransformUpdateDefer();
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

		for (auto& e : m_objects)
		{
			if (a_noDefer)
			{
				e.second.RequestTransformUpdate();
			}
			else
			{
				e.second.RequestTransformUpdateDefer();
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

			actorLookupResult_t result;
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

			actorLookupResult_t result;
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
		for (auto& e : m_objects)
		{
			NiPointer<TESObjectREFR> ref;
			auto                     handle = e.second.GetHandle();

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
		bool result = SetLanguage(a_lang);

		if (result)
		{
			if (Drivers::UI::IsImInitialized())
			{
				Drivers::UI::QueueSetLanguageGlyphData(
					GetCurrentLanguageTable()->GetGlyphData());
			}
		}
		else
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

		if (m_config.active
		        .transforms.GetActorData()
		        .contains(a_holder.m_actor->formID))
		{
			return;
		}

		if (m_config.active
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

		m_config.active
			.transforms.GetNPCData()
			.emplace(npc->formID, std::move(tmp));
	}

	void Controller::OnActorAcquire(ActorObjectHolder& a_holder) noexcept
	{
		if (EngineExtensions::GetTransformOverridesEnabled())
		{
			a_holder.ApplyXP32NodeTransformOverrides();
		}

		if (m_config.settings.data.placementRandomization &&
		    !a_holder.m_movNodes.empty())
		{
			GenerateRandomPlacementEntries(a_holder);
		}

		//Debug("%s: acquired: %X", __FUNCTION__, a_holder.GetActorFormID());
	}

	bool Controller::WantGlobalVariableUpdateOnAddRemove() const noexcept
	{
		return !m_config.active.condvars.empty();
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

			auto& objects = GetObjects();
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

			for (auto& e : m_objects)
			{
				NiPointer<TESObjectREFR> ref;
				auto                     handle = e.second.GetHandle();

				if (!handle.Lookup(ref))
				{
					continue;
				}

				if (auto actor = ref->As<Actor>())
				{
					ResetGearImpl(actor, handle, e.second, a_flags);
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

	void Controller::QueueUpdateTransformSlot(
		Game::FormID a_actor,
		ObjectSlot   a_slot)
	{
		ITaskPool::AddTask([this, a_actor, a_slot] {
			const stl::lock_guard lock(m_lock);

			UpdateTransformSlotImpl(a_actor, a_slot);
		});
	}

	void Controller::QueueUpdateTransformSlotNPC(
		Game::FormID a_npc,
		ObjectSlot   a_slot)
	{
		ITaskPool::AddTask([this, a_npc, a_slot]() {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_objects)
			{
				if (e.second.IsActorNPCOrTemplate(a_npc))
				{
					UpdateTransformSlotImpl(e.second, a_slot);
				}
			}
		});
	}

	void Controller::QueueUpdateTransformSlotRace(
		Game::FormID a_race,
		ObjectSlot   a_slot)
	{
		ITaskPool::AddTask([this, a_race, a_slot]() {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_objects)
			{
				if (e.second.IsActorRace(a_race))
				{
					UpdateTransformSlotImpl(e.second, a_slot);
				}
			}
		});
	}

	void Controller::QueueUpdateTransformSlotAll(ObjectSlot a_slot)
	{
		ITaskPool::AddTask([this, a_slot] {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_objects)
			{
				UpdateTransformSlotImpl(e.second, a_slot);
			}
		});
	}

	void Controller::QueueResetAAAll()
	{
		ITaskPool::AddTask([this] {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_objects)
			{
				actorLookupResult_t result;
				if (LookupTrackedActor(e.first, result))
				{
					ResetAA(result.actor, e.second.GetAnimState());
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

			actorLookupResult_t result;
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

			actorLookupResult_t result;
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

			actorLookupResult_t result;
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

	void Controller::QueueUpdateTransformCustom(
		Game::FormID             a_actor,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_actor, a_class, a_pkey, a_vkey] {
			const stl::lock_guard lock(m_lock);

			UpdateCustomImpl(
				a_actor,
				a_class,
				a_pkey,
				a_vkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomNPC(
		Game::FormID             a_npc,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_npc, a_class, a_pkey, a_vkey] {
			const stl::lock_guard lock(m_lock);

			UpdateCustomNPCImpl(
				a_npc,
				a_class,
				a_pkey,
				a_vkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomRace(
		Game::FormID             a_race,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_race, a_class, a_pkey, a_vkey] {
			const stl::lock_guard lock(m_lock);

			UpdateCustomRaceImpl(
				a_race,
				a_class,
				a_pkey,
				a_vkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustom(
		Game::FormID             a_actor,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_actor, a_class, a_pkey] {
			const stl::lock_guard lock(m_lock);

			UpdateCustomImpl(
				a_actor,
				a_class,
				a_pkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomNPC(
		Game::FormID             a_npc,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_npc, a_class, a_pkey] {
			const stl::lock_guard lock(m_lock);

			UpdateCustomNPCImpl(
				a_npc,
				a_class,
				a_pkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomRace(
		Game::FormID             a_race,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_race, a_class, a_pkey] {
			const stl::lock_guard lock(m_lock);

			UpdateCustomRaceImpl(
				a_race,
				a_class,
				a_pkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustom(
		Game::FormID a_actor,
		ConfigClass  a_class)
	{
		ITaskPool::AddTask([this, a_actor, a_class] {
			const stl::lock_guard lock(m_lock);

			UpdateCustomImpl(
				a_actor,
				a_class,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomNPC(
		Game::FormID a_npc,
		ConfigClass  a_class)
	{
		ITaskPool::AddTask([this, a_npc, a_class] {
			const stl::lock_guard lock(m_lock);

			UpdateCustomNPCImpl(
				a_npc,
				a_class,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomRace(
		Game::FormID a_race,
		ConfigClass  a_class)
	{
		ITaskPool::AddTask([this, a_race, a_class] {
			const stl::lock_guard lock(m_lock);

			UpdateCustomRaceImpl(
				a_race,
				a_class,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomAll(
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_class, a_pkey, a_vkey] {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_objects)
			{
				UpdateCustomImpl(
					e.second,
					a_class,
					a_pkey,
					a_vkey,
					MakeTransformUpdateFunc());
			}
		});
	}

	void Controller::QueueUpdateTransformCustomAll(
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_class, a_pkey] {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_objects)
			{
				UpdateCustomImpl(
					e.second,
					a_class,
					a_pkey,
					MakeTransformUpdateFunc());
			}
		});
	}

	void Controller::QueueUpdateTransformCustomAll(
		ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_class] {
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_objects)
			{
				UpdateCustomImpl(
					e.second,
					a_class,
					MakeTransformUpdateFunc());
			}
		});
	}

	void Controller::QueueLookupFormInfo(
		Game::FormID              a_formId,
		form_lookup_result_func_t a_func)
	{
		ITaskPool::AddTask([this, a_formId, func = std::move(a_func)] {
			auto result = LookupFormInfo(a_formId);

			const stl::lock_guard lock(m_lock);

			func(std::move(result));
		});
	}

	void Controller::QueueLookupFormInfoCrosshairRef(
		form_lookup_result_func_t a_func)
	{
		ITaskPool::AddTask([this, func = std::move(a_func)] {
			std::unique_ptr<formInfoResult_t> result;

			{
				NiPointer<TESObjectREFR> ref;
				if (LookupCrosshairRef(ref))
				{
					result = LookupFormInfo(ref->formID);
				}
			}

			const stl::lock_guard lock(m_lock);

			func(std::move(result));
		});
	}

	void Controller::QueueGetFormDatabase(form_db_get_func_t a_func)
	{
		ITaskPool::AddTask([this, func = std::move(a_func)] {
			const stl::lock_guard lock(m_lock);

			func(GetFormDatabase());
		});
	}

	void Controller::QueueGetCrosshairRef(std::function<void(Game::FormID)> a_func)
	{
		ITaskPool::AddTask([this, func = std::move(a_func)] {
			Game::FormID result;

			{
				NiPointer<TESObjectREFR> ref;
				if (LookupCrosshairRef(ref))
				{
					result = ref->formID;
				}
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
			IMaintenance::ClearConfigStoreRand(m_config.active);
		});
	}

	void Controller::QueueSendAnimationEventToActor(
		Game::FormID a_actor,
		std::string  a_event)
	{
		ITaskPool::AddTask([this, a_actor, ev = std::move(a_event)] {
			const stl::lock_guard lock(m_lock);

			auto& data = GetObjects();

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
			auto it = m_objects.find(a_actor);
			if (it != m_objects.end())
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

				auto ita = m_objects.find(a_actor);
				if (ita != m_objects.end())
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

		if (!m_config.settings.data.toggleKeepLoaded)
		{
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
		else
		{
			if (!m_actorBlockList.playerToggle)
			{
				RemoveActorBlock(player->formID, StringHolder::GetSingleton().IED);
			}

			auto it = m_objects.find(player->formID);
			if (it != m_objects.end())
			{
				EvaluateImpl(it->second, ControllerUpdateFlags::kSoundAll);
			}
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
		auto it = m_objects.find(a_actor);
		if (it == m_objects.end())
		{
			return true;
		}

		return !it->second.HasHumanoidSkeleton() ||
		       it->second.IsXP32Skeleton();
	}

	bool Controller::DoItemUpdate(
		processParams_t&          a_params,
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
				if (state->weapAnimGraphManagerHolder)
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

			const auto mfid = a_currentModelForm ?
			                      a_currentModelForm->formID :
			                      Game::FormID{};

			if (mfid != state->modelForm)
			{
				return false;
			}
		}

		const bool isVisible = !state->flags.test(ObjectEntryFlags::kInvisible);
		//const bool isLightHidden = state->flags.test(ObjectEntryFlags::kHideLight);

		if (a_visible)
		{
			if (!isVisible)
			{
				if (a_params.flags.test(ControllerUpdateFlags::kVisibilitySounds))
				{
					PlayObjectSound(a_params, a_config, a_entry, true);
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
					PlayObjectSound(a_params, a_config, a_entry, false);
				}
			}
		}

		state->UpdateFlags(a_config);

		if (isVisible != a_visible)
		//isLightHidden != a_config.flags.test(BaseFlags::kHideLight))
		{
			a_entry.SetObjectVisible(a_visible);

			/*const bool lightVisible = a_visible &&
			                          !state->flags.test(ObjectEntryFlags::kHideLight);

			for (auto& e : state->groupObjects)
			{
				if (e.second.light)
				{
					e.second.light->SetVisible(lightVisible);
				}
			}

			if (state->light)
			{
				state->light->SetVisible(lightVisible);
			}

			a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);*/
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
				state->nodes.rootNode,
				state->nodes.ref);

			a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
		}

		if (!state->flags.test(ObjectEntryFlags::kIsGroup))
		{
			if (state->currentGeomTransformTag != a_config.geometryTransform)
			{
				INode::UpdateObjectTransform(
					a_config.geometryTransform,
					state->nodes.object);

				state->currentGeomTransformTag = a_config.geometryTransform;

				a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
			}

			if (a_config.flags.test(BaseFlags::kPlaySequence))
			{
				if (a_config.niControllerSequence != state->currentSequence)
				{
					state->UpdateAndPlayAnimation(
						a_params.actor,
						a_config.niControllerSequence);
				}
			}
			else if (state->weapAnimGraphManagerHolder)
			{
				if (a_config.flags.test(Data::BaseFlags::kAnimationEvent))
				{
					state->UpdateAndSendAnimationEvent(
						a_config.animationEvent);
				}
				else
				{
					state->UpdateAndSendAnimationEvent(
						StringHolder::GetSingleton().weaponSheathe);
				}
			}
		}

		if (state->nodes.HasPhysicsNode())
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
						state->nodes.physics.get(),
						state->nodes.physics->m_localTransform,
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
		processParams_t& a_params,
		ObjectEntryBase& a_entry) noexcept
	{
		if (auto& data = a_entry.data.effectShaderData)
		{
			data->ClearEffectShaderDataFromTree(a_params.objects.m_root);
			data->ClearEffectShaderDataFromTree(a_params.objects.m_root1p);

			data.reset();

			a_params.ResetEffectShaders();
		}
	}

	void Controller::ResetEffectShaderData(
		processParams_t& a_params,
		ObjectEntryBase& a_entry,
		NiAVObject*      a_object) noexcept
	{
		if (auto& data = a_entry.data.effectShaderData)
		{
			data->ClearEffectShaderDataFromTree(a_object);

			data.reset();

			a_params.ResetEffectShaders();
		}
	}

	void Controller::UpdateObjectEffectShaders(
		processParams_t&            a_params,
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
				a_objectEntry.data.effectShaderData = std::make_unique<EffectShaderData>(*es);

				a_params.ResetEffectShaders();
			}
			else
			{
				if (a_objectEntry.data.effectShaderData->UpdateIfChanged(
						a_objectEntry.data.state->nodes.rootNode,
						*es))
				{
					a_params.ResetEffectShaders();
				}
				else if (a_params.flags.test(ControllerUpdateFlags::kWantEffectShaderConfigUpdate))
				{
					if (!a_objectEntry.data.effectShaderData->UpdateConfigValues(*es))
					{
						a_objectEntry.data.effectShaderData->Update(
							a_objectEntry.data.state->nodes.rootNode,
							*es);

						a_params.ResetEffectShaders();
					}
				}
			}
		}
		else
		{
			ResetEffectShaderData(
				a_params,
				a_objectEntry,
				a_objectEntry.data.state->nodes.rootNode);
		}
	}

	void Controller::RemoveSlotObjectEntry(
		processParams_t& a_params,
		ObjectEntrySlot& a_entry) noexcept
	{
		if (RemoveObject(
				a_params.actor,
				a_params.handle,
				a_entry,
				a_params.objects,
				a_params.flags,
				false))
		{
			a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);

			a_params.mark_slot_presence_change(a_entry.slotid);
		}
	}

	void Controller::ProcessSlots(processParams_t& a_params) noexcept
	{
		auto pm = a_params.actor->processManager;
		if (!pm)
		{
			Warning(
				"%s: [%.8X] actor has no process manager",
				__FUNCTION__,
				a_params.actor->formID.get());

			return;
		}

		const auto& settings = m_config.settings.data;

		a_params.collector.GenerateSlotCandidates(
			a_params.objects.IsPlayer(),
			a_params.objects.IsPlayer() &&
				!settings.removeFavRestriction);

		const auto equippedInfo = CreateEquippedItemInfo(pm);

		SaveLastEquippedItems(
			a_params,
			equippedInfo,
			a_params.objects);

		configStoreSlot_t::holderCache_t hc;

		const configSlotPriority_t* prio;

		if (auto d = m_config.active.slot.GetActorPriority(
				a_params.actor->formID,
				a_params.npcOrTemplate->formID,
				a_params.race->formID,
				hc))
		{
			prio = std::addressof(d->get(a_params.configSex));
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
			    leftSlot != ObjectSlot::kMax)
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
				if (slot.slot == ObjectSlot::kMax)
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
				if (f.slot == ObjectSlot::kMax)
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

				auto entry = m_config.active.slot.GetActor(
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

				auto& configEntry = entry->get(a_params.configSex);

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

				auto configOverride =
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
					/*if (objectEntry.DeferredHideObject(
							2,
							!settings.hideEquipped ||
								configEntry.slotFlags.test(SlotFlags::kAlwaysUnload)))
					{
						a_params.mark_slot_presence_change(objectEntry.slotid);
					}*/

					if (settings.hideEquipped &&
					    !configEntry.slotFlags.test(SlotFlags::kAlwaysUnload))
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
								6);

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
					objectEntry);

				if (LoadAndAttach(
						a_params,
						usedBaseConf,
						configEntry,
						objectEntry,
						itemData->form,
						modelForm,
						ItemData::IsLeftWeaponSlot(f.slot),
						visible,
						false,
						settings.hkWeaponAnimations,
						PhysicsProcessingEnabled()))
				{
					objectEntry.SetObjectVisible(visible);

					objectEntry.data.state->UpdateArrows(
						usedBaseConf.flags.test(BaseFlags::kDynamicArrows) ?
							itemData->itemCount :
							6);

					objectEntry.slotState.lastSlotted = objectEntry.data.state->formid;

					if (visible)
					{
						itemData->consume_one();
					}

					item.consume(candidates);

					a_params.state.flags.set(
						ProcessStateUpdateFlags::kMenuUpdate |
						ProcessStateUpdateFlags::kObjectAttached);

					a_params.mark_slot_presence_change(objectEntry.slotid);

					typeActive |= visible;
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
					if (f.slot == ObjectSlot::kMax)
					{
						continue;
					}

					auto& objectEntry = a_params.objects.GetSlot(f.slot);

					if (!f.slotConfig)
					{
						f.slotConfig = m_config.active.slot.GetActor(
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

					auto& configEntry = f.slotConfig->get(a_params.configSex);

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
							{ objectEntry.data.state->form, objectEntry.slotidex },
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
								bipedObject,
								nodes.first,
								nodes.second,
								*es);

						a_params.ResetEffectShaders();
					}
					else
					{
						if (objectEntry.data.effectShaderData->UpdateIfChanged(a_params.objects, *es))
						{
							a_params.ResetEffectShaders();
						}
						else if (a_params.flags.test(ControllerUpdateFlags::kWantEffectShaderConfigUpdate))
						{
							if (!objectEntry.data.effectShaderData->UpdateConfigValues(*es))
							{
								objectEntry.data.effectShaderData->Update(
									a_params.objects,
									*es);

								a_params.ResetEffectShaders();
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
		processParams_t&     a_params) noexcept
	{
		if (a_flags.test(BaseFlags::kInvisible))
		{
			return false;
		}

		if (a_actor == *g_thePlayer &&
		    m_config.settings.data.toggleKeepLoaded &&
		    m_actorBlockList.playerToggle)
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

		/*if (a_flags.test(BaseFlags::kHideOnMount))
		{
			if (!a_params.state.mounted)
			{
				a_params.state.mounted =
					!a_actor->IsMount() &&
					((a_actor->flags2 & Actor::kFlags_kGettingOnOffMount) ==
				         Actor::kFlags_kGettingOnOffMount ||
				     a_actor->HasInteractionExtraData());
			}

			return !*a_params.state.mounted;
		}*/

		return true;
	}

	bool Controller::LookupTrackedActor(
		Game::FormID         a_actor,
		actorLookupResult_t& a_out) noexcept
	{
		auto it = m_objects.find(a_actor);
		if (it == m_objects.end())
		{
			return false;
		}

		return LookupTrackedActor(it->second, a_out);
	}

	bool Controller::LookupTrackedActor(
		const ActorObjectHolder& a_record,
		actorLookupResult_t&     a_out) noexcept
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
		processParams_t&      a_params,
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
		Game::FormID     a_id,
		processParams_t& a_params) noexcept
	{
		if (a_id == a_params.objects.GetActorFormID())
		{
			return std::addressof(a_params.objects);
		}
		else
		{
			auto& data = GetObjects();

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
		processParams_t&            a_params,
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
		processParams_t&      a_params,
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

	bool Controller::ProcessCustomEntry(
		processParams_t&      a_params,
		const configCustom_t& a_config,
		ObjectEntryCustom&    a_objectEntry) noexcept
	{
		if (a_config.equipmentOverrides.empty() &&
		    a_config.flags.test(BaseFlags::kDisabled))
		{
			a_objectEntry.clear_chance_flags();
			return false;
		}

		if (a_config.customFlags.test(CustomFlags::kIgnorePlayer) &&
		    a_params.actor == *g_thePlayer)
		{
			a_objectEntry.clear_chance_flags();
			return false;
		}

		if (!a_config.run_filters(a_params))
		{
			a_objectEntry.clear_chance_flags();
			return false;
		}

		const auto& settings = m_config.settings.data;

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
				return false;
			}

			if (a_config.customFlags.test_any(CustomFlags::kEquipmentModeMask) &&
			    !hasMinCount &&
			    (!m_config.settings.data.hideEquipped ||
			     a_config.customFlags.test(CustomFlags::kAlwaysUnload)))
			{
				a_objectEntry.clear_chance_flags();
				return false;
			}

			const auto& itemData = it->second;

			assert(itemData.form);

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
				return false;
			}

			if (IsBlockedByChance(
					a_params,
					a_config,
					a_objectEntry))
			{
				return false;
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
								6);

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

						return true;
					}
				}
			}

			if (!hasMinCount)
			{
				return false;
			}

			if (RemoveObject(
					a_params.actor,
					a_params.handle,
					a_objectEntry,
					a_params.objects,
					a_params.flags,
					false))
			{
				a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
			}

			bool result;

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
					settings.hkWeaponAnimations,
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
					settings.hkWeaponAnimations,
					PhysicsProcessingEnabled());

				a_objectEntry.cflags.clear(CustomObjectEntryFlags::kGroupMode);
			}

			if (result)
			{
				a_objectEntry.data.state->UpdateArrows(
					usedBaseConf.flags.test(BaseFlags::kDynamicArrows) ?
						itemData.itemCount :
						6);

				a_objectEntry.SetObjectVisible(visible);

				if (a_config.customFlags.test_any(CustomFlags::kEquipmentModeMask) && visible)
				{
					itemData.consume_one();
				}

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
		else
		{
			const auto cform = SelectCustomForm(a_params, a_config);

			if (!cform)
			{
				a_objectEntry.clear_chance_flags();

				return false;
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

				return false;
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
				return false;
			}

			if (IsBlockedByChance(
					a_params,
					a_config,
					a_objectEntry))
			{
				return false;
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

						return true;
					}
				}
			}

			if (RemoveObject(
					a_params.actor,
					a_params.handle,
					a_objectEntry,
					a_params.objects,
					a_params.flags,
					false))
			{
				a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
			}

			bool result;

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
					settings.hkWeaponAnimations,
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
					settings.hkWeaponAnimations,
					PhysicsProcessingEnabled());

				a_objectEntry.cflags.clear(CustomObjectEntryFlags::kGroupMode);
			}

			if (result)
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
		processParams_t&                     a_params,
		const configCustomHolder_t&          a_confData,
		ActorObjectHolder::customEntryMap_t& a_entryMap) noexcept
	{
		for (auto& f : a_confData.data)
		{
			auto it = a_entryMap.try_emplace(f.first).first;

			if (!ProcessCustomEntry(
					a_params,
					f.second(a_params.configSex),
					it->second))
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
		processParams_t&               a_params,
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

	void Controller::ProcessCustom(processParams_t& a_params) noexcept
	{
		const auto& cstore = m_config.active.custom;

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
		NiNode*                          a_root,
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
			/*m_config.settings.data.hkWeaponAnimations &&
				m_config.settings.data.animEventForwarding,*/
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
					/*m_config.settings.data.hkWeaponAnimations &&
						m_config.settings.data.animEventForwarding,*/
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
	inline constexpr processParams_t& make_process_params(
		ActorObjectHolder&                     a_holder,
		std::optional<processParams_t>&        a_paramsOut,
		const stl::flag<ControllerUpdateFlags> a_flags,
		Args&&... a_args) noexcept
	{
		if (a_flags.test(ControllerUpdateFlags::kUseCachedParams))
		{
			return a_holder.GetOrCreateProcessParams(
				a_holder.IsFemale() ?
					ConfigSex::Female :
					ConfigSex::Male,
				a_flags,
				std::forward<Args>(a_args)...);
		}
		else
		{
			a_paramsOut.emplace(
				a_holder.IsFemale() ?
					ConfigSex::Female :
					ConfigSex::Male,
				a_flags,
				std::forward<Args>(a_args)...);

			return *a_paramsOut;
		}
	}

	void Controller::EvaluateImpl(
		NiNode*                          a_root,
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

		std::optional<processParams_t> ps;

		auto& params = make_process_params(
			a_holder,
			ps,
			a_flags,
			a_actor,
			a_handle,
			m_temp.sr,
			a_holder.GetTempData().idt,
			a_holder.GetTempData().eqt,
			m_temp.uc,
			a_actor,
			a_npc,
			a_npc->GetFirstNonTemporaryOrThis(),
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
			/*auto bs = std::bitset<stl::underlying(Data::ObjectSlot::kMax)>(stl::underlying(params.slotPresenceChanges.value));

				Debug(">> %.8X", a_actor->formID);

				for (std::uint32_t i = 0; i < bs.size(); i++)
				{
					if (bs.test(i))
					{
						Debug("%u: %s", i, Data::GetSlotName(static_cast<Data::ObjectSlot>(i)));
					}
				}

				Debug("<< %.8X", a_actor->formID);*/

			RunVariableMapUpdate(params, true);
		}

		if (GetSettings().data.enableXP32AA)
		{
			UpdateAA(a_actor, a_holder.m_animState);
		}

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
		processParams_t&   a_params,
		ActorObjectHolder& a_holder) noexcept
	{
		/*PerfTimer pt;
		pt.Start();*/

		auto& data = a_holder.m_lastEquipped->biped;

		auto& biped = a_params.actor->GetBiped1(false);
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

		auto skin = a_params.get_actor_skin();

		using enum_type = std::underlying_type_t<BIPED_OBJECT>;

		for (enum_type i = 0; i < stl::underlying(BIPED_OBJECT::kTotal); i++)
		{
			auto& e = biped->objects[i];
			auto& f = data[i];

			if (e.item &&
			    e.item != skin &&
			    e.item != e.addon)
			{
				auto fid = e.item->formID;

				if (f.forms.empty() || f.forms.front() != fid)
				{
					auto itex = std::find(f.forms.begin(), f.forms.end(), fid);
					if (itex != f.forms.end())
					{
						f.forms.erase(itex);
					}

					f.forms.emplace(f.forms.begin(), fid);
					if (f.forms.size() > m_bipedCache.max_forms())
					{
						f.forms.pop_back();
					}
				}

				f.seen     = GetCounterValue();
				f.occupied = true;
			}
			else
			{
				f.occupied = false;
			}

			/*if (a_params.actor == *g_thePlayer)
			{
				if (!f.forms.empty())
				{
					_DMESSAGE("%u: %x", i, f.forms.front());
				}
			}*/
		}

		//_DMESSAGE("%f", pt.Stop());
	}

	void Controller::RunVariableMapUpdate(
		processParams_t& a_params,
		bool             a_markAllForEval) noexcept
	{
		const auto& config = m_config.active.condvars;

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
		processParams_t& a_params) noexcept
	{
		if (!a_params.state.flags.test(ProcessStateUpdateFlags::kBipedDataUpdated))
		{
			IncrementCounter();

			UpdateBipedSlotCache(a_params, a_params.objects);

			a_params.state.flags.set(ProcessStateUpdateFlags::kBipedDataUpdated);
		}
	}

	void Controller::DoObjectEvaluation(
		processParams_t& a_params) noexcept
	{
		if (!m_config.settings.data.disableNPCSlots ||
		    a_params.objects.IsPlayer())
		{
			ProcessSlots(a_params);
		}

		ProcessCustom(a_params);

		if (a_params.state.flags.test_any(ProcessStateUpdateFlags::kUpdateMask))
		{
			if (a_params.state.flags.test(ProcessStateUpdateFlags::kForceUpdate))
			{
				EngineExtensions::UpdateRoot(a_params.root);
			}
			else
			{
				INode::UpdateRootIfGamePaused(a_params.root);
			}

			a_params.state.flags.clear(ProcessStateUpdateFlags::kUpdateMask);

			/*if (a_params.state.flags.test_any(ProcessStateUpdateFlags::kWantDeferredTransformUpdate) && 
				!a_params.flags.test(ControllerUpdateFlags::kImmediateTransformUpdate))
			{
				a_params.objects.RequestTransformUpdateDefer();
			}
			else
			{
				a_params.objects.RequestTransformUpdate();
			}*/
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

	/*void Controller::EvaluateTransformsImpl(Game::FormID a_actor)
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			EvaluateTransformsImpl(it->second);
		}
	}*/

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
					params->npcOrTemplate,
					params->race,
					params->configSex,
					a_holder,
					a_flags))
			{
				INode::UpdateRootIfGamePaused(params->root);
			}
		}
		else if (auto info = LookupCachedActorInfo(a_holder))
		{
			if (ProcessTransformsImpl(
					info->root,
					info->npcRoot,
					info->actor,
					info->npc,
					info->npcOrTemplate,
					info->race,
					info->sex,
					a_holder,
					a_flags))
			{
				INode::UpdateRootIfGamePaused(info->root);
			}
		}
	}

	template <class... Args>
	inline constexpr nodeOverrideParams_t make_node_override_params(
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
		NiNode*                          a_root,
		NiNode*                          a_npcRoot,
		Actor*                           a_actor,
		TESNPC*                          a_npc,
		TESNPC*                          a_npcOrTemplate,
		TESRace*                         a_race,
		ConfigSex                        a_sex,
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
			a_npcOrTemplate,
			a_race,
			a_root,
			a_npcRoot,
			a_holder,
			*this);

		configStoreNodeOverride_t::holderCache_t hc;

		for (auto& e : a_holder.m_weapNodes)
		{
			const auto r = m_config.active.transforms.GetActorPlacement(
				a_actor->formID,
				params.npcOrTemplate->formID,
				a_race->formID,
				e.nodeName,
				hc);

			if (r)
			{
				INodeOverride::ApplyNodePlacement(r->get(a_sex), e, params);
			}
			else
			{
				INodeOverride::ResetNodePlacement(e, std::addressof(params), false);
			}
		}

		for (auto& [i, e] : a_holder.m_cmeNodes)
		{
			const auto r = m_config.active.transforms.GetActorTransform(
				a_actor->formID,
				params.npcOrTemplate->formID,
				a_race->formID,
				i,
				hc);

			e.cachedConfCME = r;

			if (r)
			{
				INodeOverride::ApplyNodeVisibility(
					e,
					r->get(a_sex),
					params);
			}
		}

		for (const auto& [i, e] : a_holder.m_cmeNodes)
		{
			if (const auto conf = e.cachedConfCME)
			{
				INodeOverride::ApplyNodeOverride(
					i,
					e,
					conf->get(a_sex),
					params);
			}
			else
			{
				INodeOverride::ResetNodeOverrideImpl(e.thirdPerson);
				if (e.firstPerson)
				{
					INodeOverride::ResetNodeOverrideImpl(e.firstPerson);
				}
			}
		}

		if (PhysicsProcessingEnabled())
		{
			for (auto& [i, e] : a_holder.m_movNodes)
			{
				const auto r = m_config.active.transforms.GetActorPhysics(
					a_actor->formID,
					params.npcOrTemplate->formID,
					a_race->formID,
					i,
					hc);

				auto& simComponent = e.simComponent;

				if (r)
				{
					auto& conf = INodeOverride::GetPhysicsConfig(r->get(a_sex), params);

					if (!conf.valueFlags.test(Data::ConfigNodePhysicsFlags::kDisabled) &&
					    e.parent_has_visible_geometry())
					{
						if (!simComponent)
						{
							simComponent = a_holder.CreateAndAddSimComponent(
								e.node.get(),
								e.origTransform,
								conf);

							//Debug("%.8X: adding %s", a_actor->formID, e.first.c_str());
						}
						else if (simComponent->GetConfig() != conf)
						{
							simComponent->UpdateConfig(conf);
						}
					}
					else
					{
						if (simComponent)
						{
							a_holder.RemoveAndDestroySimComponent(simComponent);
						}
					}
				}
				else if (simComponent)
				{
					a_holder.RemoveAndDestroySimComponent(simComponent);
				}
			}
		}

		/*if (m_forceOrigWeapXFRM &&
		    EngineExtensions::IsWeaponAdjustDisabled())
		{
			for (auto& e : a_holder.m_weapNodes)
			{
				if (e.originalTransform)
				{
					e.node->m_localTransform = *e.originalTransform;
				}
			}
		}*/

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

	void Controller::ActorResetImpl(
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		/*bool eraseState = false;

		if (a_actor != *g_thePlayer)
		{
			if (auto it = m_objects.find(a_actor->formID); it != m_objects.end())
			{
				m_storedActorStates.data.insert_or_assign(a_actor->formID, it->second);
				eraseState = true;
			}
		}*/

		RemoveActorImpl(
			a_actor,
			a_handle,
			a_flags);

		EvaluateImpl(
			a_actor,
			a_handle,
			a_flags | ControllerUpdateFlags::kImmediateTransformUpdate);

		/*if (eraseState)
		{
			m_storedActorStates.data.erase(a_actor->formID);
		}*/
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

		auto it = m_objects.find(a_actor->formID);
		if (it == m_objects.end())
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
		auto it = m_objects.find(a_actor->formID);
		if (it != m_objects.end())
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
		auto it = m_objects.find(a_actor->formID);
		if (it != m_objects.end())
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
		auto it = m_objects.find(a_actor->formID);
		if (it != m_objects.end())
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

	void Controller::UpdateTransformSlotImpl(
		Game::FormID a_actor,
		ObjectSlot   a_slot)
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			UpdateTransformSlotImpl(it->second, a_slot);
		}
	}

	void Controller::UpdateTransformSlotImpl(
		ActorObjectHolder& a_record,
		ObjectSlot         a_slot)
	{
		auto info = LookupCachedActorInfo(a_record);
		if (!info)
		{
			return;
		}

		if (a_slot < ObjectSlot::kMax)
		{
			auto& objectEntry = a_record.GetSlot(a_slot);
			if (!objectEntry.data.state)
			{
				return;
			}

			configStoreSlot_t::holderCache_t hc;

			auto config = m_config.active.slot.GetActor(
				info->actor->formID,
				info->npcOrTemplate->formID,
				info->race->formID,
				a_slot,
				hc);

			if (config)
			{
				auto& conf = GetConfigForActor(
					*info,
					config->get(info->sex),
					objectEntry);

				objectEntry.data.state->transform.Update(conf);

				INode::UpdateObjectTransform(
					objectEntry.data.state->transform,
					objectEntry.data.state->nodes.rootNode,
					objectEntry.data.state->nodes.ref);

				INode::UpdateRootIfGamePaused(info->root);
			}
		}
	}

	auto Controller::MakeTransformUpdateFunc()
		-> updateActionFunc_t
	{
		return {
			[this](
				cachedActorInfo_t&         a_info,
				const configCustomEntry_t& a_confEntry,
				ObjectEntryCustom&         a_entry) {
				if (!a_entry.data.state)
				{
					return false;
				}
				else
				{
					auto& conf = GetConfigForActor(
						a_info,
						a_confEntry(a_info.sex),
						a_entry);

					UpdateTransformCustomImpl(
						a_info,
						a_confEntry(a_info.sex),
						conf,
						a_entry);

					return true;
				}
			}
		};
	}

	inline auto make_process_params(
		const Controller::cachedActorInfo_t& a_info,
		Controller&                          a_controller) noexcept
	{
		return processParams_t{
			a_info.sex,
			ControllerUpdateFlags::kNone,
			a_info.actor,
			a_info.handle,
			a_controller.GetTempData().sr,
			a_info.objects.GetTempData().idt,
			a_info.objects.GetTempData().eqt,
			a_controller.GetTempData().uc,
			a_info.actor,
			a_info.npc,
			a_info.npcOrTemplate,
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
		updateActionFunc_t       a_func)
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			UpdateCustomImpl(it->second, a_class, a_pkey, a_vkey, a_func);
		}
	}

	void IED::Controller::UpdateCustomNPCImpl(
		Game::FormID             a_npc,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey,
		updateActionFunc_t       a_func)
	{
		for (auto& e : m_objects)
		{
			if (e.second.IsActorNPCOrTemplate(a_npc))
			{
				UpdateCustomImpl(e.second, a_class, a_pkey, a_vkey, a_func);
			}
		}
	}

	void IED::Controller::UpdateCustomRaceImpl(
		Game::FormID             a_race,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey,
		updateActionFunc_t       a_func)
	{
		for (auto& e : m_objects)
		{
			if (e.second.IsActorRace(a_race))
			{
				UpdateCustomImpl(e.second, a_class, a_pkey, a_vkey, a_func);
			}
		}
	}

	void Controller::UpdateCustomImpl(
		Game::FormID             a_actor,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		updateActionFunc_t       a_func)
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			UpdateCustomImpl(it->second, a_class, a_pkey, a_func);
		}
	}

	void IED::Controller::UpdateCustomNPCImpl(
		Game::FormID             a_npc,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		updateActionFunc_t       a_func)
	{
		for (auto& e : m_objects)
		{
			if (e.second.IsActorNPCOrTemplate(a_npc))
			{
				UpdateCustomImpl(e.second, a_class, a_pkey, a_func);
			}
		}
	}

	void IED::Controller::UpdateCustomRaceImpl(
		Game::FormID             a_race,
		ConfigClass              a_class,
		const stl::fixed_string& a_pkey,
		updateActionFunc_t       a_func)
	{
		for (auto& e : m_objects)
		{
			if (e.second.IsActorRace(a_race))
			{
				UpdateCustomImpl(e.second, a_class, a_pkey, a_func);
			}
		}
	}

	void Controller::UpdateCustomImpl(
		Game::FormID       a_actor,
		ConfigClass        a_class,
		updateActionFunc_t a_func)
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			UpdateCustomImpl(it->second, a_class, a_func);
		}
	}

	void IED::Controller::UpdateCustomNPCImpl(
		Game::FormID       a_npc,
		ConfigClass        a_class,
		updateActionFunc_t a_func)
	{
		for (auto& e : m_objects)
		{
			if (e.second.IsActorNPCOrTemplate(a_npc))
			{
				UpdateCustomImpl(e.second, a_class, a_func);
			}
		}
	}

	void IED::Controller::UpdateCustomRaceImpl(
		Game::FormID       a_race,
		ConfigClass        a_class,
		updateActionFunc_t a_func)
	{
		for (auto& e : m_objects)
		{
			if (e.second.IsActorRace(a_race))
			{
				UpdateCustomImpl(e.second, a_class, a_func);
			}
		}
	}

	void Controller::UpdateCustomImpl(
		ActorObjectHolder&        a_record,
		ConfigClass               a_class,
		const stl::fixed_string&  a_pkey,
		const stl::fixed_string&  a_vkey,
		const updateActionFunc_t& a_func)
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
		auto& conf = m_config.active.custom;

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

		//auto it = m_configStore.cfg.custom.GetActorData()
	}

	void Controller::UpdateCustomImpl(
		ActorObjectHolder&        a_record,
		ConfigClass               a_class,
		const stl::fixed_string&  a_pkey,
		const updateActionFunc_t& a_func)
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
		auto& conf = m_config.active.custom;

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
		ActorObjectHolder&        a_record,
		ConfigClass               a_class,
		const updateActionFunc_t& a_func)
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
		auto& conf = m_config.active.custom;

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
		const updateActionFunc_t&             a_func)
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
		const updateActionFunc_t&             a_func)
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
		const updateActionFunc_t&             a_func)
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
		const updateActionFunc_t&            a_func)
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

	void Controller::UpdateTransformCustomImpl(
		cachedActorInfo_t&       a_info,
		const configCustom_t&    a_configEntry,
		const configTransform_t& a_xfrmConfigEntry,
		ObjectEntryCustom&       a_entry)
	{
		if (!a_entry.data.state)
		{
			return;
		}

		a_entry.data.state->transform.Update(a_xfrmConfigEntry);

		INode::UpdateObjectTransform(
			a_entry.data.state->transform,
			a_entry.data.state->nodes.rootNode,
			a_entry.data.state->nodes.ref);

		//a_entry.state->UpdateGroupTransforms(a_configEntry.group);

		for (auto& e : a_configEntry.group.entries)
		{
			if (auto it = a_entry.data.state->groupObjects.find(e.first);
			    it != a_entry.data.state->groupObjects.end())
			{
				it->second.transform.Update(e.second.transform);

				if (it->second.weapAnimGraphManagerHolder)
				{
					if (e.second.flags.test(
							Data::ConfigModelGroupEntryFlags::kAnimationEvent))
					{
						it->second.UpdateAndSendAnimationEvent(
							e.second.animationEvent);
					}
					else
					{
						it->second.UpdateAndSendAnimationEvent(
							StringHolder::GetSingleton().weaponSheathe);
					}
				}
			}
		}

		for (auto& e : a_entry.data.state->groupObjects)
		{
			INode::UpdateObjectTransform(
				e.second.transform,
				e.second.rootNode,
				nullptr);
		}

		INode::UpdateRootIfGamePaused(a_info.root);
	}

	auto Controller::LookupCachedActorInfo(
		Actor*             a_actor,
		ActorObjectHolder& a_holder) noexcept
		-> std::optional<cachedActorInfo_t>
	{
		if (a_actor != a_holder.m_actor)
		{
			Warning(
				"%s [%u]: actor mismatch (%.8X != %.8X)",
				__FUNCTION__,
				__LINE__,
				a_actor->formID.get(),
				a_holder.m_actor->formID.get());
		}

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
				"%s: %.8X: actor has no 3D",
				__FUNCTION__,
				a_actor->formID.get());

			return {};
		}

		if (root != a_holder.m_root)
		{
			Warning(
				"%s: %.8X: skeleton root mismatch",
				__FUNCTION__,
				a_actor->formID.get());

			QueueReset(a_actor, ControllerUpdateFlags::kNone);

			return {};
		}

		auto npcroot = FindNode(root, BSStringHolder::GetSingleton()->m_npcroot);
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
			npc->GetSex() == 1 ?
				ConfigSex::Female :
				ConfigSex::Male,
			a_holder);
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
				"%s [%u]: %.8X: could not lookup by handle (%.8X)",
				__FUNCTION__,
				__LINE__,
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
				"%s: %.8X: not an actor (%u, %hhu)",
				__FUNCTION__,
				refr->formID.get(),
				handle.get(),
				refr->formType);

			return {};
		}

		return LookupCachedActorInfo(actor, a_holder);
	}

	void Controller::SaveLastEquippedItems(
		processParams_t&          a_params,
		const equippedItemInfo_t& a_info,
		ActorObjectHolder&        a_objectHolder) noexcept
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

		if (auto biped = a_params.get_biped())
		{
			auto& e = biped->get_object(BIPED_OBJECT::kQuiver);

			if (e.item &&
			    e.item != e.addon &&
			    e.item->IsAmmo())
			{
				auto& slot = a_objectHolder.GetSlot(ObjectSlot::kAmmo);

				slot.slotState.lastEquipped     = e.item->formID;
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

			Drivers::UI::QueueRemoveTask(-0xFFFF);

			break;
		case SKSEMessagingInterface::kMessage_PreLoadGame:

			Drivers::UI::QueueRemoveTask(-0xFFFF);

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
		if (a_evn && a_evn->actor)
		{
			if (a_evn->actor->IsActor() && a_evn->baseObject)
			{
				/*if (auto form = a_evn->baseObject.Lookup())
				{
					if (IFormCommon::IsEquippableForm(form))
					{
						QueueRequestEvaluate(a_evn->actor->formID, false, true);
					}
				}*/

				/*if (auto form = a_evn->baseObject.Lookup())
				{
					_DMESSAGE("%.8X | %hhu | %s", form->formID, form->formType, IFormCommon::GetFormName(form).c_str());
				}*/

				QueueRequestEvaluate(a_evn->actor->formID, false, true);
			}
		}

		//m_invChangeConsumerFlags.set(InventoryChangeConsumerFlags::kAll);

		return EventResult::kContinue;
	}

	auto Controller::ReceiveEvent(
		const TESContainerChangedEvent* a_evn,
		BSTEventSource<TESContainerChangedEvent>*)
		-> EventResult
	{
		if (a_evn && a_evn->baseObj)
		{
			if (a_evn->oldContainer)
			{
				if (a_evn->oldContainer.As<Actor>())
				{
					QueueRequestEvaluate(a_evn->oldContainer, true, false);
				}
			}

			if (a_evn->newContainer &&
			    a_evn->oldContainer != a_evn->newContainer)  // ?
			{
				if (a_evn->newContainer.As<Actor>())
				{
					QueueRequestEvaluate(a_evn->newContainer, true, false);
				}
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

					auto it = m_objects.find(a_actor->formID);
					if (it != m_objects.end())
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
			QueueReset(a_evn->refr, ControllerUpdateFlags::kPlaySound);
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
			auto uish = UIStringHolder::GetSingleton();

			if (a_evn->menuName ==
			        uish->GetString(UIStringHolder::STRING_INDICES::kinventoryMenu) ||
			    a_evn->menuName ==
			        uish->GetString(UIStringHolder::STRING_INDICES::kcontainerMenu) ||
			    a_evn->menuName ==
			        uish->GetString(UIStringHolder::STRING_INDICES::kgiftMenu) ||
			    a_evn->menuName ==
			        uish->GetString(UIStringHolder::STRING_INDICES::kbarterMenu) ||
			    a_evn->menuName ==
			        uish->GetString(UIStringHolder::STRING_INDICES::kmagicMenu) ||
			    a_evn->menuName ==
			        uish->GetString(UIStringHolder::STRING_INDICES::kfavoritesMenu))
			{
				if (auto player = *g_thePlayer)
				{
					QueueRequestEvaluate(player->formID, false, false, false);
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
		-> std::optional<npcRacePair_t>
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

		return npcRacePair_t{ npc, race };
	}

	bool Controller::SaveCurrentConfigAsDefault(
		stl::flag<ExportFlags>                   a_exportFlags,
		stl::flag<ConfigStoreSerializationFlags> a_flags)
	{
		auto tmp = CreateFilteredConfigStore(
			m_config.active,
			a_exportFlags,
			a_flags);

		FillGlobalSlotConfig(tmp->slot);
		IMaintenance::CleanConfigStore(*tmp);

		if (!SaveConfigStore(PATHS::DEFAULT_CONFIG_USER, *tmp))
		{
			return false;
		}

		m_config.initial = std::move(*tmp);

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

		//m_storedActorStates.clear();

		if (m_forceDefaultConfig)
		{
			m_config.stash = m_config.initial;
		}
		else
		{
			m_config.active = m_config.initial;
		}

		m_actorBlockList.clear();
		m_bipedCache.clear();

		ClearObjectsImpl();
		ClearObjectDatabase();

		ResetCounter();
	}

	std::size_t Controller::Store(
		boost::archive::binary_oarchive& a_out)
	{
		const stl::lock_guard lock(m_lock);

		a_out << m_actorBlockList;

		if (m_forceDefaultConfig)
		{
			a_out << m_config.stash;
		}
		else
		{
			a_out << m_config.active;
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
		configStore_t    cfgStore;

		a_in >> blockList;
		a_in >> cfgStore;

		if (a_version < stl::underlying(SerializationVersion::kDataVersion9))
		{
			actorStateHolder_t actorState;
			a_in >> actorState;
		}

		FillGlobalSlotConfig(cfgStore.slot);
		IMaintenance::CleanConfigStore(cfgStore);
		IMaintenance::CleanBlockList(blockList);

		if (!m_config.settings.data.placementRandomization)
		{
			IMaintenance::ClearConfigStoreRand(cfgStore);
		}

		m_actorBlockList = std::move(blockList);

		if (m_forceDefaultConfig)
		{
			m_config.stash = std::move(cfgStore);
		}
		else
		{
			m_config.active = std::move(cfgStore);
		}

		if (a_version >= stl::underlying(SerializationVersion::kDataVersion7))
		{
			a_in >> *this;
		}

		//auto& v = r.get(ConfigSex::Female);

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
		UpdateActorInfo(m_objects);
	}

	void Controller::OnUIClose()
	{
		ClearActorInfo();
	}

	void Controller::JSOnDataImport()
	{
		FillGlobalSlotConfig(m_config.active.slot);
		IMaintenance::CleanConfigStore(m_config.active);
		QueueResetAll(ControllerUpdateFlags::kNone);

		if (auto& rt = UIGetRenderTask())
		{
			rt->QueueReset();
		}
	}

	/*auto Controller::ReceiveEvent(
		const SKSENiNodeUpdateEvent* a_evn,
		BSTEventSource<SKSENiNodeUpdateEvent>* a_dispatcher)
		-> EventResult
	{
		if (a_evn && a_evn->reference)
		{
			if (auto actor = a_evn->reference->As<Actor>())
			{
				{
					const stl::lock_guard lock(m_lock);

					auto it = m_objects.find(actor->formID);
					if (it != m_objects.end())
					{
						RemoveActorImpl(actor, it->second.GetHandle(), ControllerUpdateFlags::kNone);
					}
				}

				QueueEvaluate(actor, ControllerUpdateFlags::kNone);
			}
		}

		return EventResult::kEvent_Continue;
	}*/

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

		for (auto& e : m_objects)
		{
			m_activeHandles.emplace_back(e.second.GetHandle());
		}
	}

	void Controller::EvaluateStoredHandles(ControllerUpdateFlags a_flags)
	{
		const stl::lock_guard lock(m_lock);

		for (auto& e : m_activeHandles)
		{
			NiPointer<TESObjectREFR> ref;
			if (e.Lookup(ref))
			{
				QueueEvaluate(ref, a_flags);
			}
		}

		m_activeHandles.clear();
	}

	void Controller::ClearStoredHandles()
	{
		const stl::lock_guard lock(m_lock);

		m_activeHandles.clear();
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

			if (a_level == ObjectDatabaseLevel::kDisabled)
			{
				QueueObjectDatabaseClear();
			}
			else
			{
				QueueDatabaseCleanup();
			}
		}
	}

	void Controller::QueueSetLanguage(const stl::fixed_string& a_lang)
	{
		ITaskPool::AddTask([this, a_lang] {
			const stl::lock_guard lock(m_lock);

			SetLanguageImpl(a_lang);
		});
	}

	/*void Controller::QueueClearVariableStorage(bool a_requestEval)
	{
		ITaskPool::AddTask([this, a_requestEval] {
			const stl::lock_guard lock(m_lock);

			ClearConditionalVariables();

			if (a_requestEval)
			{
				for (auto& e : m_objects)
				{
					e.second.m_flags.set(
						ActorObjectHolderFlags::kWantVarUpdate |
						ActorObjectHolderFlags::kRequestEvalImmediate);
				}
			}
		});
	}*/
}