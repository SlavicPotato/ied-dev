#include "pch.h"

#include "Controller.h"
#include "NodeProcessorTask.h"

#include "IED/EngineExtensions.h"
#include "IED/Inventory.h"
#include "IED/NodeMap.h"
#include "IED/UI/UIMain.h"
#include "IED/Util/Common.h"

#include "Drivers/Input.h"
#include "Drivers/UI.h"

#include "IED/Parsers/JSONConfigStoreParser.h"

namespace IED
{
	using namespace Util::Common;
	using namespace ::Util::Node;
	using namespace Data;

	Controller::Controller(
		const std::shared_ptr<const ConfigINI>& a_config) :
		NodeProcessorTask(*this),
		m_rng1(0.0f, 100.0f),
		m_iniconf(a_config),
		m_nodeOverrideEnabled(a_config->m_nodeOverrideEnabled),
		m_nodeOverridePlayerEnabled(a_config->m_nodeOverridePlayerEnabled),
		m_forceDefaultConfig(a_config->m_forceDefaultConfig),
		m_npcProcessingDisabled(a_config->m_disableNPCProcessing)
	{
		InitializeInputHandlers();
	}

	inline static constexpr bool IsActorValid(TESObjectREFR* a_refr) noexcept
	{
		if (a_refr == nullptr ||
		    a_refr->formID == 0 ||
		    a_refr->loadedState == nullptr ||
		    a_refr->IsDeleted() ||
		    !a_refr->IsActor())
		{
			return false;
		}
		return true;
	}

	void Controller::SinkInputEvents()
	{
		Drivers::Input::RegisterForKeyEvents(m_inputHandlers.playerBlock);

		if (m_iniconf->m_enableUI)
		{
			Drivers::Input::RegisterForPriorityKeyEvents(m_inputHandlers.uiToggle);
		}
	}

	void Controller::SinkEventsT0()
	{
		if (m_esif.test(EventSinkInstallationFlags::kT0))
		{
			return;
		}

		m_esif.set(EventSinkInstallationFlags::kT0);

		ITaskPool::AddTaskFixed(this);
	}

	bool Controller::SinkEventsT1()
	{
		if (m_esif.test(EventSinkInstallationFlags::kT1))
		{
			return true;
		}

		m_esif.set(EventSinkInstallationFlags::kT1);

		if (auto mm = MenuManager::GetSingleton())
		{
			auto dispatcher = mm->MenuOpenCloseEventDispatcher();
			dispatcher->AddEventSink(this);

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

		m_esif.set(EventSinkInstallationFlags::kT2);

		if (auto edl = ScriptEventSourceHolder::GetSingleton())
		{
			edl->AddEventSink<TESInitScriptEvent>(this);
			edl->AddEventSink<TESObjectLoadedEvent>(this);
			edl->AddEventSink<TESEquipEvent>(this);
			edl->AddEventSink<TESContainerChangedEvent>(this);
			edl->AddEventSink<TESFurnitureEvent>(this);
			edl->AddEventSink<TESDeathEvent>(this);
			edl->AddEventSink<TESSwitchRaceCompleteEvent>(this);
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
		IScopedLock lock(m_lock);

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

		auto& settings = m_config.settings.data;

		if (settings.logLevel)
		{
			gLog.SetLogLevel(*settings.logLevel);
		}

		ISKSE::GetBacklog().SetLimit(std::clamp<std::uint32_t>(settings.ui.logLimit, 1, 2000));

		auto& nodeMap = NodeMap::GetSingleton();

		if (!nodeMap.LoadExtra(PATHS::NODEMAP))
		{
			if (Serialization::FileExists(PATHS::NODEMAP))
			{
				Error(
					"%s: %s",
					PATHS::NODEMAP,
					nodeMap.GetLastException().what());
			}
		}

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

		if (m_iniconf->m_enableUI)
		{
			InitializeUI();
		}

		m_iniconf.reset();
	}

	static void UpdateSoundPairFromINI(
		const stl::optional<Data::ConfigForm>& a_src,
		stl::optional<Game::FormID>& a_dst)
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
		const Data::ConfigSound<Data::ConfigForm>::soundPair_t& a_src,
		Data::ConfigSound<Game::FormID>::soundPair_t& a_dst)
	{
		UpdateSoundPairFromINI(a_src.first, a_dst.first);
		UpdateSoundPairFromINI(a_src.second, a_dst.second);
	}

	void Controller::InitializeSound()
	{
		auto& settings = m_config.settings.data.sound;

		UpdateSoundPairFromINI(m_iniconf->m_sound.arrow, settings.arrow);
		UpdateSoundPairFromINI(m_iniconf->m_sound.armor, settings.armor);
		UpdateSoundPairFromINI(m_iniconf->m_sound.weapon, settings.weapon);
		UpdateSoundPairFromINI(m_iniconf->m_sound.gen, settings.gen);

		UpdateSoundForms();

		SetPlaySound(settings.enabled);
		SetPlaySoundNPC(settings.npc);
	}

	void Controller::UpdateSoundForms()
	{
		auto& settings = m_config.settings.data.sound;

		SetSounds(
			MakeSoundPair(settings.arrow),
			MakeSoundPair(settings.armor),
			MakeSoundPair(settings.weapon),
			MakeSoundPair(settings.gen));
	}

	void Controller::InitializeInputHandlers()
	{
		m_inputHandlers.playerBlock.SetLambda([this] {
			ITaskPool::AddTask(
				[this]() {
					TogglePlayerBlock();
				});
		});

		m_inputHandlers.playerBlock.SetProcessPaused(false);

		if (m_iniconf->m_enableUI)
		{
			m_inputHandlers.uiToggle.SetLambda(
				[this] {
					ITaskPool::AddTask([this] {
						UIOpen();
					});
				});
		}
	}

	void Controller::InitializeUI()
	{
		UIInitialize(*this);

		auto& config = m_config.settings.data;

		UIEnableRestrictions(config.ui.enableRestrictions);
		UISetLock(config.ui.enableControlLock);
		UISetEnabledInMenu(m_iniconf->m_uiEnableInMenu);

		if (m_iniconf->m_forceUIToggleKeys &&
		    m_iniconf->m_toggleUIKeys.Has())
		{
			m_inputHandlers.uiToggle.SetKeys(
				m_iniconf->m_toggleUIKeys.GetComboKey(),
				m_iniconf->m_toggleUIKeys.GetKey());

			m_iniKeysForced = true;
		}
		else
		{
			if (config.ui.toggleKeys)
			{
				m_inputHandlers.uiToggle.SetKeys(
					config.ui.toggleKeys->comboKey,
					config.ui.toggleKeys->key);
			}
			else if (m_iniconf->m_toggleUIKeys.Has())
			{
				m_inputHandlers.uiToggle.SetKeys(
					m_iniconf->m_toggleUIKeys.GetComboKey(),
					m_iniconf->m_toggleUIKeys.GetKey());
			}
		}

		if (!m_inputHandlers.uiToggle.Enabled())
		{
			m_inputHandlers.uiToggle.SetKeys(
				0,
				DIK_BACKSPACE);
		}

		if (!Math::IsEqual(config.ui.scale, 1.0f))
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
		auto& clang = settings.data.language;

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

	void Controller::InitializeStrings()
	{
		m_bsstrings = std::make_unique<BSStringHolder>();
	}

	void Controller::Evaluate(
		Actor* a_actor,
		Game::ObjectRefHandle a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		IScopedLock lock(m_lock);

		EvaluateImpl(a_actor, a_handle, a_flags);
	}

	void Controller::EvaluateImpl(
		Actor* a_actor,
		Game::ObjectRefHandle a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto root = a_actor->GetNiRootNode(false);
		if (!root)
		{
			return;
		}

		auto npcroot = FindNode(root, m_bsstrings->m_npcroot);
		if (!npcroot)
		{
			return;
		}

		EvaluateImpl(root, npcroot, a_actor, a_handle, a_flags);
	}

	void Controller::EvaluateImpl(
		Actor* a_actor,
		Game::ObjectRefHandle a_handle,
		ActorObjectHolder& a_objects,
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

		auto npcroot = FindNode(root, m_bsstrings->m_npcroot);
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

			QueueReset(a_actor, ControllerUpdateFlags::kNone);
		}
		else
		{
			EvaluateImpl(root, npcroot, a_actor, a_handle, a_objects, a_flags);
		}
	}

	bool Controller::RemoveActor(
		TESObjectREFR* a_actor,
		Game::ObjectRefHandle a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		IScopedLock lock(m_lock);

		return RemoveActorImpl(a_actor, a_handle, a_flags);
	}

	bool Controller::RemoveActor(
		TESObjectREFR* a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		IScopedLock lock(m_lock);

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
		TESObjectREFR* a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::QueueActorTask(
			a_actor,
			[this, a_flags](
				Actor* a_actor,
				Game::ActorHandle a_handle) {
				Evaluate(
					a_actor,
					static_cast<Game::ObjectRefHandle>(a_handle),
					a_flags);
			});
	}

	void Controller::QueueEvaluate(
		Game::FormID a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_actor, a_flags]() {
			IScopedLock lock(m_lock);

			actorLookupResult_t result;
			if (LookupTrackedActor(a_actor, result))
			{
				Evaluate(result.actor, result.handle, a_flags);
			}
		});
	}

	void Controller::QueueEvaluateNPC(
		Game::FormID a_npc,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_npc, a_flags]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				if (e.second.IsActorNPC(a_npc))
				{
					EvaluateImpl(e.second, a_flags);
				}
			}
		});
	}

	void Controller::QueueEvaluateRace(
		Game::FormID a_race,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_race, a_flags]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				if (e.second.IsActorRace(a_race))
				{
					EvaluateImpl(e.second, a_flags);
				}
			}
		});
	}

	void Controller::QueueRequestEvaluate(
		Game::FormID a_actor,
		bool a_defer) const
	{
		ITaskPool::AddTask([this, a_actor, a_defer]() {
			RequestEvaluate(a_actor, a_defer);
		});
	}

	void Controller::RequestEvaluate(Game::FormID a_actor, bool a_defer) const
	{
		IScopedLock lock(m_lock);

		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			if (a_defer)
			{
				it->second.RequestEvalDefer();
			}
			else
			{
				it->second.RequestEval();
			}
		}
	}

	void Controller::QueueEvaluateAll(
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_flags]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				EvaluateImpl(e.second, a_flags);
			}
		});
	}

	void Controller::QueueRequestEvaluateTransformsActor(
		Game::FormID a_actor,
		bool a_noDefer) const
	{
		ITaskPool::AddTask([this, a_actor, a_noDefer]() {
			RequestEvaluateTransformsActor(a_actor, a_noDefer);
		});
	}

	void Controller::RequestEvaluateTransformsActor(
		Game::FormID a_actor,
		bool a_noDefer) const
	{
		IScopedLock lock(m_lock);

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
		bool a_noDefer) const
	{
		IScopedLock lock(m_lock);

		if (!m_nodeOverrideEnabled)
		{
			return;
		}

		for (auto& e : m_objects)
		{
			if (e.second.IsActorNPC(a_npc))
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
		bool a_noDefer) const
	{
		IScopedLock lock(m_lock);

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
		IScopedLock lock(m_lock);

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
		TESObjectREFR* a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::QueueActorTask(
			a_actor,
			[this, a_flags](Actor* a_actor, Game::ActorHandle a_handle) {
				RemoveActor(
					a_actor,
					static_cast<Game::ObjectRefHandle>(a_handle),
					a_flags);
			});
	}

	void Controller::QueueReset(
		TESObjectREFR* a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::QueueActorTask(
			a_actor,
			[this, a_flags](
				Actor* a_actor,
				Game::ActorHandle a_handle) {
				IScopedLock lock(m_lock);

				ActorResetImpl(
					a_actor,
					static_cast<Game::ObjectRefHandle>(a_handle),
					a_flags);
			});
	}

	void Controller::QueueReset(
		Game::FormID a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_actor, a_flags]() {
			IScopedLock lock(m_lock);

			actorLookupResult_t result;
			if (LookupTrackedActor(a_actor, result))
			{
				ActorResetImpl(result.actor, result.handle, a_flags);
			}
		});
	}

	void Controller::QueueResetNPC(
		Game::FormID a_npc,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_npc, a_flags]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				actorLookupResult_t result;
				if (!LookupTrackedActor(e.second, result))
				{
					continue;
				}

				if (auto baseForm = result.actor->baseForm)
				{
					if (auto npc = baseForm->As<TESNPC>())
					{
						if (npc->formID == a_npc)
						{
							ActorResetImpl(
								result.actor,
								result.handle,
								a_flags);
						}
					}
				}
			}
		});
	}

	void Controller::QueueResetRace(
		Game::FormID a_race,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_race, a_flags]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				actorLookupResult_t result;
				if (!LookupTrackedActor(e.second, result))
				{
					continue;
				}

				if (auto race = Game::GetActorRace(result.actor))
				{
					if (race->formID == a_race)
					{
						ActorResetImpl(
							result.actor,
							result.handle,
							a_flags);
					}
				}
			}
		});
	}

	void Controller::QueueReset(
		Game::FormID a_actor,
		stl::flag<ControllerUpdateFlags> a_flags,
		Data::ObjectSlot a_slot)
	{
		ITaskPool::AddTask([this, a_actor, a_flags, a_slot]() {
			IScopedLock lock(m_lock);

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
		Game::FormID a_npc,
		stl::flag<ControllerUpdateFlags> a_flags,
		Data::ObjectSlot a_slot)
	{
		ITaskPool::AddTask([this, a_npc, a_flags, a_slot]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				actorLookupResult_t result;
				if (!LookupTrackedActor(e.second, result))
				{
					continue;
				}

				if (auto baseForm = result.actor->baseForm)
				{
					if (auto npc = baseForm->As<TESNPC>())
					{
						if (npc->formID == a_npc)
						{
							ActorResetImpl(
								result.actor,
								result.handle,
								a_flags,
								a_slot);
						}
					}
				}
			}
		});
	}

	void Controller::QueueResetRace(
		Game::FormID a_race,
		stl::flag<ControllerUpdateFlags> a_flags,
		Data::ObjectSlot a_slot)
	{
		ITaskPool::AddTask([this, a_race, a_flags, a_slot]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				actorLookupResult_t result;
				if (!LookupTrackedActor(e.second, result))
				{
					continue;
				}

				if (auto race = Game::GetActorRace(result.actor))
				{
					if (race->formID == a_race)
					{
						ActorResetImpl(
							result.actor,
							result.handle,
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
			auto handle = e.second.GetHandle();

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

	void Controller::QueueResetAll(
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		ITaskPool::AddTask([this, a_flags]() {
			IScopedLock lock(m_lock);

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
		Data::ObjectSlot a_slot)
	{
		ITaskPool::AddTask([this, a_flags, a_slot]() {
			IScopedLock lock(m_lock);

			actorLookupResultMap_t result;
			CollectKnownActors(result);

			for (auto& e : result)
			{
				ActorResetImpl(e.second, e.first, a_flags, a_slot);
			}
		});
	}

	void Controller::QueueClearObjects()
	{
		ITaskPool::AddTask([this] {
			IScopedLock lock(m_lock);

			ClearObjectsImpl();
		});
	}

	void Controller::QueueUpdateTransformSlot(
		Game::FormID a_actor,
		Data::ObjectSlot a_slot)
	{
		ITaskPool::AddTask([this, a_actor, a_slot] {
			IScopedLock lock(m_lock);

			UpdateTransformSlotImpl(a_actor, a_slot);
		});
	}

	void Controller::QueueUpdateTransformSlotNPC(
		Game::FormID a_npc,
		Data::ObjectSlot a_slot)
	{
		ITaskPool::AddTask([this, a_npc, a_slot]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				if (e.second.IsActorNPC(a_npc))
				{
					UpdateTransformSlotImpl(e.second, a_slot);
				}
			}
		});
	}

	void Controller::QueueUpdateTransformSlotRace(
		Game::FormID a_race,
		Data::ObjectSlot a_slot)
	{
		ITaskPool::AddTask([this, a_race, a_slot]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				if (e.second.IsActorRace(a_race))
				{
					UpdateTransformSlotImpl(e.second, a_slot);
				}
			}
		});
	}

	void Controller::QueueUpdateTransformSlotAll(Data::ObjectSlot a_slot)
	{
		ITaskPool::AddTask([this, a_slot] {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				UpdateTransformSlotImpl(e.second, a_slot);
			}
		});
	}

	void Controller::QueueAttachSlotNode(
		Game::FormID a_actor,
		Data::ObjectSlot a_slot,
		bool a_evalIfNone)
	{
		ITaskPool::AddTask([this, a_actor, a_slot, a_evalIfNone] {
			IScopedLock lock(m_lock);

			AttachSlotNodeImpl(
				a_actor,
				a_slot,
				a_evalIfNone);
		});
	}

	void Controller::QueueAttachSlotNodeNPC(
		Game::FormID a_npc,
		Data::ObjectSlot a_slot,
		bool a_evalIfNone)
	{
		ITaskPool::AddTask([this, a_npc, a_slot, a_evalIfNone]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				if (e.second.IsActorNPC(a_npc))
				{
					AttachSlotNodeImpl(
						e.second,
						a_slot,
						a_evalIfNone);
				}
			}
		});
	}

	void Controller::QueueAttachSlotNodeRace(
		Game::FormID a_race,
		Data::ObjectSlot a_slot,
		bool a_evalIfNone)
	{
		ITaskPool::AddTask([this, a_race, a_slot, a_evalIfNone]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				if (e.second.IsActorRace(a_race))
				{
					AttachSlotNodeImpl(
						e.second,
						a_slot,
						a_evalIfNone);
				}
			}
		});
	}

	void Controller::QueueAttachSlotNodeAll(
		Data::ObjectSlot a_slot,
		bool a_evalIfNone)
	{
		ITaskPool::AddTask([this, a_slot, a_evalIfNone] {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				AttachSlotNodeImpl(
					e.second,
					a_slot,
					a_evalIfNone);
			}
		});
	}

	void Controller::QueueResetCustom(
		Game::FormID a_actor,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_actor, a_class, a_pkey, a_vkey]() {
			IScopedLock lock(m_lock);

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
		Game::FormID a_npc,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_npc, a_class, a_pkey, a_vkey]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				actorLookupResult_t result;
				if (!LookupTrackedActor(e.second, result))
				{
					continue;
				}

				if (auto baseForm = result.actor->baseForm)
				{
					if (auto npc = baseForm->As<TESNPC>())
					{
						if (npc->formID == a_npc)
						{
							ResetCustomImpl(
								result.actor,
								result.handle,
								a_class,
								a_pkey,
								a_vkey);
						}
					}
				}
			}
		});
	}

	void Controller::QueueResetCustomRace(
		Game::FormID a_race,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_race, a_class, a_pkey, a_vkey]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				actorLookupResult_t result;
				if (!LookupTrackedActor(e.second, result))
				{
					continue;
				}

				if (auto race = Game::GetActorRace(result.actor))
				{
					if (race->formID == a_race)
					{
						ResetCustomImpl(
							result.actor,
							result.handle,
							a_class,
							a_pkey,
							a_vkey);
					}
				}
			}
		});
	}

	void Controller::QueueResetCustom(
		Game::FormID a_actor,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_actor, a_class, a_pkey]() {
			IScopedLock lock(m_lock);

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
		Game::FormID a_npc,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_npc, a_class, a_pkey]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				actorLookupResult_t result;
				if (!LookupTrackedActor(e.second, result))
				{
					continue;
				}

				if (auto baseForm = result.actor->baseForm)
				{
					if (auto npc = baseForm->As<TESNPC>())
					{
						if (npc->formID == a_npc)
						{
							ResetCustomImpl(
								result.actor,
								result.handle,
								a_class,
								a_pkey);
						}
					}
				}
			}
		});
	}

	void Controller::QueueResetCustomRace(
		Game::FormID a_race,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_race, a_class, a_pkey]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				actorLookupResult_t result;
				if (!LookupTrackedActor(e.second, result))
				{
					continue;
				}

				if (auto race = Game::GetActorRace(result.actor))
				{
					if (race->formID == a_race)
					{
						ResetCustomImpl(
							result.actor,
							result.handle,
							a_class,
							a_pkey);
					}
				}
			}
		});
	}

	void Controller::QueueResetCustom(
		Game::FormID a_actor,
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_actor, a_class]() {
			IScopedLock lock(m_lock);

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
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_npc, a_class]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				actorLookupResult_t result;
				if (!LookupTrackedActor(e.second, result))
				{
					continue;
				}

				if (auto baseForm = result.actor->baseForm)
				{
					if (auto npc = baseForm->As<TESNPC>())
					{
						if (npc->formID == a_npc)
						{
							ResetCustomImpl(
								result.actor,
								result.handle,
								a_class);
						}
					}
				}
			}
		});
	}

	void Controller::QueueResetCustomRace(
		Game::FormID a_race,
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_race, a_class]() {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				actorLookupResult_t result;
				if (!LookupTrackedActor(e.second, result))
				{
					continue;
				}

				if (auto race = Game::GetActorRace(result.actor))
				{
					if (race->formID == a_race)
					{
						ResetCustomImpl(
							result.actor,
							result.handle,
							a_class);
					}
				}
			}
		});
	}

	void Controller::QueueResetCustomAll(
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_class, a_pkey, a_vkey]() {
			IScopedLock lock(m_lock);

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
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_class, a_pkey]() {
			IScopedLock lock(m_lock);

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
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_class]() {
			IScopedLock lock(m_lock);

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
		Game::FormID a_actor,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_actor, a_class, a_pkey, a_vkey] {
			IScopedLock lock(m_lock);

			UpdateCustomImpl(
				a_actor,
				a_class,
				a_pkey,
				a_vkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomNPC(
		Game::FormID a_npc,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_npc, a_class, a_pkey, a_vkey] {
			IScopedLock lock(m_lock);

			UpdateCustomNPCImpl(
				a_npc,
				a_class,
				a_pkey,
				a_vkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomRace(
		Game::FormID a_race,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_race, a_class, a_pkey, a_vkey] {
			IScopedLock lock(m_lock);

			UpdateCustomRaceImpl(
				a_race,
				a_class,
				a_pkey,
				a_vkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustom(
		Game::FormID a_actor,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_actor, a_class, a_pkey] {
			IScopedLock lock(m_lock);

			UpdateCustomImpl(
				a_actor,
				a_class,
				a_pkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomNPC(
		Game::FormID a_npc,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_npc, a_class, a_pkey] {
			IScopedLock lock(m_lock);

			UpdateCustomNPCImpl(
				a_npc,
				a_class,
				a_pkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomRace(
		Game::FormID a_race,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_race, a_class, a_pkey] {
			IScopedLock lock(m_lock);

			UpdateCustomRaceImpl(
				a_race,
				a_class,
				a_pkey,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustom(
		Game::FormID a_actor,
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_actor, a_class] {
			IScopedLock lock(m_lock);

			UpdateCustomImpl(
				a_actor,
				a_class,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomNPC(
		Game::FormID a_npc,
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_npc, a_class] {
			IScopedLock lock(m_lock);

			UpdateCustomNPCImpl(
				a_npc,
				a_class,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomRace(
		Game::FormID a_race,
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_race, a_class] {
			IScopedLock lock(m_lock);

			UpdateCustomRaceImpl(
				a_race,
				a_class,
				MakeTransformUpdateFunc());
		});
	}

	void Controller::QueueUpdateTransformCustomAll(
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_class, a_pkey, a_vkey] {
			IScopedLock lock(m_lock);

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
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_class, a_pkey] {
			IScopedLock lock(m_lock);

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
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_class] {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				UpdateCustomImpl(
					e.second,
					a_class,
					MakeTransformUpdateFunc());
			}
		});
	}

	void Controller::QueueUpdateAttachCustom(
		Game::FormID a_actor,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_actor, a_class, a_pkey, a_vkey] {
			IScopedLock lock(m_lock);

			UpdateCustomImpl(
				a_actor,
				a_class,
				a_pkey,
				a_vkey,
				MakeAttachUpdateFunc());
		});
	}

	void Controller::QueueUpdateAttachCustomNPC(
		Game::FormID a_npc,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_npc, a_class, a_pkey, a_vkey] {
			IScopedLock lock(m_lock);

			UpdateCustomNPCImpl(
				a_npc,
				a_class,
				a_pkey,
				a_vkey,
				MakeAttachUpdateFunc());
		});
	}

	void Controller::QueueUpdateAttachCustomRace(
		Game::FormID a_race,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_race, a_class, a_pkey, a_vkey] {
			IScopedLock lock(m_lock);

			UpdateCustomRaceImpl(
				a_race,
				a_class,
				a_pkey,
				a_vkey,
				MakeAttachUpdateFunc());
		});
	}

	void Controller::QueueUpdateAttachCustom(
		Game::FormID a_actor,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_actor, a_class, a_pkey] {
			IScopedLock lock(m_lock);

			UpdateCustomImpl(
				a_actor,
				a_class,
				a_pkey,
				MakeAttachUpdateFunc());
		});
	}

	void Controller::QueueUpdateAttachNPC(
		Game::FormID a_npc,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_npc, a_class, a_pkey] {
			IScopedLock lock(m_lock);

			UpdateCustomNPCImpl(
				a_npc,
				a_class,
				a_pkey,
				MakeAttachUpdateFunc());
		});
	}

	void Controller::QueueUpdateAttachRace(
		Game::FormID a_race,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_race, a_class, a_pkey] {
			IScopedLock lock(m_lock);

			UpdateCustomRaceImpl(
				a_race,
				a_class,
				a_pkey,
				MakeAttachUpdateFunc());
		});
	}

	void Controller::QueueUpdateAttachCustom(
		Game::FormID a_actor,
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_actor, a_class] {
			IScopedLock lock(m_lock);

			UpdateCustomImpl(
				a_actor,
				a_class,
				MakeAttachUpdateFunc());
		});
	}

	void Controller::QueueUpdateAttachCustomNPC(
		Game::FormID a_npc,
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_npc, a_class] {
			IScopedLock lock(m_lock);

			UpdateCustomNPCImpl(
				a_npc,
				a_class,
				MakeAttachUpdateFunc());
		});
	}

	void Controller::QueueUpdateAttachCustomRace(
		Game::FormID a_race,
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_race, a_class] {
			IScopedLock lock(m_lock);

			UpdateCustomRaceImpl(
				a_race,
				a_class,
				MakeAttachUpdateFunc());
		});
	}

	void Controller::QueueUpdateAttachCustomAll(
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey)
	{
		ITaskPool::AddTask([this, a_class, a_pkey, a_vkey] {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				UpdateCustomImpl(
					e.second,
					a_class,
					a_pkey,
					a_vkey,
					MakeAttachUpdateFunc());
			}
		});
	}

	void Controller::QueueUpdateAttachCustomAll(
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey)
	{
		ITaskPool::AddTask([this, a_class, a_pkey] {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				UpdateCustomImpl(
					e.second,
					a_class,
					a_pkey,
					MakeAttachUpdateFunc());
			}
		});
	}

	void Controller::QueueUpdateAttachCustomAll(
		Data::ConfigClass a_class)
	{
		ITaskPool::AddTask([this, a_class] {
			IScopedLock lock(m_lock);

			for (auto& e : m_objects)
			{
				UpdateCustomImpl(
					e.second,
					a_class,
					MakeAttachUpdateFunc());
			}
		});
	}

	void Controller::QueueEvaluateNearbyActors(bool a_removeFirst)
	{
		ITaskPool::AddTask([this, a_removeFirst] {
			IScopedLock lock(m_lock);

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
		Game::FormID a_formId,
		form_lookup_result_func_t a_func)
	{
		ITaskPool::AddTask([this, a_formId, func = std::move(a_func)] {
			auto result = LookupFormInfo(a_formId);

			IScopedLock lock(m_lock);

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

			IScopedLock lock(m_lock);

			func(std::move(result));
		});
	}

	void Controller::QueueGetFormDatabase(form_db_get_func_t a_func)
	{
		ITaskPool::AddTask([this, func = std::move(a_func)] {
			IScopedLock lock(m_lock);

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

			IScopedLock lock(m_lock);
			func(result);
		});
	}

	void Controller::QueueUpdateSoundForms()
	{
		ITaskPool::AddTask([this] {
			IScopedLock lock(m_lock);
			UpdateSoundForms();
		});
	}

	void Controller::AddActorBlock(
		Game::FormID a_actor,
		const stl::fixed_string& a_key)
	{
		IScopedLock lock(m_lock);

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
		Game::FormID a_actor,
		const stl::fixed_string& a_key)
	{
		IScopedLock lock(m_lock);

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

		IScopedLock lock(m_lock);

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
		IScopedLock lock(m_lock);

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

	bool Controller::ProcessItemUpdate(
		processParams_t& a_params,
		const Data::configBaseValues_t& a_config,
		const Data::configModelGroup_t* a_groupConfig,
		const Data::NodeDescriptor& a_node,
		objectEntryBase_t& a_entry,
		bool a_visible)
	{
		if (a_entry.state->resetTriggerFlags !=
		    (a_config.flags & Data::FlagsBase::kResetTriggerFlags))
		{
			return false;
		}

		bool isVisible = a_entry.state->nodes.obj->IsVisible();

		if (a_visible)
		{
			if (!isVisible)
			{
				if (a_params.flags.test(ControllerUpdateFlags::kVisibilitySounds))
				{
					PlayObjectSound(a_params, a_config, a_entry, true);
				}

				for (auto& e : a_entry.state->dbEntries)
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

		if (a_visible != isVisible)
		{
			a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
		}

		a_entry.state->nodes.obj->SetVisible(a_visible);

		a_entry.state->UpdateFlags(a_config);

		if (a_entry.state->nodeDesc.name != a_node.name)
		{
			AttachNodeImpl(
				a_params.npcroot,
				a_node,
				a_config.flags.test(Data::FlagsBase::kReferenceMode),
				a_entry);
		}

		a_entry.state->transform.Update(a_config);

		UpdateObjectTransform(
			a_entry.state->transform,
			a_entry.state->nodes.obj,
			a_entry.state->nodes.ref);

		/*if (a_groupConfig)
		{
			a_entry.state->UpdateGroupTransforms(*a_groupConfig);

			for (auto& e : a_entry.state->groupObjects)
			{
				UpdateObjectTransform(
					e.second.transform,
					e.second.object,
					nullptr);
			}
		}*/

		a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);

		return true;
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

		a_params.collector.GenerateSlotCandidates();

		auto equippedInfo = CreateEquippedItemInfo(pm);

		SaveLastEquippedItems(
			a_params.actor,
			equippedInfo,
			a_params.objects);

		Data::configStoreSlot_t::holderCache_t hc;

		using enum_type = std::underlying_type_t<ObjectType>;

		for (enum_type i = 0; i < stl::underlying(ObjectType::kMax); i++)
		{
			auto type = static_cast<ObjectType>(i);

			auto equipmentFlag = ItemData::GetRaceEquipmentFlagFromType(type);
			auto mainSlot = ItemData::GetSlotFromType(type);

			ObjectSlot slots[2]{
				mainSlot,
				ObjectSlot::kMax
			};

			if (auto leftSlot = ItemData::GetLeftSlot(mainSlot);
			    leftSlot != ObjectSlot::kMax)
			{
				auto& cer = a_params.objects.GetSlot(mainSlot);
				auto& cel = a_params.objects.GetSlot(leftSlot);

				if (cel.slotState.lastEquipped &&
				    (!cer.slotState.lastEquipped ||
				     cer.slotState.lastSeenEquipped < cel.slotState.lastSeenEquipped))
				{
					slots[0] = leftSlot;
					slots[1] = mainSlot;
				}
				else
				{
					slots[1] = leftSlot;
				}
			}

			auto& candidates = a_params.collector.m_slotResults[stl::underlying(type)].m_items;

			for (auto slot : slots)
			{
				if (slot == ObjectSlot::kMax)
				{
					continue;
				}

				auto& objectEntry = a_params.objects.GetSlot(slot);

				if (a_params.actor != *g_thePlayer &&
				    (!equipmentFlag || (a_params.race->validEquipTypes & equipmentFlag) != equipmentFlag))
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
					a_params.npc->formID,
					a_params.race->formID,
					slot,
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

				auto item = SelectItem(
					a_params.actor,
					configEntry,
					candidates,
					objectEntry.slotState.lastEquipped);

				const configBaseValues_t* usedConf =
					!item ? configEntry.get_equipment_override(
								a_params.collector.m_data,
								a_params) :
                            configEntry.get_equipment_override(
								a_params.collector.m_data,
								{ item->form, ItemData::SlotToExtraSlot(objectEntry.slotid) },
								a_params);

				if (!usedConf)
				{
					usedConf = std::addressof(configEntry);
				}

				if (usedConf->flags.test(FlagsBase::kDisabled))
				{
					RemoveObject(
						a_params.actor,
						a_params.handle,
						objectEntry,
						a_params.objects,
						a_params.flags);

					continue;
				}

				if (slot == equippedInfo.leftSlot ||
				    slot == equippedInfo.rightSlot)
				{
					auto& settings = m_config.settings.data;

					if (settings.hideEquipped &&
					    !configEntry.slotFlags.test(SlotFlags::kAlwaysUnload))
					{
						if (!objectEntry.hideCountdown &&
						    objectEntry.state &&
						    objectEntry.state->nodes.obj->IsVisible())
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

				objectEntry.ResetDeferedHide();

				bool visible = GetVisibilitySwitch(
					a_params.actor,
					usedConf->flags,
					a_params);

				if (objectEntry.state && objectEntry.state->form == item->form)
				{
					if (ProcessItemUpdate(
							a_params,
							*usedConf,
							nullptr,
							usedConf->targetNode,
							objectEntry,
							visible))
					{
						if (visible)
						{
							item->item->sharedCount--;
						}

						item.consume(candidates);
						continue;
					}
				}

				if (LoadAndAttach(
						a_params,
						*usedConf,
						usedConf->targetNode,
						objectEntry,
						item->form,
						nullptr,
						ItemData::IsLeftWeaponSlot(slot),
						false,
						visible,
						false))
				{
					objectEntry.state->nodes.obj->SetVisible(visible);
					a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);

					if (visible)
					{
						item->item->sharedCount--;
					}
					item.consume(candidates);
				}

				// Debug("%X: (%.8X) attached | %u ", a_actor->formID, item->formID,
				// slot);
			}
		}
	}

	bool Controller::GetVisibilitySwitch(
		Actor* a_actor,
		stl::flag<FlagsBase> a_flags,
		processParams_t& a_params)
	{
		if (a_flags.test(FlagsBase::kInvisible))
		{
			return false;
		}

		if (a_actor == *g_thePlayer &&
		    m_config.settings.data.toggleKeepLoaded &&
		    m_actorBlockList.playerToggle)
		{
			return false;
		}

		if (a_flags.test(FlagsBase::kHideIfUsingFurniture) &&
		    a_params.get_using_furniture())
		{
			return false;
		}

		if (a_flags.test(FlagsBase::kHideLayingDown) &&
		    a_params.get_laying_down())
		{
			return false;
		}

		/*if (a_flags.test(FlagsBase::kHideOnMount))
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
		Game::FormID a_actor,
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
		actorLookupResult_t& a_out)
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

	bool Controller::CustomEntryValidateInventoryForm(
		processParams_t& a_params,
		const Data::collectorData_t::itemData_t& a_itemData,
		const Data::configCustom_t& a_config,
		bool& a_hasMinCount)
	{
		if (a_itemData.count < 1)
		{
			return false;
		}

		if ((a_config.countRange.min && a_itemData.count < a_config.countRange.min) ||
		    a_config.countRange.max && a_itemData.count > a_config.countRange.max)
		{
			return false;
		}

		if (a_config.customFlags.test(CustomFlags::kEquipmentMode))
		{
			if (!a_config.customFlags.test(CustomFlags::kIgnoreRaceEquipTypes) &&
			    a_params.actor != *g_thePlayer)
			{
				auto equipmentFlag = ItemData::GetRaceEquipmentFlagFromType(a_itemData.type);
				if (!equipmentFlag || (a_params.race->validEquipTypes & equipmentFlag) != equipmentFlag)
				{
					return false;
				}
			}

			if (a_itemData.form->formType == TESAmmo::kTypeID)
			{
				a_hasMinCount = !a_itemData.is_equipped();
			}
			else
			{
				if (a_config.customFlags.test(CustomFlags::kDisableIfEquipped) &&
				    a_itemData.is_equipped())
				{
					a_hasMinCount = false;
				}
				else
				{
					std::int64_t delta = 0;

					if (a_itemData.equipped)
					{
						delta++;
					}

					if (a_itemData.equippedLeft)
					{
						delta++;
					}

					a_hasMinCount = a_itemData.sharedCount - delta > 0;
				}
			}

			if (!a_hasMinCount &&
			    (!m_config.settings.data.hideEquipped ||
			     a_config.customFlags.test(CustomFlags::kAlwaysUnload)))
			{
				return false;
			}
		}
		else
		{
			a_hasMinCount = true;
		}

		return true;
	}

	Data::collectorData_t::container_type::iterator Controller::CustomEntrySelectInventoryForm(
		processParams_t& a_params,
		const configCustom_t& a_config,
		bool& a_hasMinCount)
	{
		auto& formData = a_params.collector.m_data.forms;

		if (auto it = formData.find(a_config.form.get_id()); it != formData.end())
		{
			if (CustomEntryValidateInventoryForm(a_params, it->second, a_config, a_hasMinCount))
			{
				return it;
			}
		}

		for (auto& e : a_config.extraItems)
		{
			if (auto it = formData.find(e); it != formData.end())
			{
				if (CustomEntryValidateInventoryForm(a_params, it->second, a_config, a_hasMinCount))
				{
					return it;
				}
			}
		}

		return formData.end();
	}

	bool Controller::IsBlockedByChance(
		processParams_t& a_params,
		const Data::configCustom_t& a_config,
		objectEntryCustom_t& a_objectEntry)
	{
		if (a_config.customFlags.test(Data::CustomFlags::kUseChance))
		{
			if (!a_objectEntry.cflags.test(CustomObjectEntryFlags::kProcessedChance))
			{
				if (m_rng1.Get() > a_config.chance)
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
		processParams_t& a_params,
		const Data::configCustom_t& a_config,
		objectEntryCustom_t& a_objectEntry)
	{
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

		const configBaseValues_t* usedBaseConf =
			a_config.get_equipment_override(
				a_params.collector.m_data,
				a_params.objects.m_entriesSlot,
				a_params);

		if (!usedBaseConf)
		{
			usedBaseConf = std::addressof(a_config);
		}

		if (usedBaseConf->flags.test(FlagsBase::kDisabled))
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

		if (a_config.customFlags.test(CustomFlags::kIsInInventory))
		{
			bool hasMinCount;

			auto it = CustomEntrySelectInventoryForm(a_params, a_config, hasMinCount);
			if (it == a_params.collector.m_data.forms.end())
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
				usedBaseConf->flags.value,
				a_params);

			if (a_objectEntry.state && a_objectEntry.state->form == itemData.form)
			{
				if (a_config.customFlags.test(Data::CustomFlags::kUseGroup) ==
				    a_objectEntry.cflags.test(CustomObjectEntryFlags::kUseGroup))
				{
					bool _visible = hasMinCount && visible;

					if (ProcessItemUpdate(
							a_params,
							*usedBaseConf,
							std::addressof(a_config.group),
							usedBaseConf->targetNode,
							a_objectEntry,
							_visible))
					{
						if (_visible)
						{
							if (a_config.customFlags.test(CustomFlags::kEquipmentMode))
							{
								itemData.sharedCount--;
							}
						}

						return true;
					}
				}
			}

			if (!hasMinCount)
			{
				return false;
			}

			bool result;

			if (a_config.customFlags.test(Data::CustomFlags::kUseGroup))
			{
				result = LoadAndAttachGroup(
					a_params,
					*usedBaseConf,
					a_config.group,
					usedBaseConf->targetNode,
					a_objectEntry,
					itemData.form,
					a_config.customFlags.test(Data::CustomFlags::kLeftWeapon),
					visible);

				a_objectEntry.cflags.set(CustomObjectEntryFlags::kUseGroup);
			}
			else
			{
				auto modelForm = a_config.modelForm.get_id() ?
                                     a_config.modelForm.get_form() :
                                     itemData.form;

				a_objectEntry.modelForm = modelForm->formID;

				result = LoadAndAttach(
					a_params,
					*usedBaseConf,
					usedBaseConf->targetNode,
					a_objectEntry,
					itemData.form,
					modelForm,
					a_config.customFlags.test(CustomFlags::kLeftWeapon),
					false,
					visible,
					a_config.customFlags.test(CustomFlags::kDisableCollision));

				a_objectEntry.cflags.clear(CustomObjectEntryFlags::kUseGroup);
			}

			if (result)
			{
				a_objectEntry.SetNodeVisible(visible);

				if (a_config.customFlags.test(CustomFlags::kEquipmentMode) && visible)
				{
					itemData.sharedCount--;
				}

				a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
			}

			return result;
		}
		else
		{
			if (!a_config.form.get_id() ||
			    a_config.form.get_id().IsTemporary())
			{
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

				return false;
			}

			bool visible = GetVisibilitySwitch(
				a_params.actor,
				usedBaseConf->flags.value,
				a_params);

			if (a_objectEntry.state && a_objectEntry.state->form == form)
			{
				if (a_config.customFlags.test(Data::CustomFlags::kUseGroup) ==
				    a_objectEntry.cflags.test(CustomObjectEntryFlags::kUseGroup))
				{
					if (ProcessItemUpdate(
							a_params,
							*usedBaseConf,
							std::addressof(a_config.group),
							usedBaseConf->targetNode,
							a_objectEntry,
							visible))
					{
						return true;
					}
				}
			}

			a_objectEntry.modelForm = {};

			bool result;

			if (a_config.customFlags.test(Data::CustomFlags::kUseGroup))
			{
				result = LoadAndAttachGroup(
					a_params,
					*usedBaseConf,
					a_config.group,
					usedBaseConf->targetNode,
					a_objectEntry,
					form,
					a_config.customFlags.test(Data::CustomFlags::kLeftWeapon),
					visible);

				a_objectEntry.cflags.set(CustomObjectEntryFlags::kUseGroup);
			}
			else
			{
				result = LoadAndAttach(
					a_params,
					*usedBaseConf,
					usedBaseConf->targetNode,
					a_objectEntry,
					form,
					nullptr,
					a_config.customFlags.test(CustomFlags::kLeftWeapon),
					false,
					visible,
					a_config.customFlags.test(CustomFlags::kDisableCollision));

				a_objectEntry.cflags.clear(CustomObjectEntryFlags::kUseGroup);
			}

			if (result)
			{
				a_objectEntry.SetNodeVisible(visible);

				a_params.state.flags.set(ProcessStateUpdateFlags::kMenuUpdate);
			}

			return result;
		}
	}

	void Controller::ProcessCustomEntryMap(
		processParams_t& a_params,
		const Data::configCustomHolder_t& a_confData,
		ActorObjectHolder::customEntryMap_t& a_entryMap)
	{
		for (auto& f : a_confData.data)
		{
			auto& conf = f.second(a_params.configSex);

			auto it = a_entryMap.try_emplace(f.first).first;

			if (!ProcessCustomEntry(
					a_params,
					conf,
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
		processParams_t& a_params,
		const configCustomPluginMap_t& a_confPluginMap,
		ConfigClass a_class)
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

		if (auto it = actorConfig.find(a_params.actor->formID); it != actorConfig.end())
		{
			ProcessCustomMap(a_params, it->second, ConfigClass::Actor);
		}

		auto& npcConfig = cstore.GetNPCData();

		if (auto it = npcConfig.find(a_params.npc->formID); it != npcConfig.end())
		{
			ProcessCustomMap(a_params, it->second, ConfigClass::NPC);
		}

		auto& raceConfig = cstore.GetRaceData();

		if (auto it = raceConfig.find(a_params.race->formID); it != raceConfig.end())
		{
			ProcessCustomMap(a_params, it->second, ConfigClass::Race);
		}

		auto& globalConfig = cstore.GetGlobalData()[0];

		ProcessCustomMap(a_params, globalConfig, ConfigClass::Global);
	}

	void Controller::EvaluateImpl(
		NiNode* a_root,
		NiNode* a_npcroot,
		Actor* a_actor,
		Game::ObjectRefHandle a_handle,
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

		if (!a_actor->GetBiped(false))
		{
			return;
		}

		auto& objects = GetObjectHolder(
			a_actor,
			a_root,
			a_npcroot,
			*this,
			a_handle,
			m_nodeOverrideEnabled,
			m_nodeOverridePlayerEnabled,
			m_storedActorStates);

		/*__debugbreak();
		_DMESSAGE("%X %p", a_actor->formID, a_actor->GetNiRootNode(false));*/

		if (a_handle != objects.GetHandle())
		{
			Warning(
				"%s [%u]: %.8X: handle mismatch (%u != %u)",
				__FUNCTION__,
				__LINE__,
				a_actor->formID.get(),
				a_handle.get(),
				objects.GetHandle().get());

			RemoveActorImpl(a_actor, a_handle, ControllerUpdateFlags::kNone);

			return;
		}

		if (objects.m_root != a_root)
		{
			Warning(
				"%s [%u]: %.8X: skeleton root mismatch",
				__FUNCTION__,
				__LINE__,
				a_actor->formID.get());

			if (RemoveActorImpl(a_actor, a_handle, ControllerUpdateFlags::kNone))
			{
				auto& objs = GetObjectHolder(
					a_actor,
					a_root,
					a_npcroot,
					*this,
					a_handle,
					m_nodeOverrideEnabled,
					m_nodeOverridePlayerEnabled,
					m_storedActorStates);

				EvaluateImpl(a_root, a_npcroot, a_actor, a_handle, objs, a_flags);
			}
		}
		else
		{
			EvaluateImpl(a_root, a_npcroot, a_actor, a_handle, objects, a_flags);
		}
	}

	void Controller::EvaluateImpl(
		NiNode* a_root,
		NiNode* a_npcroot,
		Actor* a_actor,
		Game::ObjectRefHandle a_handle,
		ActorObjectHolder& a_objects,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		/*PerfTimer pt;
		pt.Start();*/

		if (!IsActorBlockedImpl(a_actor->formID))
		{
			npcRacePair_t nrp;

			if (GetNPCRacePair(a_actor, nrp))
			{
				processParams_t params{
					a_root,
					a_npcroot,
					a_handle,
					a_objects,
					GetEntryDataList(a_actor),
					nrp.npc->GetSex() == 1 ?
                        ConfigSex::Female :
                        ConfigSex::Male,
					a_flags,
					{ a_actor },
					a_actor,
					nrp.npc,
					nrp.race
				};

				if (!params.dataList)
				{
					Warning(
						"%s [%u]: %.8X: missing container object list",
						__FUNCTION__,
						__LINE__,
						a_actor->formID.get());
				}

				params.collector.Run(
					nrp.npc->container,
					params.dataList);

				if (!m_config.settings.data.disableNPCSlots ||
				    a_actor == *g_thePlayer)
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
						UpdateRootInMenu(a_root);
					}
				}
			}
		}
		else
		{
			RemoveActorGear(a_actor, a_handle, a_objects, a_flags);
		}

		a_objects.RequestTransformUpdateDefer();

		//Debug("%X : %f", a_actor->formID.get(), pt.Stop());
	}

	void Controller::EvaluateImpl(
		ActorObjectHolder& a_objects,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto handle = a_objects.GetHandle();

		NiPointer<TESObjectREFR> refr;
		if (!handle.Lookup(refr))
		{
			Warning(
				"%s [%u]: %.8X: could not lookup by handle (%u)",
				__FUNCTION__,
				__LINE__,
				a_objects.m_formid,
				handle.get());

			return;
		}

		if (refr->formID != a_objects.m_formid)
		{
			Warning(
				"%s [%u]: form id mismatch (%.8X != %.8X, %hhu)",
				__FUNCTION__,
				__LINE__,
				refr->formID,
				a_objects.m_formid,
				refr->formType);
		}

		auto actor = refr->As<Actor>();
		if (!actor)
		{
			Warning(
				"%s [%u]: %.8X: not an actor (%u, %hhu)",
				__FUNCTION__,
				__LINE__,
				refr->formID.get(),
				handle.get(),
				refr->formType);

			return;
		}

		if (!IsREFRValid(refr))
		{
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
		const ActorObjectHolder& a_objects)
	{
		Controller::actorInfo_t info;
		if (LookupCachedActorInfo(a_objects, info))
		{
			if (ProcessTransformsImpl(
					info.npcRoot,
					info.actor,
					info.npc,
					info.race,
					info.sex,
					a_objects,
					nullptr))
			{
				UpdateRootInMenu(info.root);
			}
		}
	}

	bool Controller::ProcessTransformsImpl(
		NiNode* a_npcRoot,
		Actor* a_actor,
		TESNPC* a_npc,
		TESRace* a_race,
		ConfigSex a_sex,
		const ActorObjectHolder& a_objects,
		const collectorData_t::container_type* a_equippedForms)
	{
		/*PerfTimer pt;
		pt.Start();*/

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
			a_objects,
			*this,
			a_actor,
			a_npc,
			a_race
		};

		configStoreNodeOverride_t::holderCache_t hc;

		for (auto& e : a_objects.m_weapNodes)
		{
			auto r = m_config.active.transforms.GetActorPlacement(
				a_actor->formID,
				a_npc->formID,
				a_race->formID,
				e.nodeName,
				hc);

			if (r)
			{
				ApplyNodePlacement(r->get(a_sex), e, params);
			}
			else
			{
				ResetNodePlacement(e);
			}
		}

		for (auto& e : a_objects.m_cmeNodes)
		{
			auto r = m_config.active.transforms.GetActorTransform(
				a_actor->formID,
				a_npc->formID,
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
			else
			{
				ResetNodeOverride(e.second);
			}
		}

		for (auto& e : a_objects.m_cmeNodes)
		{
			if (e.second.cachedConfCME)
			{
				ApplyNodeOverride(
					e.second,
					e.second.cachedConfCME->get(a_sex),
					params);
			}
		}

		//Debug("%X : %f", a_actor->formID.get(), pt.Stop());

		return true;
	}

	void Controller::ActorResetImpl(
		Actor* a_actor,
		Game::ObjectRefHandle a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		bool eraseState = false;

		if (a_actor != *g_thePlayer)
		{
			if (auto it = m_objects.find(a_actor->formID); it != m_objects.end())
			{
				m_storedActorStates.data.insert_or_assign(a_actor->formID, it->second).first;
				eraseState = true;
			}
		}

		RemoveActorImpl(a_actor, a_handle, a_flags);
		EvaluateImpl(a_actor, a_handle, a_flags);

		if (eraseState)
		{
			m_storedActorStates.data.erase(a_actor->formID);
		}
	}

	void Controller::ActorResetImpl(
		Actor* a_actor,
		Game::ObjectRefHandle a_handle,
		stl::flag<ControllerUpdateFlags> a_flags,
		ObjectSlot a_slot)
	{
		/*if (IsActorBlockedImpl(a_actor->formID))
		{
			RemoveActorImpl(a_actor, a_handle, a_flags);
			return;
		}*/

		if (a_slot == ObjectSlot::kMax)
		{
			return;
		}

		auto it = m_objects.find(a_actor->formID);
		if (it == m_objects.end())
		{
			return;
		}

		RemoveObject(a_actor, a_handle, it->second.GetSlot(a_slot), it->second, a_flags);
		EvaluateImpl(a_actor, a_handle, it->second, a_flags);
	}

	void Controller::ResetCustomImpl(
		Actor* a_actor,
		Game::ObjectRefHandle a_handle,
		Data::ConfigClass a_class,
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

		EvaluateImpl(a_actor, a_handle, ControllerUpdateFlags::kNone);
	}

	void Controller::ResetCustomImpl(
		Actor* a_actor,
		Game::ObjectRefHandle a_handle,
		Data::ConfigClass a_class,
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

		EvaluateImpl(a_actor, a_handle, ControllerUpdateFlags::kNone);
	}

	void Controller::ResetCustomImpl(
		Actor* a_actor,
		Game::ObjectRefHandle a_handle,
		Data::ConfigClass a_class)
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

		EvaluateImpl(a_actor, a_handle, ControllerUpdateFlags::kNone);
	}

	void Controller::UpdateTransformSlotImpl(
		Game::FormID a_actor,
		ObjectSlot a_slot)
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			UpdateTransformSlotImpl(it->second, a_slot);
		}
	}

	void Controller::UpdateTransformSlotImpl(
		ActorObjectHolder& a_record,
		ObjectSlot a_slot)
	{
		actorInfo_t info;
		if (!LookupCachedActorInfo(a_record, info))
		{
			return;
		}

		if (a_slot != ObjectSlot::kMax)
		{
			Data::configStoreSlot_t::holderCache_t hc;

			auto config = m_config.active.slot.GetActor(
				info.actor->formID,
				info.npc->formID,
				info.race->formID,
				a_slot,
				hc);

			auto& objectEntry = a_record.GetSlot(a_slot);
			if (!objectEntry.state)
			{
				return;
			}

			if (config)
			{
				auto& conf = GetConfigForActor(
					info.actor,
					info.race,
					config->get(info.sex),
					objectEntry);

				objectEntry.state->transform.Update(conf);

				UpdateObjectTransform(
					objectEntry.state->transform,
					objectEntry.state->nodes.obj,
					objectEntry.state->nodes.ref);

				UpdateRootInMenu(info.root);
			}
		}
	}

	void Controller::UpdateRootInMenu(NiNode* a_root)
	{
		auto mm = MenuManager::GetSingleton();

		if (mm && mm->InPausedMenu())
		{
			auto sh = UIStringHolder::GetSingleton();

			if (mm->IsMenuOpen(
					sh->GetString(UIStringHolder::STRING_INDICES::kinventoryMenu)) ||
			    mm->IsMenuOpen(
					sh->GetString(UIStringHolder::STRING_INDICES::kcontainerMenu)) ||
			    mm->IsMenuOpen(
					sh->GetString(UIStringHolder::STRING_INDICES::kfavoritesMenu)) ||
			    mm->IsMenuOpen(
					sh->GetString(UIStringHolder::STRING_INDICES::kbarterMenu)) ||
			    mm->IsMenuOpen(
					sh->GetString(UIStringHolder::STRING_INDICES::kgiftMenu)) ||
			    mm->IsMenuOpen(
					sh->GetString(UIStringHolder::STRING_INDICES::kmagicMenu)) ||
			    mm->IsMenuOpen(
					sh->GetString(UIStringHolder::STRING_INDICES::kconsole)))
			{
				EngineExtensions::UpdateRoot(a_root);
			}
		}
	}

	auto Controller::MakeTransformUpdateFunc()
		-> updateActionFunc_t
	{
		return {
			[this](
				actorInfo_t& a_info,
				const Data::configCustomEntry_t& a_confEntry,
				objectEntryCustom_t& a_entry) {
				if (!a_entry.state)
				{
					return false;
				}
				else
				{
					auto& conf = GetConfigForActor(
						a_info.actor,
						a_info.race,
						a_confEntry(a_info.sex),
						a_info.objects->GetSlots());

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

	auto Controller::MakeAttachUpdateFunc()
		-> updateActionFunc_t
	{
		return { [this](
					 const actorInfo_t& a_info,
					 const Data::configCustomEntry_t& a_confEntry,
					 objectEntryCustom_t& a_entry) {
					if (!a_entry.state)
					{
						return false;
					}
					else
					{
						auto& configEntry = a_confEntry(a_info.sex);

						auto& conf = GetConfigForActor(
							a_info.actor,
							a_info.race,
							configEntry,
							a_info.objects->GetSlots());

						AttachNodeImpl(
							a_info,
							a_info.npcRoot,
							conf.targetNode,
							conf.flags.test(Data::FlagsBase::kReferenceMode),
							a_entry);

						return true;
					}
				},
			     true };
	}

	const Data::configBaseValues_t& Controller::GetConfigForActor(
		Actor* a_actor,
		TESRace* a_race,
		const Data::configCustom_t& a_config,
		const ActorObjectHolder::slot_container_type& a_slots)
	{
		if (auto entryList = GetEntryDataList(a_actor))
		{
			if (auto npc = Game::GetActorBase(a_actor))
			{
				ItemCandidateCollector collector(a_actor);
				collector.Run(npc->container, entryList);

				CommonParams params{ a_actor, npc, a_race };

				if (auto eo = a_config.get_equipment_override(collector.m_data, a_slots, params))
				{
					return *eo;
				}
			}
		}

		return a_config;
	}

	const Data::configBaseValues_t& IED::Controller::GetConfigForActor(
		Actor* a_actor,
		TESRace* a_race,
		const Data::configSlot_t& a_config,
		const objectEntrySlot_t& a_entry)
	{
		if (auto entryList = GetEntryDataList(a_actor))
		{
			if (auto npc = Game::GetActorBase(a_actor))
			{
				ItemCandidateCollector collector(a_actor);
				collector.Run(npc->container, entryList);

				auto form = a_entry.GetFormIfActive();

				CommonParams params{ a_actor, npc, a_race };

				if (auto eo = !form ?
                                  a_config.get_equipment_override(collector.m_data, params) :
                                  a_config.get_equipment_override(
									  collector.m_data,
									  { form, ItemData::SlotToExtraSlot(a_entry.slotid) },
									  params))
				{
					return *eo;
				}
			}
		}

		return a_config;
	}

	void Controller::UpdateCustomImpl(
		Game::FormID a_actor,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey,
		updateActionFunc_t a_func)
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			UpdateCustomImpl(it->second, a_class, a_pkey, a_vkey, a_func);
		}
	}

	void IED::Controller::UpdateCustomNPCImpl(
		Game::FormID a_npc,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey,
		updateActionFunc_t a_func)
	{
		for (auto& e : m_objects)
		{
			if (e.second.IsActorNPC(a_npc))
			{
				UpdateCustomImpl(e.second, a_class, a_pkey, a_vkey, a_func);
			}
		}
	}

	void IED::Controller::UpdateCustomRaceImpl(
		Game::FormID a_race,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey,
		updateActionFunc_t a_func)
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
		Game::FormID a_actor,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		updateActionFunc_t a_func)
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			UpdateCustomImpl(it->second, a_class, a_pkey, a_func);
		}
	}

	void IED::Controller::UpdateCustomNPCImpl(
		Game::FormID a_npc,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		updateActionFunc_t a_func)
	{
		for (auto& e : m_objects)
		{
			if (e.second.IsActorNPC(a_npc))
			{
				UpdateCustomImpl(e.second, a_class, a_pkey, a_func);
			}
		}
	}

	void IED::Controller::UpdateCustomRaceImpl(
		Game::FormID a_race,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		updateActionFunc_t a_func)
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
		Game::FormID a_actor,
		Data::ConfigClass a_class,
		updateActionFunc_t a_func)
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			UpdateCustomImpl(it->second, a_class, a_func);
		}
	}

	void IED::Controller::UpdateCustomNPCImpl(
		Game::FormID a_npc,
		Data::ConfigClass a_class,
		updateActionFunc_t a_func)
	{
		for (auto& e : m_objects)
		{
			if (e.second.IsActorNPC(a_npc))
			{
				UpdateCustomImpl(e.second, a_class, a_func);
			}
		}
	}

	void IED::Controller::UpdateCustomRaceImpl(
		Game::FormID a_race,
		Data::ConfigClass a_class,
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
		ActorObjectHolder& a_record,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey,
		const updateActionFunc_t& a_func)
	{
		actorInfo_t info;
		if (!LookupCachedActorInfo(a_record, info))
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
				auto it = cfgdata.find(info.actor->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomImpl(
						info,
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
				auto it = cfgdata.find(info.npc->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomImpl(
						info,
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
				auto it = cfgdata.find(info.race->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomImpl(
						info,
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
				info,
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
				info.root,
				info.npcRoot,
				info.actor,
				info.handle,
				a_record,
				ControllerUpdateFlags::kNone);
		}

		//auto it = m_configStore.cfg.custom.GetActorData()
	}

	void Controller::UpdateCustomImpl(
		ActorObjectHolder& a_record,
		Data::ConfigClass a_class,
		const stl::fixed_string& a_pkey,
		const updateActionFunc_t& a_func)
	{
		actorInfo_t info;
		if (!LookupCachedActorInfo(a_record, info))
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
				auto it = cfgdata.find(info.actor->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(info, it->second, data, a_pkey, a_func);
				}
			}
			break;
		case ConfigClass::NPC:
			{
				auto& cfgdata = conf.GetNPCData();
				auto it = cfgdata.find(info.npc->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(info, it->second, data, a_pkey, a_func);
				}
			}
			break;
		case ConfigClass::Race:
			{
				auto& cfgdata = conf.GetRaceData();
				auto it = cfgdata.find(info.race->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(info, it->second, data, a_pkey, a_func);
				}
			}
			break;
		default:
			{
				UpdateCustomAllImpl(
					info,
					conf.GetGlobalData()[0],
					data,
					a_pkey,
					a_func);
			}
			break;
		}

		if (!a_func.clean)
		{
			EvaluateImpl(
				info.root,
				info.npcRoot,
				info.actor,
				info.handle,
				a_record,
				ControllerUpdateFlags::kNone);
		}
	}

	void Controller::UpdateCustomImpl(
		ActorObjectHolder& a_record,
		Data::ConfigClass a_class,
		const updateActionFunc_t& a_func)
	{
		actorInfo_t info;
		if (!LookupCachedActorInfo(a_record, info))
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
				auto it = cfgdata.find(info.actor->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(info, it->second, data, a_func);
				}
			}
			break;
		case ConfigClass::NPC:
			{
				auto& cfgdata = conf.GetNPCData();
				auto it = cfgdata.find(info.npc->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(info, it->second, data, a_func);
				}
			}
			break;
		case ConfigClass::Race:
			{
				auto& cfgdata = conf.GetRaceData();
				auto it = cfgdata.find(info.race->formID);
				if (it != cfgdata.end())
				{
					UpdateCustomAllImpl(info, it->second, data, a_func);
				}
			}
			break;
		default:
			{
				UpdateCustomAllImpl(
					info,
					conf.GetGlobalData()[0],
					data,
					a_func);
			}
			break;
		}

		if (!a_func.clean)
		{
			EvaluateImpl(
				info.root,
				info.npcRoot,
				info.actor,
				info.handle,
				a_record,
				ControllerUpdateFlags::kNone);
		}
	}

	void Controller::UpdateCustomImpl(
		actorInfo_t& a_info,
		const Data::configCustomPluginMap_t& a_confPluginMap,
		ActorObjectHolder::customPluginMap_t& a_pluginMap,
		const stl::fixed_string& a_pkey,
		const stl::fixed_string& a_vkey,
		const updateActionFunc_t& a_func)
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
		actorInfo_t& a_info,
		const Data::configCustomPluginMap_t& a_confPluginMap,
		ActorObjectHolder::customPluginMap_t& a_pluginMap,
		const stl::fixed_string& a_pkey,
		const updateActionFunc_t& a_func)
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
		bool ran = false;

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
		actorInfo_t& a_info,
		const Data::configCustomPluginMap_t& a_confPluginMap,
		ActorObjectHolder::customPluginMap_t& a_pluginMap,
		const updateActionFunc_t& a_func)
	{
		bool failed = false;
		bool ran = false;

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
		actorInfo_t& a_info,
		const Data::configCustomEntryMap_t& a_confEntryMap,
		ActorObjectHolder::customEntryMap_t& a_entryMap,
		const stl::fixed_string& a_vkey,
		const updateActionFunc_t& a_func)
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
		actorInfo_t& a_info,
		const Data::configCustom_t& a_configEntry,
		const Data::configTransform_t& a_xfrmConfigEntry,
		objectEntryCustom_t& a_entry)
	{
		if (!a_entry.state)
		{
			return;
		}

		a_entry.state->transform.Update(a_xfrmConfigEntry);

		UpdateObjectTransform(
			a_entry.state->transform,
			a_entry.state->nodes.obj,
			a_entry.state->nodes.ref);

		a_entry.state->UpdateGroupTransforms(a_configEntry.group);

		for (auto& e : a_entry.state->groupObjects)
		{
			UpdateObjectTransform(
				e.second.transform,
				e.second.object,
				nullptr);
		}

		UpdateRootInMenu(a_info.root);
	}

	void Controller::AttachSlotNodeImpl(
		Game::FormID a_actor,
		Data::ObjectSlot a_slot,
		bool a_evalIfNone)
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			AttachSlotNodeImpl(it->second, a_slot, a_evalIfNone);
		}
	}

	bool Controller::AttachSlotNodeImpl(
		ActorObjectHolder& a_record,
		Data::ObjectSlot a_slot,
		bool a_evalIfNone)
	{
		actorInfo_t info;
		if (!LookupCachedActorInfo(a_record, info))
		{
			return false;
		}

		if (IsActorBlockedImpl(info.actor->formID))
		{
			return false;
		}

		if (a_slot == ObjectSlot::kMax)
		{
			return false;
		}
		else
		{
			auto& objectEntry = a_record.GetSlot(a_slot);

			if (!objectEntry.state)
			{
				if (a_evalIfNone)
				{
					EvaluateImpl(
						info.root,
						info.npcRoot,
						info.actor,
						info.handle,
						a_record,
						ControllerUpdateFlags::kNone);

					return objectEntry.state.get() != nullptr;
				}
				else
				{
					return false;
				}
			}
			else
			{
				bool result = false;

				Data::configStoreSlot_t::holderCache_t hc;

				auto config = m_config.active.slot.GetActor(
					info.actor->formID,
					info.npc->formID,
					info.race->formID,
					a_slot,
					hc);

				if (config)
				{
					auto& configEntry = config->get(info.sex);
					auto& conf = GetConfigForActor(
						info.actor,
						info.race,
						configEntry,
						objectEntry);

					result = AttachNodeImpl(
						info,
						info.npcRoot,
						conf.targetNode,
						conf.flags.test(Data::FlagsBase::kReferenceMode),
						objectEntry);
				}

				return result;
			}
		}
	}

	bool Controller::AttachNodeImpl(
		const actorInfo_t& a_info,
		NiNode* a_root,
		const Data::NodeDescriptor& a_node,
		bool a_atmReference,
		objectEntryBase_t& a_entry)
	{
		if (!a_node)
		{
			return false;
		}

		bool result;

		if (result = AttachNodeImpl(
				a_root,
				a_node,
				a_atmReference,
				a_entry))
		{
			RequestEvaluateTransformsActor(a_info.actor->formID, false);
			UpdateRootInMenu(a_info.root);
		}
		else
		{
			Warning(
				"[%.8X] [race: %.8X] couldn't attach to node '%s'",
				a_info.actor->formID.get(),
				a_info.race->formID.get(),
				a_node.name.c_str());
		}

		return result;
	}

	bool Controller::AttachNodeImpl(
		NiNode* a_root,
		const Data::NodeDescriptor& a_node,
		bool a_atmReference,
		objectEntryBase_t& a_entry)
	{
		if (!a_entry.state)
		{
			return false;
		}

		bool result;

		if (result = AttachObjectToTargetNode(
				a_node,
				a_atmReference,
				a_root,
				a_entry.state->nodes.obj,
				a_entry.state->nodes.ref))
		{
			a_entry.state->nodeDesc = a_node;
			a_entry.state->atmReference = a_atmReference;

			a_entry.state->flags.clear(ObjectEntryFlags::kRefSyncDisableFailedOrphan);
		}

		return result;
	}

	bool Controller::LookupCachedActorInfo(
		const ActorObjectHolder& a_objects,
		actorInfo_t& a_out)
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

			return false;
		}

		if (!IsREFRValid(refr))
		{
			return false;
		}

		if (refr->formID != a_objects.m_formid)
		{
			Warning(
				"%s [%u]: form id mismatch (%.8X != %.8X, %hhu)",
				__FUNCTION__,
				__LINE__,
				refr->formID,
				a_objects.m_formid,
				refr->formType);
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

			return false;
		}

		auto npc = Game::GetActorBase(actor);

		auto race = actor->race;
		if (!race)
		{
			race = npc->race.race;

			if (!race)
			{
				return false;
			}
		}

		auto root = actor->GetNiRootNode(false);
		if (!root)
		{
			Warning(
				"%s: %.8X: actor has no 3D",
				__FUNCTION__,
				actor->formID.get());

			return false;
		}

		if (root != a_objects.m_root)
		{
			Warning(
				"%s: %.8X: skeleton root mismatch",
				__FUNCTION__,
				actor->formID.get());

			QueueReset(actor, ControllerUpdateFlags::kNone);

			return false;
		}

		auto npcroot = FindNode(root, m_bsstrings->m_npcroot);
		if (!npcroot)
		{
			return false;
		}

		a_out.actor = actor;
		a_out.handle = handle;
		a_out.npc = npc;
		a_out.race = race;
		a_out.root = root;
		a_out.npcRoot = npcroot;
		a_out.sex = npc->GetSex() == 1 ?
                        ConfigSex::Female :
                        ConfigSex::Male;
		a_out.objects = std::addressof(a_objects);

		return true;
	}

	void Controller::SaveLastEquippedItems(
		Actor* a_actor,
		const equippedItemInfo_t& a_info,
		ActorObjectHolder& a_cache)
	{
		auto ts = IPerfCounter::Query();

		if (a_info.rightSlot < ObjectSlot::kMax)
		{
			auto& slot = a_cache.GetSlot(a_info.rightSlot);

			slot.slotState.lastEquipped = a_info.right->formID;
			slot.slotState.lastSeenEquipped = ts;
		}

		if (a_info.leftSlot < ObjectSlot::kMax)
		{
			auto& slot = a_cache.GetSlot(a_info.leftSlot);

			slot.slotState.lastEquipped = a_info.left->formID;
			slot.slotState.lastSeenEquipped = ts;
		}
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
				QueueEvaluate(actor, ControllerUpdateFlags::kNone);
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
			QueueEvaluate(a_evn->reference, ControllerUpdateFlags::kNone);
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
					if (form->formType == SpellItem::kTypeID ||
					    IFormCommon::IsEquippableForm(form))
					{
						QueueRequestEvaluate(a_evn->actor->formID, false);
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
					if (auto oldContainer = a_evn->oldContainer.As<Actor>())
					{
						QueueRequestEvaluate(oldContainer->formID, true);
					}

					if (a_evn->oldContainer != a_evn->newContainer)  // ?
					{
						if (auto newContainer = a_evn->newContainer.As<Actor>())
						{
							QueueRequestEvaluate(newContainer->formID, true);
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
			QueueEvaluate(a_evn->actor, ControllerUpdateFlags::kAll);
		}

		return EventResult::kContinue;
	}

	auto Controller::ReceiveEvent(
		const TESDeathEvent* a_evn,
		BSTEventSource<TESDeathEvent>*)
		-> EventResult
	{
		if (a_evn &&
		    a_evn->source &&
		    a_evn->source->IsActor())
		{
			ITaskPool::QueueActorTask(
				a_evn->source,
				[this](Actor* a_actor, Game::ActorHandle a_handle) {
					IScopedLock lock(m_lock);

					auto it = m_objects.find(a_actor->formID);
					if (it != m_objects.end())
					{
						if (RemoveInvisibleObjects(
								it->second,
								static_cast<Game::ObjectRefHandle>(a_handle)))
						{
							EvaluateImpl(it->second, ControllerUpdateFlags::kNone);
						}
					}
				});

			if (a_evn->dead)
			{
				QueueRequestEvaluateTransformsActor(a_evn->source->formID, true);
			}
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
				QueueEvaluate(*g_thePlayer, ControllerUpdateFlags::kAll);
			}
		}

		return EventResult::kContinue;
	}

	EventResult IED::Controller::ReceiveEvent(
		const TESQuestStartStopEvent* a_evn,
		BSTEventSource<TESQuestStartStopEvent>* a_dispatcher)
	{
		return EventResult::kContinue;
	}

	bool Controller::GetNPCRacePair(
		Actor* a_actor,
		npcRacePair_t& a_out) noexcept
	{
		auto actorBase = a_actor->baseForm;
		if (!actorBase)
		{
			return false;
		}

		auto npc = actorBase->As<TESNPC>();
		if (!npc)
		{
			return false;
		}

		auto race = a_actor->race;
		if (!race)
		{
			race = npc->race.race;

			if (!race)
			{
				return false;
			}
		}

		a_out = { npc, race };

		return true;
	}

	bool Controller::SaveCurrentConfigAsDefault(
		stl::flag<ConfigStoreSerializationFlags> a_flags)
	{
		auto tmp = CreateExportData(m_config.active, a_flags);

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
		IScopedLock lock(m_lock);

		WriteRecord(
			a_intfc,
			SKSE_SERIALIZATION_TYPE_ID,
			stl::underlying(SerializationVersion::kCurrentVersion));
	}

	void Controller::LoadGameHandler(SKSESerializationInterface* a_intfc)
	{
		IScopedLock lock(m_lock);

		std::uint32_t type, length, version;

		while (a_intfc->GetNextRecordInfo(
			std::addressof(type),
			std::addressof(version),
			std::addressof(length)))
		{
			switch (type)
			{
			case SKSE_SERIALIZATION_TYPE_ID:
				ReadRecord(a_intfc, type, version);
				break;
			default:
				Warning(
					"%s: unrecognized record type: '%.4s'",
					__FUNCTION__,
					std::addressof(type));
				break;
			}
		}
	}

	void Controller::RevertHandler(SKSESerializationInterface* a_intfc)
	{
		Debug("Reverting..");

		IScopedLock lock(m_lock);

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

		ClearPlayerState();
		ClearObjectsImpl();
	}

	std::size_t Controller::Store(
		boost::archive::binary_oarchive& a_out)
	{
		a_out << m_actorBlockList;

		if (m_forceDefaultConfig)
		{
			a_out << m_config.stash;
		}
		else
		{
			a_out << m_config.active;
		}

		a_out << actorStateHolder_t(*this);

		return 3;
	}

	std::size_t Controller::Load(
		SKSESerializationInterface* a_intfc,
		std::uint32_t a_version,
		boost::archive::binary_iarchive& a_in)
	{
		if (a_version > stl::underlying(SerializationVersion::kCurrentVersion))
		{
			throw std::exception("unsupported version");
		}

		actorBlockList_t blockList;
		configStore_t cfgStore;
		actorStateHolder_t actorState;

		a_in >> blockList;
		a_in >> cfgStore;
		a_in >> actorState;

		FillGlobalSlotConfig(cfgStore.slot);
		CleanConfigStore(cfgStore);
		CleanBlockList(blockList);

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

		return 3;
	}

	void Controller::FillGlobalSlotConfig(
		Data::configStoreSlot_t& a_data) const
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
				}
			}
		}
	}

	void Controller::SaveSettings()
	{
		IScopedLock lock(m_lock);
		m_config.settings.SaveIfDirty();
	}

	void Controller::OnUIOpen()
	{
		UpdateActorInfo(m_objects);
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
					IScopedLock lock(m_lock);

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
			IScopedLock lock(m_lock);
			UpdateActorInfo(m_objects, a_actor);
		});
	}

	void IED::Controller::QueueUpdateActorInfo(
		Game::FormID a_actor,
		std::function<void(bool)> a_callback)
	{
		ITaskPool::AddTask([this, a_actor, callback = std::move(a_callback)]() {
			IScopedLock lock(m_lock);
			callback(UpdateActorInfo(a_actor));
		});
	}

	void Controller::QueueUpdateNPCInfo(
		Game::FormID a_npc,
		std::function<void(bool)> a_callback)
	{
		ITaskPool::AddTask([this, a_npc, callback = std::move(a_callback)]() {
			IScopedLock lock(m_lock);
			callback(UpdateNPCInfo(a_npc));
		});
	}

	void Controller::StoreActiveHandles()
	{
		IScopedLock lock(m_lock);

		m_activeHandles.clear();

		for (auto& e : m_objects)
		{
			m_activeHandles.emplace_back(e.second.GetHandle());
		}
	}

	void Controller::EvaluateStoredHandles(ControllerUpdateFlags a_flags)
	{
		IScopedLock lock(m_lock);

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

	void Controller::QueueObjectDatabaseClear()
	{
		ITaskPool::AddTask([this]() {
			IScopedLock lock(m_lock);

			ClearObjectDatabase();
		});
	}

	void Controller::SetObjectDatabaseLevel(ObjectDatabaseLevel a_level)
	{
		IScopedLock lock(m_lock);

		auto o = GetODBLevel();

		if (a_level != o)
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
			IScopedLock lock(m_lock);

			SetLanguageImpl(a_lang);
		});
	}
}