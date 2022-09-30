#include "pch.h"

#include "Controller.h"

#include "IED/EngineExtensions.h"
#include "IED/Inventory.h"
#include "IED/NodeMap.h"
#include "IED/UI/UIMain.h"
#include "IED/Util/Common.h"

#include "IED/UI/UIIntroBanner.h"

#include "Drivers/Input.h"
#include "Drivers/UI.h"

#include "IED/Parsers/JSONConfigStoreParser.h"

#include <ext/SKSEMessagingHandler.h>
#include <ext/SKSESerializationEventHandler.h>

namespace IED
{
	using namespace Util::Common;
	using namespace ::Util::Node;
	using namespace Data;

	Controller::Controller(
		const std::shared_ptr<const ConfigINI>& a_config) :
		ActorProcessorTask(*this),
		IEquipment(m_rngBase),
		EffectController(a_config->m_effectShaders),
		IAnimationManager(m_config.settings),
		m_iniconf(a_config),
		m_nodeOverrideEnabled(a_config->m_nodeOverrideEnabled),
		m_nodeOverridePlayerEnabled(a_config->m_nodeOverridePlayerEnabled),
		m_forceDefaultConfig(a_config->m_forceDefaultConfig),
		m_npcProcessingDisabled(a_config->m_disableNPCProcessing),
		m_applyTransformOverrides(a_config->m_applyTransformOverrides),
		m_enableCorpseScatter(a_config->m_enableCorpseScatter),
		m_forceOrigWeapXFRM(a_config->m_forceOrigWeapXFRM),
		m_bipedCache(
			a_config->m_bipedSlotCacheMaxSize,
			a_config->m_bipedSlotCacheMaxForms)
	{
		InitializeInputHandlers();

#if defined(IED_ENABLE_1D10T_SAFEGUARDS)
		m_activeWriteCMETransforms = a_config->m_activeWriteCMETransforms;
#endif
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
		stl::scoped_lock lock(m_lock);

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

		if (Drivers::UI::IsImInitialized())
		{
			InitializeUI();
		}

		if (EffectControllerEnabled())
		{
			SetEffectControllerParallelUpdates(
				settings.effectShaderParallelUpdates);
		}

		SetProcessorTaskRunAUState(settings.hkWeaponAnimations);

		m_safeToOpenUI = true;

		m_iniconf.reset();
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
			"Found SDS interface [%s %u.%u.%u, interface ver: %.8X]",
			result.intfc->GetPluginName(),
			GET_PLUGIN_VERSION_MAJOR(pluginVersion),
			GET_PLUGIN_VERSION_MINOR(pluginVersion),
			GET_PLUGIN_VERSION_REV(pluginVersion),
			result.intfc->GetInterfaceVersion());

		result.intfc->RegisterForPlayerShieldOnBackEvent(this);
	}

	static void UpdateSoundPairFromINI(
		const stl::optional<ConfigForm>& a_src,
		stl::optional<Game::FormID>&     a_dst)
	{
		if (a_src && !a_dst)
		{
			Game::FormID tmp;

			if (IData::GetPluginInfo().ResolveFormID(*a_src, tmp))
			{
				a_dst = tmp;
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
			if (!DispatchIntroBanner())
			{
				Warning("Couldn't dispatch intro banner render task");
			}
		}
	}

	bool Controller::DispatchIntroBanner()
	{
		auto task = make_timed_ui_task<UI::UIIntroBanner>(7000000, m_iniconf->m_introBannerVOffset);

		task->SetControlLock(false);
		task->SetFreezeTime(false);
		task->SetWantCursor(false);
		task->SetEnabledInMenu(true);
		task->EnableRestrictions(false);

		return Drivers::UI::AddTask(-0xFFFF, std::move(task));
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
		CleanConfigStore(m_config.initial);

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
				"XP32 aa: crc:[%d], xpe:[sword:%d, axe: %d, dagger: %d, mace: %d, 2hsword: %d, 2haxe: %d, bow: %d]",
				info->crc,
				info->get_base(AnimationWeaponType::Sword),
				info->get_base(AnimationWeaponType::Axe),
				info->get_base(AnimationWeaponType::Dagger),
				info->get_base(AnimationWeaponType::Mace),
				info->get_base(AnimationWeaponType::TwoHandedSword),
				info->get_base(AnimationWeaponType::TwoHandedAxe),
				info->get_base(AnimationWeaponType::Bow));
		}
	}

	void Controller::InitializeBSFixedStringTable()
	{
		ASSERT(StringCache::IsInitialized());

		stl::scoped_lock lock(m_lock);

		InitializeAnimationStrings();
	}

	void Controller::OnDataLoaded()
	{
		stl::scoped_lock lock(m_lock);

		InitializeFPStateData();
		InitializeData();

		ASSERT(SinkEventsT1());
		ASSERT(SinkEventsT2());

		if (IFPV_Detected())
		{
			Debug("IFPV detector plugin found");
		}

		SetProcessorTaskRunState(true);
	}

	void Controller::Evaluate(
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		stl::scoped_lock lock(m_lock);

		EvaluateImpl(a_actor, a_handle, a_flags);
	}

	void Controller::EvaluateImpl(
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto root = a_actor->GetNiRootNode(false);
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
		ActorObjectHolder&               a_objects,
		stl::flag<ControllerUpdateFlags> a_flags)
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

		if (a_objects.m_root != root)
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
				a_handle,
				a_objects,
				a_flags);
		}
	}

	bool Controller::RemoveActor(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		stl::scoped_lock lock(m_lock);

		return RemoveActorImpl(a_actor, a_handle, a_flags);
	}

	bool Controller::RemoveActor(
		TESObjectREFR*                   a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		stl::scoped_lock lock(m_lock);

		return RemoveActorImpl(a_actor, a_flags);
	}

	bool Controller::RemoveActor(
		Game::FormID                     a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		stl::scoped_lock lock(m_lock);

		return RemoveActorImpl(a_actor, a_flags);
	}

	void Controller::QueueNiNodeUpdate(Game::FormID a_actor)
	{
		ITaskPool::AddTask([this, a_actor]() {
			if (auto actor = a_actor.As<Actor>())
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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

			for (auto& e : m_objects)
			{
				EvaluateImpl(e.second, a_flags);
			}
		});
	}

	void Controller::QueueRequestEvaluateTransformsActor(
		Game::FormID a_actor,
		bool         a_noDefer) const
	{
		ITaskPool::AddTask([this, a_actor, a_noDefer]() {
			RequestEvaluateTransformsActor(a_actor, a_noDefer);
		});
	}

	void Controller::RequestEvaluateTransformsActor(
		Game::FormID a_actor,
		bool         a_noDefer) const
	{
		stl::scoped_lock lock(m_lock);

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
		stl::scoped_lock lock(m_lock);

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
		stl::scoped_lock lock(m_lock);

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
		stl::scoped_lock lock(m_lock);

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
		stl::flag<ControllerUpdateFlags> a_flags)
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
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::QueueLoadedActorTask(
			a_actor,
			[this, a_flags](
				Actor*            a_actor,
				Game::ActorHandle a_handle) {
				stl::scoped_lock lock(m_lock);

				ActorResetImpl(
					a_actor,
					static_cast<Game::ObjectRefHandle>(a_handle),
					a_flags);
			});
	}

	void Controller::QueueReset(
		Game::FormID                     a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_actor, a_flags]() {
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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

	void Controller::OnActorAcquire(ActorObjectHolder& a_holder)
	{
		if (!a_holder.m_cmeNodes.empty() &&
		    !a_holder.m_movNodes.empty())
		{
			for (auto& e : NodeOverrideData::GetExtraMovNodes())
			{
				a_holder.CreateExtraMovNodes(a_holder.m_npcroot, e);
			}
		}

		if (m_applyTransformOverrides)
		{
			a_holder.ApplyXP32NodeTransformOverrides(a_holder.m_npcroot);
		}

		if (m_config.settings.data.placementRandomization &&
		    !a_holder.m_movNodes.empty())
		{
			GenerateRandomPlacementEntries(a_holder);
		}
	}

	void Controller::QueueResetAll(
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_flags]() {
			stl::scoped_lock lock(m_lock);

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
		ITaskPool::AddTask([this, a_flags, a_slot]() {
			stl::scoped_lock lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				ActorResetImpl(e.second, e.first, a_flags, a_slot);
			}
		});
	}

	void Controller::QueueResetGearAll(
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_flags]() {
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

			ClearObjectsImpl();
		});
	}

	void Controller::QueueUpdateTransformSlot(
		Game::FormID a_actor,
		ObjectSlot   a_slot)
	{
		ITaskPool::AddTask([this, a_actor, a_slot] {
			stl::scoped_lock lock(m_lock);

			UpdateTransformSlotImpl(a_actor, a_slot);
		});
	}

	void Controller::QueueUpdateTransformSlotNPC(
		Game::FormID a_npc,
		ObjectSlot   a_slot)
	{
		ITaskPool::AddTask([this, a_npc, a_slot]() {
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

			for (auto& e : m_objects)
			{
				UpdateTransformSlotImpl(e.second, a_slot);
			}
		});
	}

	void Controller::QueueResetAAAll()
	{
		ITaskPool::AddTask([this] {
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);

			for (auto& e : m_objects)
			{
				UpdateCustomImpl(
					e.second,
					a_class,
					MakeTransformUpdateFunc());
			}
		});
	}

	void Controller::QueueEvaluateNearbyActors(bool a_removeFirst)
	{
		ITaskPool::AddTask([this, a_removeFirst] {
			stl::scoped_lock lock(m_lock);

			auto player = *g_thePlayer;
			if (player)
			{
				auto handle = player->GetHandle();
				if (handle.IsValid())
				{
					if (a_removeFirst)
					{
						RemoveActorImpl(
							player,
							static_cast<Game::ObjectRefHandle>(handle),
							ControllerUpdateFlags::kNone);
					}

					EvaluateImpl(
						player,
						static_cast<Game::ObjectRefHandle>(handle),
						ControllerUpdateFlags::kNone);
				}
			}

			auto pl = Game::ProcessLists::GetSingleton();
			if (!pl)
			{
				return;
			}

			for (auto& handle : pl->highActorHandles)
			{
				NiPointer<Actor> actor;

				if (!handle.Lookup(actor))
				{
					continue;
				}

				if (a_removeFirst)
				{
					RemoveActorImpl(
						actor,
						static_cast<Game::ObjectRefHandle>(handle),
						ControllerUpdateFlags::kNone);
				}

				EvaluateImpl(
					actor,
					static_cast<Game::ObjectRefHandle>(handle),
					ControllerUpdateFlags::kNone);
			}
		});
	}

	void Controller::QueueLookupFormInfo(
		Game::FormID              a_formId,
		form_lookup_result_func_t a_func)
	{
		ITaskPool::AddTask([this, a_formId, func = std::move(a_func)] {
			auto result = LookupFormInfo(a_formId);

			stl::scoped_lock lock(m_lock);

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

			stl::scoped_lock lock(m_lock);

			func(std::move(result));
		});
	}

	void Controller::QueueGetFormDatabase(form_db_get_func_t a_func)
	{
		ITaskPool::AddTask([this, func = std::move(a_func)] {
			stl::scoped_lock lock(m_lock);

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

			stl::scoped_lock lock(m_lock);
			func(result);
		});
	}

	void Controller::QueueUpdateSoundForms()
	{
		ITaskPool::AddTask([this] {
			stl::scoped_lock lock(m_lock);
			UpdateSoundForms();
		});
	}

	void Controller::QueueClearRand()
	{
		ITaskPool::AddTask([this] {
			stl::scoped_lock lock(m_lock);
			ClearConfigStoreRand(m_config.active);
		});
	}

	void Controller::QueueSendAnimationEventToActor(
		Game::FormID a_actor,
		std::string  a_event)
	{
		ITaskPool::AddTask([this, a_actor, ev = std::move(a_event)] {
			stl::scoped_lock lock(m_lock);

			auto& data = GetData();

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
			if (!actor)
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
		stl::scoped_lock lock(m_lock);

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
		stl::scoped_lock lock(m_lock);

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

		stl::scoped_lock lock(m_lock);

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
		stl::scoped_lock lock(m_lock);

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

	bool Controller::ProcessItemUpdate(
		processParams_t&                 a_params,
		const configBaseValues_t&        a_config,
		const Data::equipmentOverride_t* a_override,
		ObjectEntryBase&                 a_entry,
		bool                             a_visible,
		TESForm*                         a_currentModelForm)
	{
		auto& state = a_entry.state;

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

		const bool isVisible = state->nodes.rootNode->IsVisible();

		if (a_visible)
		{
			if (!isVisible)
			{
				if (a_params.flags.test(ControllerUpdateFlags::kVisibilitySounds))
				{
					PlayObjectSound(a_params, a_config, a_entry, true);
				}

				for (auto& e : state->dbEntries)
				{
					e->accessed = IPerfCounter::Query();
				}
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

		if (isVisible != a_visible)
		{
			state->nodes.rootNode->SetVisible(a_visible);

			a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
		}

		state->UpdateFlags(a_config);

		bool updateTransform = false;

		if (state->nodeDesc.name != a_config.targetNode.name)
		{
			AttachNodeImpl(
				a_params.npcroot,
				a_config.targetNode,
				a_config.flags.test(BaseFlags::kReferenceMode),
				a_entry);

			updateTransform = true;

			a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
		}

		if (state->transform != static_cast<const configTransform_t&>(a_config))
		{
			state->transform.Update(a_config);

			updateTransform = true;

			a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
		}

		if (updateTransform)
		{
			UpdateObjectTransform(
				state->transform,
				state->nodes.rootNode,
				state->nodes.ref);
		}

		if (!state->flags.test(ObjectEntryFlags::kIsGroup))
		{
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

		return true;
	}

	template <class Ta, class Tb>
	constexpr void Controller::UpdateObjectEffectShaders(
		processParams_t& a_params,
		const Ta&        a_config,
		Tb&              a_objectEntry,
		bool             a_updateValues) requires(  //
		(std::is_same_v<Ta, Data::configCustom_t>&&
	         std::is_same_v<Tb, ObjectEntryCustom>) ||
		(std::is_same_v<Ta, Data::configSlot_t> &&
	     std::is_same_v<Tb, ObjectEntrySlot>))
	{
		if (!EffectControllerEnabled() ||
		    !a_objectEntry.state)
		{
			return;
		}

		const configEffectShaderHolder_t* es;

		if constexpr (std::is_same_v<Ta, configCustom_t>)
		{
			es = a_config.get_effect_shader_sfp(
				a_params.collector.data,
				{ a_objectEntry.state->form, ItemData::GetItemSlotExtraGeneric(a_objectEntry.state->form) },
				a_params);
		}
		else if constexpr (std::is_same_v<Ta, configSlot_t>)
		{
			es = a_config.get_effect_shader_fp(
				a_params.collector.data,
				{ a_objectEntry.state->form, a_objectEntry.slotidex },
				a_params);
		}

		if (es)
		{
			if (a_objectEntry.state->effectShaders.UpdateIfChanged(
					a_objectEntry.state->nodes.rootNode,
					*es))
			{
				a_params.state.ResetEffectShaders(a_params.handle);
			}
			else
			{
				if (a_updateValues)
				{
					if (!a_objectEntry.state->effectShaders.UpdateConfigValues(*es))
					{
						a_objectEntry.state->effectShaders.Update(
							a_objectEntry.state->nodes.rootNode,
							*es);

						a_params.state.ResetEffectShaders(a_params.handle);
					}
				}
			}
		}
		else
		{
			if (a_objectEntry.state->effectShaders)
			{
				a_objectEntry.state->effectShaders.clear();

				a_params.state.ResetEffectShaders(a_params.handle);
			}
		}
	}

	void Controller::ProcessSlots(processParams_t& a_params)
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
			!settings.removeFavRestriction);

		auto equippedInfo = CreateEquippedItemInfo(pm);

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
				ObjectSlot slot;
				bool       equipped;
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

				const bool isEquipped = slot.slot == equippedInfo.leftSlot ||
				                        slot.slot == equippedInfo.rightSlot ||
				                        (slot.slot == ObjectSlot::kAmmo && a_params.collector.data.IsSlotEquipped(ObjectSlotExtra::kAmmo));

				slot.equipped = isEquipped;

				if (isEquipped && prio && prio->flags.test(SlotPriorityFlags::kAccountForEquipped))
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

		auto limit = prio ?
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
					RemoveObject(
						a_params.actor,
						a_params.handle,
						objectEntry,
						a_params.objects,
						a_params.flags);

					continue;
				}

				auto entry = m_config.active.slot.GetActor(
					a_params.actor->formID,
					a_params.npcOrTemplate->formID,
					a_params.race->formID,
					f.slot,
					hc);

				if (!entry)
				{
					RemoveObject(
						a_params.actor,
						a_params.handle,
						objectEntry,
						a_params.objects,
						a_params.flags);

					continue;
				}

				auto& configEntry = entry->get(a_params.configSex);

				if (!configEntry.run_filters(a_params))
				{
					RemoveObject(
						a_params.actor,
						a_params.handle,
						objectEntry,
						a_params.objects,
						a_params.flags);

					continue;
				}

				const auto item = SelectSlotItem(
					configEntry,
					candidates,
					objectEntry.slotState.lastEquipped);

				auto configOverride =
					!item ? configEntry.get_equipment_override(
								a_params.collector.data,
								a_params) :
                            configEntry.get_equipment_override_fp(
								a_params.collector.data,
								{ item->item->form, objectEntry.slotidex },
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
					RemoveObject(
						a_params.actor,
						a_params.handle,
						objectEntry,
						a_params.objects,
						a_params.flags);

					continue;
				}

				if (f.equipped)
				{
					if (settings.hideEquipped &&
					    !configEntry.slotFlags.test(SlotFlags::kAlwaysUnload))
					{
						if (!objectEntry.hideCountdown &&
						    objectEntry.IsNodeVisible())
						{
							objectEntry.hideCountdown = 2;
						}
					}
					else
					{
						RemoveObject(
							a_params.actor,
							a_params.handle,
							objectEntry,
							a_params.objects,
							ControllerUpdateFlags::kNone);
					}

					continue;
				}

				if (!item)
				{
					RemoveObject(
						a_params.actor,
						a_params.handle,
						objectEntry,
						a_params.objects,
						a_params.flags);

					continue;
				}

				objectEntry.ResetDeferredHide();

				bool visible = GetVisibilitySwitch(
					a_params.actor,
					usedBaseConf.flags,
					a_params);

				auto modelForm = usedBaseConf.forceModel.get_form();

				if (objectEntry.state &&
				    objectEntry.state->form == item->item->form)
				{
					if (ProcessItemUpdate(
							a_params,
							usedBaseConf,
							configOverride,
							objectEntry,
							visible,
							modelForm))
					{
						if (visible)
						{
							item->item->sharedCount--;
						}

						item.consume(candidates);

						UpdateObjectEffectShaders(
							a_params,
							configEntry,
							objectEntry,
							a_params.flags.test(ControllerUpdateFlags::kWantEffectShaderConfigUpdate));

						typeActive |= visible;

						continue;
					}
				}

				if (LoadAndAttach(
						a_params,
						usedBaseConf,
						configEntry,
						objectEntry,
						item->item->form,
						modelForm,
						ItemData::IsLeftWeaponSlot(f.slot),
						visible,
						false,
						settings.hkWeaponAnimations))
				{
					objectEntry.state->nodes.rootNode->SetVisible(visible);

					if (visible)
					{
						item->item->sharedCount--;
					}

					item.consume(candidates);

					UpdateObjectEffectShaders(
						a_params,
						configEntry,
						objectEntry,
						a_params.flags.test(ControllerUpdateFlags::kWantEffectShaderConfigUpdate));

					a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);

					typeActive |= visible;
				}

				// Debug("%X: (%.8X) attached | %u ", a_actor->formID, item->formID, slot);
			}

			if (!e.activeEquipped && typeActive)
			{
				activeTypes++;
			}
		}
	}

	bool Controller::GetVisibilitySwitch(
		Actor*               a_actor,
		stl::flag<BaseFlags> a_flags,
		processParams_t&     a_params)
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
		    a_params.get_using_furniture())
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
		actorLookupResult_t& a_out)
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
		actorLookupResult_t&     a_out)
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
		ObjectEntryCustom&    a_objectEntry)
	{
		if (a_config.customFlags.test(CustomFlags::kUseChance))
		{
			if (!a_objectEntry.cflags.test(CustomObjectEntryFlags::kProcessedChance))
			{
				if (m_rng1.Get(m_rngBase) > a_config.chance)
				{
					a_objectEntry.cflags.set(CustomObjectEntryFlags::kBlockedByChance);
				}
				else
				{
					a_objectEntry.cflags.clear(CustomObjectEntryFlags::kBlockedByChance);
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

	bool Controller::ProcessCustomEntry(
		processParams_t&      a_params,
		const configCustom_t& a_config,
		ObjectEntryCustom&    a_objectEntry)
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

			auto configOverride =
				a_config.get_equipment_override_sfp(
					a_params.collector.data,
					{ it->second.form, ItemData::GetItemSlotExtraGeneric(it->second.form) },
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

			auto& itemData = it->second;

			if (!itemData.form)
			{
				return false;
			}

			bool visible = GetVisibilitySwitch(
				a_params.actor,
				usedBaseConf.flags,
				a_params);

			auto* modelForm =
				usedBaseConf.forceModel.get_id() ?
					usedBaseConf.forceModel.get_form() :
                    a_config.modelForm.get_form();

			if (a_objectEntry.state &&
			    a_objectEntry.state->form == itemData.form)
			{
				if (a_config.customFlags.test(CustomFlags::kGroupMode) ==
				    a_objectEntry.cflags.test(CustomObjectEntryFlags::kGroupMode))
				{
					bool _visible = hasMinCount && visible;

					if (ProcessItemUpdate(
							a_params,
							usedBaseConf,
							configOverride,
							a_objectEntry,
							_visible,
							modelForm))
					{
						if (_visible)
						{
							if (a_config.customFlags.test_any(CustomFlags::kEquipmentModeMask))
							{
								itemData.sharedCount--;
							}
						}

						UpdateObjectEffectShaders(
							a_params,
							a_config,
							a_objectEntry,
							a_params.flags.test(ControllerUpdateFlags::kWantEffectShaderConfigUpdate));

						return true;
					}
				}
			}

			if (!hasMinCount)
			{
				return false;
			}

			bool result;

			if (a_config.customFlags.test(CustomFlags::kGroupMode))
			{
				result = LoadAndAttachGroup(
					a_params,
					usedBaseConf,
					a_config.group,
					a_objectEntry,
					itemData.form,
					a_config.customFlags.test(CustomFlags::kLeftWeapon),
					visible,
					a_config.customFlags.test(CustomFlags::kDisableHavok),
					settings.hkWeaponAnimations);

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
					settings.hkWeaponAnimations);

				a_objectEntry.cflags.clear(CustomObjectEntryFlags::kGroupMode);
			}

			if (result)
			{
				a_objectEntry.SetNodeVisible(visible);

				if (a_config.customFlags.test_any(CustomFlags::kEquipmentModeMask) && visible)
				{
					itemData.sharedCount--;
				}

				UpdateObjectEffectShaders(
					a_params,
					a_config,
					a_objectEntry,
					a_params.flags.test(ControllerUpdateFlags::kWantEffectShaderConfigUpdate));

				a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
			}

			return result;
		}
		else
		{
			if (!a_config.form.get_id() ||
			    a_config.form.get_id().IsTemporary())
			{
				a_objectEntry.clear_chance_flags();

				return false;
			}

			auto form = a_config.form.get_form();
			if (!form)
			{
				Debug(
					"%s: [%.8X] couldn't find form %.8X",
					__FUNCTION__,
					a_params.actor->formID.get(),
					a_config.form.get_id().get());

				a_objectEntry.clear_chance_flags();

				return false;
			}

			auto configOverride =
				a_config.get_equipment_override_sfp(
					a_params.collector.data,
					{ form, ItemData::GetItemSlotExtraGeneric(form) },
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

			bool visible = GetVisibilitySwitch(
				a_params.actor,
				usedBaseConf.flags,
				a_params);

			auto* modelForm =
				usedBaseConf.forceModel.get_id() ?
					usedBaseConf.forceModel.get_form() :
                    a_config.modelForm.get_form();

			if (a_objectEntry.state &&
			    a_objectEntry.state->form == form)
			{
				if (a_config.customFlags.test(CustomFlags::kGroupMode) ==
				    a_objectEntry.cflags.test(CustomObjectEntryFlags::kGroupMode))
				{
					if (ProcessItemUpdate(
							a_params,
							usedBaseConf,
							configOverride,
							a_objectEntry,
							visible,
							modelForm))
					{
						UpdateObjectEffectShaders(
							a_params,
							a_config,
							a_objectEntry,
							a_params.flags.test(ControllerUpdateFlags::kWantEffectShaderConfigUpdate));

						return true;
					}
				}
			}

			bool result;

			if (a_config.customFlags.test(CustomFlags::kGroupMode))
			{
				result = LoadAndAttachGroup(
					a_params,
					usedBaseConf,
					a_config.group,
					a_objectEntry,
					form,
					a_config.customFlags.test(CustomFlags::kLeftWeapon),
					visible,
					a_config.customFlags.test(CustomFlags::kDisableHavok),
					settings.hkWeaponAnimations);

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
					settings.hkWeaponAnimations);

				a_objectEntry.cflags.clear(CustomObjectEntryFlags::kGroupMode);
			}

			if (result)
			{
				a_objectEntry.SetNodeVisible(visible);

				UpdateObjectEffectShaders(
					a_params,
					a_config,
					a_objectEntry,
					a_params.flags.test(ControllerUpdateFlags::kWantEffectShaderConfigUpdate));

				a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
			}

			return result;
		}
	}

	void Controller::ProcessCustomEntryMap(
		processParams_t&                     a_params,
		const configCustomHolder_t&          a_confData,
		ActorObjectHolder::customEntryMap_t& a_entryMap)
	{
		for (auto& f : a_confData.data)
		{
			auto it = a_entryMap.try_emplace(f.first).first;

			if (!ProcessCustomEntry(
					a_params,
					f.second(a_params.configSex),
					it->second))
			{
				RemoveObject(
					a_params.actor,
					a_params.handle,
					it->second,
					a_params.objects,
					a_params.flags);
			}
		}
	}

	void Controller::ProcessCustomMap(
		processParams_t&               a_params,
		const configCustomPluginMap_t& a_confPluginMap,
		ConfigClass                    a_class)
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

	void Controller::ProcessCustom(processParams_t& a_params)
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
		stl::flag<ControllerUpdateFlags> a_flags)
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

		IncrementCounter();

		auto& objects = GetObjectHolder(
			a_actor,
			a_root,
			a_npcroot,
			*this,
			a_handle,
			m_nodeOverrideEnabled,
			m_nodeOverridePlayerEnabled,
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
				auto& objs = GetObjectHolder(
					a_actor,
					a_root,
					a_npcroot,
					*this,
					a_handle,
					m_nodeOverrideEnabled,
					m_nodeOverridePlayerEnabled,
					/*m_config.settings.data.hkWeaponAnimations &&
						m_config.settings.data.animEventForwarding,*/
					m_bipedCache.GetOrCreate(a_actor->formID, GetCounterValue()));

				EvaluateImpl(
					a_root,
					a_npcroot,
					a_actor,
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
				a_handle,
				objects,
				a_flags);
		}
	}

	void Controller::EvaluateImpl(
		NiNode*                          a_root,
		NiNode*                          a_npcroot,
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		ActorObjectHolder&               a_objects,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
#if defined(IED_ENABLE_STATS_G)
		PerfTimer pt;
		pt.Start();
#endif

		IncrementCounter();

		if (!IsActorBlockedImpl(a_actor->formID))
		{
			DoObjectEvaluation(
				a_root,
				a_npcroot,
				a_actor,
				a_handle,
				a_objects,
				a_flags);
		}
		else
		{
			RemoveActorGear(
				a_actor,
				a_handle,
				a_objects,
				a_flags);
		}

		if (m_config.settings.data.enableXP32AA)
		{
			UpdateAA(a_actor, a_objects.m_animState);
		}

		if (a_flags.test(ControllerUpdateFlags::kImmediateTransformUpdate))
		{
			a_objects.RequestTransformUpdate();
		}
		else
		{
			a_objects.RequestTransformUpdateDefer();
		}

#if defined(IED_ENABLE_STATS_G)
		Debug("G: [%.8X]: %f", a_actor->formID.get(), pt.Stop());
#endif
	}

	void Controller::UpdateBipedSlotCache(
		processParams_t&   a_params,
		ActorObjectHolder& a_objects)
	{
		/*PerfTimer pt;
		pt.Start();*/

		auto& data = a_objects.m_lastEquipped->data;

		auto& biped = a_params.actor->GetBiped1(false);
		if (!biped)
		{
			std::for_each(
				data.begin(),
				data.end(),
				[](auto& a_v) { a_v.occupied = false; });

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

	void Controller::DoObjectEvaluation(
		NiNode*                          a_root,
		NiNode*                          a_npcroot,
		Actor*                           a_actor,
		Game::ObjectRefHandle            a_handle,
		ActorObjectHolder&               a_objects,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto nrp = GetNPCRacePair(a_actor);
		if (!nrp)
		{
			return;
		}

		processParams_t params{
			a_root,
			a_npcroot,
			a_handle,
			a_objects.m_female ?
				ConfigSex::Female :
                ConfigSex::Male,
			a_flags,
			{ m_temp.sr, a_actor },
			a_actor,
			nrp->npc,
			nrp->npc->GetFirstNonTemporaryOrThis(),
			nrp->race,
			a_objects,
			*this
		};

		auto dataList = GetEntryDataList(a_actor);
		if (!dataList)
		{
			Debug(
				"%s [%u]: %.8X: missing container object list",
				__FUNCTION__,
				__LINE__,
				a_actor->formID.get());
		}

		UpdateBipedSlotCache(params, a_objects);

		params.collector.Run(
			*nrp->npc,
			dataList);

		if (!m_config.settings.data.disableNPCSlots ||
		    params.is_player())
		{
			ProcessSlots(params);
		}

		ProcessCustom(params);

		if (params.state.flags.test_any(ProcessStateUpdateFlags::kUpdateMask))
		{
			if (params.state.flags.test(ProcessStateUpdateFlags::kForceUpdate))
			{
				EngineExtensions::UpdateRoot(a_root);
			}
			else
			{
				UpdateNodeIfGamePaused(a_root);
			}

			a_objects.RequestTransformUpdate();
		}
	}

	void Controller::EvaluateImpl(
		ActorObjectHolder&               a_objects,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto handle = a_objects.GetHandle();

		NiPointer<TESObjectREFR> refr;
		if (!handle.Lookup(refr))
		{
			Warning(
				"%s [%u]: %.8X: could not lookup by handle (%.8X)",
				__FUNCTION__,
				__LINE__,
				a_objects.GetActorFormID().get(),
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

		if (refr != a_objects.m_actor)
		{
			Warning(
				"%s [%u]: actor mismatch (%.8X != %.8X)",
				__FUNCTION__,
				__LINE__,
				refr->formID.get(),
				a_objects.m_actor->formID.get());

			return;
		}

		EvaluateImpl(actor, handle, a_objects, a_flags);
	}

	void Controller::EvaluateTransformsImpl(Game::FormID a_actor)
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			EvaluateTransformsImpl(it->second);
		}
	}

	void Controller::EvaluateTransformsImpl(
		ActorObjectHolder& a_objects)
	{
		if (auto info = LookupCachedActorInfo(a_objects))
		{
			if (ProcessTransformsImpl(
					info->npcRoot,
					info->actor,
					info->npc,
					info->race,
					info->sex,
					a_objects,
					nullptr))
			{
				UpdateNodeIfGamePaused(info->root);
			}
		}
	}

	bool Controller::ProcessTransformsImpl(
		NiNode*                                a_npcRoot,
		Actor*                                 a_actor,
		TESNPC*                                a_npc,
		TESRace*                               a_race,
		ConfigSex                              a_sex,
		ActorObjectHolder&                     a_objects,
		const collectorData_t::container_type* a_equippedForms)
	{
#if defined(IED_ENABLE_STATS_T)
		PerfTimer pt;
		pt.Start();
#endif

		if (a_actor == *g_thePlayer &&
		    !m_nodeOverridePlayerEnabled)
		{
			return false;
		}

		if (a_objects.m_cmeNodes.empty() &&
		    a_objects.m_weapNodes.empty())
		{
			return false;
		}

		nodeOverrideParams_t params{
			a_npcRoot,
			a_actor,
			a_npc,
			a_npc->GetFirstNonTemporaryOrThis(),
			a_race,
			a_objects,
			*this
		};

		configStoreNodeOverride_t::holderCache_t hc;

		for (auto& e : a_objects.m_weapNodes)
		{
			auto r = m_config.active.transforms.GetActorPlacement(
				a_actor->formID,
				params.npcOrTemplate->formID,
				a_race->formID,
				e.nodeName,
				hc);

			if (r)
			{
				ApplyNodePlacement(r->get(a_sex), e, params);
			}
			else
			{
				ResetNodePlacement(e, std::addressof(params));
			}
		}

		for (auto& e : a_objects.m_cmeNodes)
		{
			auto r = m_config.active.transforms.GetActorTransform(
				a_actor->formID,
				params.npcOrTemplate->formID,
				a_race->formID,
				e.first,
				hc);

			e.second.cachedConfCME = r;

			if (r)
			{
				ApplyNodeVisibility(
					e.second.node,
					r->get(a_sex),
					params);
			}
		}

		for (auto& e : a_objects.m_cmeNodes)
		{
			if (e.second.cachedConfCME)
			{
				ApplyNodeOverride(
					e.first,
					e.second,
					e.second.cachedConfCME->get(a_sex),
					params);
			}
			else
			{
				// only called from main, no need to run checks
				ResetNodeOverrideImpl(e.second.node, e.second.orig);
			}
		}

		if (m_forceOrigWeapXFRM &&
		    EngineExtensions::IsWeaponAdjustDisabled())
		{
			for (auto& e : a_objects.m_weapNodes)
			{
				if (e.originalTransform)
				{
					e.node->m_localTransform = *e.originalTransform;
				}
			}
		}

		if (m_config.settings.data.enableXP32AA &&
		    a_objects.m_animState.flags.test(ActorAnimationState::Flags::kNeedUpdate))
		{
			UpdateAA(a_actor, a_objects.m_animState);
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
		bool eraseState = false;

		if (a_actor != *g_thePlayer)
		{
			if (auto it = m_objects.find(a_actor->formID); it != m_objects.end())
			{
				m_storedActorStates.data.insert_or_assign(a_actor->formID, it->second);
				eraseState = true;
			}
		}

		RemoveActorImpl(
			a_actor,
			a_handle,
			a_flags);

		EvaluateImpl(
			a_actor,
			a_handle,
			a_flags | ControllerUpdateFlags::kImmediateTransformUpdate);

		if (eraseState)
		{
			m_storedActorStates.data.erase(a_actor->formID);
		}
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
			a_flags);

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
						ControllerUpdateFlags::kNone);

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
						ControllerUpdateFlags::kNone);
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
						ControllerUpdateFlags::kNone);
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
		ActorObjectHolder&               a_objects,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		RemoveActorGear(a_actor, a_handle, a_objects, a_flags);
		EvaluateImpl(a_actor, a_handle, a_objects, a_flags);
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
			if (!objectEntry.state)
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

				objectEntry.state->transform.Update(conf);

				UpdateObjectTransform(
					objectEntry.state->transform,
					objectEntry.state->nodes.rootNode,
					objectEntry.state->nodes.ref);

				UpdateNodeIfGamePaused(info->root);
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
				if (!a_entry.state)
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

	const configBaseValues_t& Controller::GetConfigForActor(
		const cachedActorInfo_t& a_info,
		const configCustom_t&    a_config,
		const ObjectEntryCustom& a_entry)
	{
		assert(a_entry.state);

		ItemCandidateCollector collector(m_temp.sr, a_info.actor);

		collector.Run(
			*a_info.npc,
			GetEntryDataList(a_info.actor));

		CommonParams params{
			a_info.actor,
			a_info.npc,
			a_info.npcOrTemplate,
			a_info.race,
			a_info.objects,
			*this
		};

		if (auto eo = a_config.get_equipment_override_sfp(
				collector.data,
				{ a_entry.state->form, ItemData::GetItemSlotExtraGeneric(a_entry.state->form) },
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
		assert(a_entry.state);

		ItemCandidateCollector collector(m_temp.sr, a_info.actor);

		collector.Run(
			*a_info.npc,
			GetEntryDataList(a_info.actor));

		CommonParams params{
			a_info.actor,
			a_info.npc,
			a_info.npcOrTemplate,
			a_info.race,
			a_info.objects,
			*this
		};

		if (auto eo = a_config.get_equipment_override_fp(
				collector.data,
				{ a_entry.state->form, a_entry.slotidex },
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
				if (!a_func.func(a_info, itce->second, e.second))
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
						if (!a_func.func(a_info, itc2->second, f.second))
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

		a_func.clean = a_func.func(a_info, itc->second, itd->second);
	}

	void Controller::UpdateTransformCustomImpl(
		cachedActorInfo_t&       a_info,
		const configCustom_t&    a_configEntry,
		const configTransform_t& a_xfrmConfigEntry,
		ObjectEntryCustom&       a_entry)
	{
		if (!a_entry.state)
		{
			return;
		}

		a_entry.state->transform.Update(a_xfrmConfigEntry);

		UpdateObjectTransform(
			a_entry.state->transform,
			a_entry.state->nodes.rootNode,
			a_entry.state->nodes.ref);

		//a_entry.state->UpdateGroupTransforms(a_configEntry.group);

		for (auto& e : a_configEntry.group.entries)
		{
			if (auto it = a_entry.state->groupObjects.find(e.first);
			    it != a_entry.state->groupObjects.end())
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

		for (auto& e : a_entry.state->groupObjects)
		{
			UpdateObjectTransform(
				e.second.transform,
				e.second.rootNode,
				nullptr);
		}

		UpdateNodeIfGamePaused(a_info.root);
	}

	auto Controller::LookupCachedActorInfo(
		ActorObjectHolder& a_objects)
		-> std::optional<cachedActorInfo_t>
	{
		auto handle = a_objects.GetHandle();

		NiPointer<TESObjectREFR> refr;
		if (!handle.Lookup(refr))
		{
			Warning(
				"%s [%u]: %.8X: could not lookup by handle (%.8X)",
				__FUNCTION__,
				__LINE__,
				a_objects.m_formid,
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

		if (actor != a_objects.m_actor)
		{
			Warning(
				"%s [%u]: actor mismatch (%.8X != %.8X)",
				__FUNCTION__,
				__LINE__,
				actor->formID.get(),
				a_objects.m_actor->formID.get());
		}

		auto npc = actor->GetActorBase();
		if (!npc)
		{
			return {};
		}

		auto race = actor->race;
		if (!race)
		{
			race = npc->race;

			if (!race)
			{
				return {};
			}
		}

		auto root = actor->GetNiRootNode(false);
		if (!root)
		{
			Warning(
				"%s: %.8X: actor has no 3D",
				__FUNCTION__,
				actor->formID.get());

			return {};
		}

		if (root != a_objects.m_root)
		{
			Warning(
				"%s: %.8X: skeleton root mismatch",
				__FUNCTION__,
				actor->formID.get());

			QueueReset(actor, ControllerUpdateFlags::kNone);

			return {};
		}

		auto npcroot = FindNode(root, BSStringHolder::GetSingleton()->m_npcroot);
		if (!npcroot)
		{
			return {};
		}

		return std::make_optional<cachedActorInfo_t>(
			actor,
			handle,
			npc,
			npc->GetFirstNonTemporaryOrThis(),
			race,
			root,
			npcroot,
			npc->GetSex() == 1 ?
				ConfigSex::Female :
                ConfigSex::Male,
			a_objects);
	}

	void Controller::SaveLastEquippedItems(
		processParams_t&          a_params,
		const equippedItemInfo_t& a_info,
		ActorObjectHolder&        a_objectHolder)
	{
		auto ts = IPerfCounter::Query();

		if (a_info.rightSlot < ObjectSlot::kMax)
		{
			auto& slot = a_objectHolder.GetSlot(a_info.rightSlot);

			slot.slotState.lastEquipped     = a_info.right->formID;
			slot.slotState.lastSeenEquipped = ts;
		}

		if (a_info.leftSlot < ObjectSlot::kMax)
		{
			auto& slot = a_objectHolder.GetSlot(a_info.leftSlot);

			slot.slotState.lastEquipped     = a_info.left->formID;
			slot.slotState.lastSeenEquipped = ts;
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
				slot.slotState.lastSeenEquipped = ts;
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

			SaveSettings();

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
				QueueReset(
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
		if (a_evn)
		{
			QueueReset(
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
			if (a_evn->actor->IsActor())
			{
				if (auto form = a_evn->baseObject.Lookup())
				{
					if (IFormCommon::IsEquippableForm(form))
					{
						QueueRequestEvaluate(a_evn->actor->formID, false, true);
					}
				}
			}
		}

		return EventResult::kContinue;
	}

	auto Controller::ReceiveEvent(
		const TESContainerChangedEvent* a_evn,
		BSTEventSource<TESContainerChangedEvent>*)
		-> EventResult
	{
		if (a_evn)
		{
			if (auto form = a_evn->baseObj.Lookup())
			{
				if (IsInventoryForm(form))
				{
					if (a_evn->oldContainer)
					{
						if (auto oldContainer = a_evn->oldContainer.As<Actor>())
						{
							QueueRequestEvaluate(oldContainer->formID, true, false);
						}
					}

					if (a_evn->newContainer &&
					    a_evn->oldContainer != a_evn->newContainer)  // ?
					{
						if (auto newContainer = a_evn->newContainer.As<Actor>())
						{
							QueueRequestEvaluate(newContainer->formID, true, false);
						}
					}
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
					stl::scoped_lock lock(m_lock);

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

		FillGlobalSlotConfig(tmp.slot);
		CleanConfigStore(tmp);

		if (!SaveConfigStore(PATHS::DEFAULT_CONFIG_USER, tmp))
		{
			return false;
		}

		m_config.initial = std::move(tmp);

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

		stl::scoped_lock lock(m_lock);

		m_actorBlockList.clear();
		m_storedActorStates.clear();

		if (m_forceDefaultConfig)
		{
			m_config.stash = m_config.initial;
		}
		else
		{
			m_config.active = m_config.initial;
		}

		ClearObjectsImpl();
		ClearPlayerState();
		ClearObjectDatabase();

		m_bipedCache.clear();
		ResetCounter();
	}

	std::size_t Controller::Store(
		boost::archive::binary_oarchive& a_out)
	{
		stl::scoped_lock lock(m_lock);

		a_out << m_actorBlockList;

		if (m_forceDefaultConfig)
		{
			a_out << m_config.stash;
		}
		else
		{
			a_out << m_config.active;
		}

		actorStateHolder_t actorState(*this);

		a_out << actorState;
		a_out << *this;

		return 4;
	}

	std::size_t Controller::Load(
		SKSESerializationInterface*      a_intfc,
		std::uint32_t                    a_version,
		boost::archive::binary_iarchive& a_in)
	{
		if (a_version > stl::underlying(SerializationVersion::kCurrentVersion))
		{
			throw std::exception("unsupported version");
		}

		stl::scoped_lock lock(m_lock);

		actorBlockList_t   blockList;
		configStore_t      cfgStore;
		actorStateHolder_t actorState;

		a_in >> blockList;
		a_in >> cfgStore;
		a_in >> actorState;

		FillGlobalSlotConfig(cfgStore.slot);
		CleanConfigStore(cfgStore);
		CleanBlockList(blockList);

		if (!m_config.settings.data.placementRandomization)
		{
			ClearConfigStoreRand(cfgStore);
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

		m_storedActorStates = std::move(actorState);

		if (a_version >= stl::underlying(SerializationVersion::kDataVersion7))
		{
			a_in >> *this;
		}

		return 4;
	}

	void Controller::FillGlobalSlotConfig(
		configStoreSlot_t& a_data) const
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

	void Controller::SaveSettings()
	{
		stl::scoped_lock lock(m_lock);
		m_config.settings.SaveIfDirty();
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
		CleanConfigStore(m_config.active);
		QueueResetAll(ControllerUpdateFlags::kNone);
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
					stl::scoped_lock lock(m_lock);

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
			stl::scoped_lock lock(m_lock);
			UpdateActorInfo(a_actor);
		});
	}

	void Controller::QueueUpdateActorInfo(
		Game::FormID              a_actor,
		std::function<void(bool)> a_callback)
	{
		ITaskPool::AddTask([this, a_actor, callback = std::move(a_callback)]() {
			stl::scoped_lock lock(m_lock);
			callback(UpdateActorInfo(a_actor));
		});
	}

	void Controller::QueueUpdateNPCInfo(Game::FormID a_npc)
	{
		ITaskPool::AddTask([this, a_npc]() {
			stl::scoped_lock lock(m_lock);
			UpdateNPCInfo(a_npc);
		});
	}

	void Controller::QueueUpdateNPCInfo(
		Game::FormID              a_npc,
		std::function<void(bool)> a_callback)
	{
		ITaskPool::AddTask([this, a_npc, callback = std::move(a_callback)]() {
			stl::scoped_lock lock(m_lock);
			callback(UpdateNPCInfo(a_npc));
		});
	}

	void Controller::StoreActiveHandles()
	{
		stl::scoped_lock lock(m_lock);

		m_activeHandles.clear();

		for (auto& e : m_objects)
		{
			m_activeHandles.emplace_back(e.second.GetHandle());
		}
	}

	void Controller::EvaluateStoredHandles(ControllerUpdateFlags a_flags)
	{
		stl::scoped_lock lock(m_lock);

		for (auto& e : m_activeHandles)
		{
			NiPointer<TESObjectREFR> ref;
			if (e.Lookup(ref))
			{
				QueueEvaluate(ref, a_flags);
			}
		}

		ClearStoredHandles();
	}

	void Controller::ClearStoredHandles()
	{
		stl::scoped_lock lock(m_lock);

		m_activeHandles.clear();
	}

	void Controller::QueueObjectDatabaseClear()
	{
		ITaskPool::AddTask([this]() {
			stl::scoped_lock lock(m_lock);

			ClearObjectDatabase();
		});
	}

	void Controller::SetObjectDatabaseLevel(ObjectDatabaseLevel a_level)
	{
		stl::scoped_lock lock(m_lock);

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
		ITaskPool::AddTask([this, a_lang]() {
			stl::scoped_lock lock(m_lock);

			SetLanguageImpl(a_lang);
		});
	}

	void Controller::ProcessEffectShaders()
	{
		stl::scoped_lock lock(m_lock);

		ProcessEffects(m_objects);
	}

	void Controller::QueueSettingsSave(bool a_dirtyOnly)
	{
		ITaskPool::AddTask([this,
		                    a_dirtyOnly,
		                    sl = std::source_location::current()]() {
			stl::scoped_lock lock(m_lock);

			PerfTimer pt;
			pt.Start();

			auto& settings = GetConfigStore().settings;

			bool result = a_dirtyOnly ?
			                  settings.SaveIfDirty() :
                              settings.Save();

			if (result)
			{
				Debug(
					"%s: %fs",
					sl.function_name(),
					pt.Stop());
			}
		});
	}
}