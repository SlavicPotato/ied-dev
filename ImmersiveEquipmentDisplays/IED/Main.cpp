#include "pch.h"

#include "Main.h"

#include "AnimationUpdateController.h"
#include "AreaLightingDetection.h"
#include "ConfigINI.h"
#include "ConfigStore.h"
#include "Controller/Controller.h"
#include "EngineExtensions.h"
#include "FormHolder.h"
#include "GlobalProfileManager.h"
#include "LocaleData.h"
#include "Localization/LocalizationDataManager.h"
#include "NodeMap.h"
#include "NodeOverrideData.h"
#include "PapyrusInterface/Papyrus.h"
#include "ReferenceLightController.h"
#include "SPtrHolder.h"
#include "SkeletonCache.h"

#include "Drivers/Input.h"
#include "Drivers/Render.h"
#include "Drivers/UI.h"

#include <ext/SKSEMessagingHandler.h>
#include <ext/SKSESerializationEventHandler.h>

#if defined(IED_ENABLE_OUTFIT) && defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
#	include "IED/OM/PersistentOutfitFormManager.h"
#endif

namespace IED
{
	Initializer Initializer::m_Instance;

	bool Initializer::Run(const SKSEInterface* a_skse)
	{
		if (m_done)
		{
			return false;
		}

		RunChecks();

		Debug("Loading INI..");

		const auto& config = m_config = stl::make_smart<const ConfigINI>(PLUGIN_INI_FILE_NOEXT);

		if (!config->IsLoaded())
		{
			Warning("Couldn't load '%s', using defaults", PLUGIN_INI_FILE);
		}

		if (!ITaskPool::ValidateMemory())
		{
			FatalError("ITaskPool: memory validation failed");
			return false;
		}

		gLog.SetLogLevel(config->m_logLevel);

		auto& skse = ISKSE::GetSingleton();

		Debug("Creating trampolines..");

		if (!skse.CreateTrampolines(a_skse))
		{
			return false;
		}

		Debug("Installing task hooks..");

		ITaskPool::Install(
			ISKSE::GetBranchTrampoline(),
			ISKSE::GetLocalTrampoline(),
			config->m_effectShaderFlickerFix);

		Debug("Creating controller..");

		m_controller = new Controller(config);

		SetupSKSEEventHandlers(a_skse);

		SKSEMessagingHandler::GetSingleton().AddSink(this);

		Debug("Sinking controller events..");

		m_controller->SinkEventsT0();

		Debug("Installing engine extensions..");

		EngineExtensions::Install(m_controller, config);

#if defined(IED_ENABLE_OUTFIT) && defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
		OM::PersistentOutfitFormManager::GetSingleton().Install();
#endif

		bool uiEnabled = false;

		if (config->m_enableUI)
		{
			Debug("Initializing render driver..");

			if (Drivers::Render::Initialize(true))
			{
				Debug("Installing priority input hook..");

				Drivers::Input::InstallPriorityHook();

				Debug("Initializing UI driver..");

				Drivers::UI::Initialize();
				Drivers::UI::SetImGuiIni(PATHS::IMGUI_INI);

				uiEnabled = true;

				Message("Graphical user interface enabled");

				if (config->m_dpiAwareness)
				{
					ImGui_ImplWin32_EnableDpiAwareness();
					Debug("Enabled process DPI awareness");
				}
			}
			else
			{
				WinApi::MessageBoxErrorLog(
					PLUGIN_NAME_FULL,
					"Failed initializing render driver, UI disabled");
			}
		}

		Debug("Initializing controller input handlers..");

		m_controller->InitializeInputHandlers();

		Debug("Early initialization done");

		if (!uiEnabled)
		{
			ISKSE::GetSingleton().CloseBacklog();
		}

		if (config->m_closeLogFile)
		{
			Debug("Closing log..");

			gLog.Close();
		}

		if (config->m_enableLights)
		{
			auto& rlc = ReferenceLightController::GetSingleton();

			rlc.SetNPCLightCellAttachFixEnabled(config->m_lightNPCCellAttachFix);
			rlc.SetNPCEnableVanillaLightUpdates(config->m_lightNPCVanillaUpdates);
		}

		if (config->m_behaviorGraphAnims)
		{
			AnimationUpdateController::GetSingleton().Initialize();
		}

		ALD::SetInteriorAmbientLightThreshold(config->m_interiorAmbientLightThreshold);

		auto& sc = SkeletonCache::GetSingleton();

		sc.EnableMakeOnLoad(config->m_scMakeOnLoad);

		m_done = true;

		return true;
	}

	void Initializer::RunChecks()
	{
#if defined(_XM_AVX2_INTRINSICS_) || defined(BT_USE_AVX)
		if (!IsProcessorFeaturePresent(PF_AVX2_INSTRUCTIONS_AVAILABLE))
		{
			stl::report_and_fail(
				PLUGIN_NAME_FULL,
				"Plugin uses AVX2 instrinsics but the processor lacks support for the instruction set");
		}
#endif
	}

	const char* Initializer::GetLanguage()
	{
		const auto e = *g_iniSettingCollection;
		if (!e)
		{
			return nullptr;
		}

		const auto* const f = e->Get("sLanguage:General");
		if (!f)
		{
			return nullptr;
		}

		if (f->GetType() != Setting::kType_String)
		{
			return nullptr;
		}

		return f->data.s;
	}

	void Initializer::SetupSKSEEventHandlers(
		const SKSEInterface* a_skse)
	{
		const auto& skse = ISKSE::GetSingleton();

		Debug("Setting up SKSE message handler..");

		const auto handle = a_skse->GetPluginHandle();

		const auto messagingInterface = skse.GetInterface<SKSEMessagingInterface>();

		ASSERT(messagingInterface);

		SKSEMessagingHandler::GetSingleton().Setup(handle, messagingInterface);

		Debug("Registering papyrus functions..");

		const auto papyrusInterface = skse.GetInterface<SKSEPapyrusInterface>();

		ASSERT(papyrusInterface);

		papyrusInterface->Register(Papyrus::Register);

		Debug("Setting up SKSE serialization handler..");

		const auto serializationInterface = skse.GetInterface<SKSESerializationInterface>();

		ASSERT(serializationInterface);

		SKSESerializationEventHandler::GetSingleton().Setup(
			handle,
			SKSE_SERIALIZATION_UID,
			serializationInterface,
			true);
	}

	void Initializer::Receive(const SKSEMessagingEvent& a_evn)
	{
		switch (a_evn.message->type)
		{
		case SKSEMessagingInterface::kMessage_InputLoaded:

			ASSERT(StringCache::IsInitialized());

			BSStringHolder::Create();

			NodeOverrideData::Create();
			NodeOverrideData::LoadAndAddExtraNodes(PATHS::EXTRA_NODES);
			NodeOverrideData::LoadAndAddConvertNodes(PATHS::CONVERT_NODES);
			NodeOverrideData::LoadAndAddNodeMonitor(PATHS::NODE_MONITOR);
			NodeOverrideData::LoadAndAddAdditionalCMENodes(PATHS::ADDITIONAL_CME_NODES);

			ASSERT(Drivers::Input::SinkToInputDispatcher());

			if (m_config->m_enableLights)
			{
				ReferenceLightController::GetSingleton().Initialize();
			}

			m_config.reset();

			break;
		case SKSEMessagingInterface::kMessage_DataLoaded:
			{
				LocaleData::CreateSingleton();

				auto language = GetLanguage();

				if (language)
				{
					Debug("Detected language: '%s'", language);
				}
				else
				{
					Warning("Couldn't automatically detect language, using default locale");

					language = "english";
				}

				LocaleData::GetSingleton()->SetFromLang(language);

				ASSERT(Data::IData::PopulateRaceList());
				ASSERT(Data::IData::PopulatePluginInfo());

				FormHolder::Initialize();
				SPtrHolder::Initialize();

				auto& ldm = Localization::LocalizationDataManager::GetSingleton();
				if (!ldm.Load(PATHS::LOCALIZATION))
				{
					Error(
						"Exception occured while loading localization data: %s",
						ldm.GetLastException().what());
				}

				auto& nodeMap = IED::Data::NodeMap::GetSingleton();

				if (Serialization::FileExists(PATHS::NODEMAP))
				{
					if (!nodeMap.LoadExtra(PATHS::NODEMAP))
					{
						Error(
							"%s: %s",
							PATHS::NODEMAP,
							nodeMap.GetLastException().what());
					}
				}

				for (auto& e : NodeOverrideData::GetExtraMovNodes())
				{
					nodeMap.Add(
						e.names[1].first,
						e.desc,
						Data::NodeDescriptorFlags::kNone);
				}

				GlobalProfileManager::GetSingleton<SlotProfile>().Load(
					PATHS::PROFILE_MANAGER_SLOT);

				GlobalProfileManager::GetSingleton<CustomProfile>().Load(
					PATHS::PROFILE_MANAGER_CUSTOM);

				GlobalProfileManager::GetSingleton<NodeOverrideProfile>().Load(
					PATHS::PROFILE_MANAGER_EQUIPMENT_TRANSFORMS);

				GlobalProfileManager::GetSingleton<FormFilterProfile>().Load(
					PATHS::PROFILE_MANAGER_FORM_FILTER);

				GlobalProfileManager::GetSingleton<CondVarProfile>().Load(
					PATHS::PROFILE_MANAGER_VARIABLES);

#if defined(IED_ENABLE_I3DI)
				GlobalProfileManager::GetSingleton<ModelProfile>().Load(
					PATHS::PROFILE_MANAGER_MODEL);
#endif

#if defined(IED_ENABLE_OUTFIT)
				GlobalProfileManager::GetSingleton<OutfitProfile>().Load(
					PATHS::PROFILE_MANAGER_OUTFIT);

#	if defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
				GlobalProfileManager::GetSingleton<OutfitFormListProfile>().Load(
					PATHS::PROFILE_MANAGER_OUTFIT_FORM_LIST);
#	endif
#endif

				GlobalProfileManager::GetSingleton<KeybindProfile>().Load(
					PATHS::PROFILE_MANAGER_KEYBIND);

				const auto pluginInfo = Data::IData::GetPluginInfo().GetInfo();

				Debug(
					"Loaded plugins: %zu, light: %zu [%zu total]",
					pluginInfo.plugins,
					pluginInfo.light,
					pluginInfo.total);
			}
			break;
		}
	}

}