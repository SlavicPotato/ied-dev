#include "pch.h"

#include "Config.h"
#include "EngineExtensions.h"

#include "ConfigOverride.h"
#include "Controller/Controller.h"
#include "Controller/NodeOverrideData.h"
#include "FormHolder.h"
#include "GlobalProfileManager.h"
#include "Localization/LocalizationDataManager.h"
#include "Main.h"
#include "NodeMap.h"
#include "PapyrusInterface/Papyrus.h"

#include "Drivers/Input.h"
#include "Drivers/Render.h"
#include "Drivers/UI.h"

#include "UI/UIMain.h"

#include <ext/SKSEMessaging.h>

namespace IED
{
	std::shared_ptr<Controller> g_controller;

	static bool s_loaded = false;

	static void SKSE_MessageHandler(SKSEMessagingInterface::Message* a_message)
	{
		switch (a_message->type)
		{
		case SKSEMessagingInterface::kMessage_DataLoaded:
			{
				ASSERT(Data::IData::PopulateRaceList());
				ASSERT(Data::IData::PopulatePluginInfo());
				ASSERT(Data::IData::PopulateMiscInfo());

				FormHolder::Populate();

				auto& ldm = Localization::LocalizationDataManager::GetSingleton();
				if (!ldm.Load(PATHS::LOCALIZATION))
				{
					gLog.Error(
						"Exception occured while loading localization data: %s",
						ldm.GetLastException().what());
				}

				g_controller->InitializeData();

				GlobalProfileManager::GetSingleton<SlotProfile>().Load(
					PATHS::PROFILE_MANAGER_SLOT);

				GlobalProfileManager::GetSingleton<CustomProfile>().Load(
					PATHS::PROFILE_MANAGER_CUSTOM);

				GlobalProfileManager::GetSingleton<NodeOverrideProfile>().Load(
					PATHS::PROFILE_MANAGER_EQUIPMENT_TRANSFORMS);

				GlobalProfileManager::GetSingleton<FormFilterProfile>().Load(
					PATHS::PROFILE_MANAGER_FORM_FILTER);

				ASSERT(g_controller->SinkEventsT2());
			}
			break;
		case SKSEMessagingInterface::kMessage_InputLoaded:

			OverrideNodeInfo::Create();
			g_controller->InitializeStrings();

			ASSERT(Drivers::Input::SinkToInputDispatcher());

			g_controller->SinkEventsT1();

			break;
		case SKSEMessagingInterface::kMessage_PreLoadGame:
			g_controller->StoreActiveHandles();
			break;
		case SKSEMessagingInterface::kMessage_NewGame:
			break;
		case SKSEMessagingInterface::kMessage_PostLoadGame:
			g_controller->EvaluateStoredHandles(ControllerUpdateFlags::kNone);
			break;
		case SKSEMessagingInterface::kMessage_SaveGame:
			g_controller->SaveSettings();
			break;
		}
	}

	static void SaveGameHandler(SKSESerializationInterface* a_intfc)
	{
		g_controller->SaveGameHandler(a_intfc);
	}

	static void LoadGameHandler(SKSESerializationInterface* a_intfc)
	{
		g_controller->LoadGameHandler(a_intfc);
	}

	static void RevertHandler(SKSESerializationInterface* a_intfc)
	{
		g_controller->RevertHandler(a_intfc);
	}

	bool Initialize(const SKSEInterface* a_skse)
	{
		auto config = std::make_shared<ConfigINI>(PLUGIN_INI_FILE);

		if (!config->IsLoaded())
		{
			gLog.Warning("Couldn't load configuration file, using defaults");
		}

		if (!ITaskPool::ValidateMemory())
		{
			gLog.FatalError("ITaskPool: memory validation failed");
			return false;
		}

		gLog.SetLogLevel(config->m_logLevel);

		auto& skse = ISKSE::GetSingleton();

		if (!skse.CreateTrampolines(a_skse))
		{
			return false;
		}

		ITaskPool::Install(
			ISKSE::GetBranchTrampoline(),
			ISKSE::GetLocalTrampoline());

		if (config->m_enableUI)
		{
			if (Drivers::Render::Initialize())
			{
				Drivers::Input::InstallPriorityHook();
				Drivers::UI::Initialize();
				Drivers::UI::SetImGuiIni(PATHS::IMGUI_INI);

				gLog.Message("Graphical user interface enabled");

				if (config->m_dpiAwareness)
				{
					ImGui_ImplWin32_EnableDpiAwareness();
					gLog.Debug("Enabled DPI awareness");
				}
			}
			else
			{
				config->m_enableUI = false;
				gLog.Error("Failed initializing render driver, UI disabled");
			}
		}

		if (!config->m_enableUI)
		{
			ISKSE::CloseBacklog();
		}

		bool closeLog = config->m_closeLogFile;

		g_controller = std::make_shared<Controller>(config);
		g_controller->SinkEventsT0();

		auto handle = a_skse->GetPluginHandle();
		auto mi = skse.GetInterface<SKSEMessagingInterface>();

		mi->RegisterListener(handle, "SKSE", SKSE_MessageHandler);

		skse.GetInterface<SKSEPapyrusInterface>()->Register(Papyrus::Register);

		auto si = skse.GetInterface<SKSESerializationInterface>();

		si->SetUniqueID(handle, 'ADEI');
		si->SetRevertCallback(handle, RevertHandler);
		si->SetSaveCallback(handle, SaveGameHandler);
		si->SetLoadCallback(handle, LoadGameHandler);

		/*auto dispatcher = static_cast<EventDispatcher<SKSENiNodeUpdateEvent>*>(
			mi->GetEventDispatcher(SKSEMessagingInterface::kDispatcher_NiNodeUpdateEvent));

		if (dispatcher)
		{
			dispatcher->AddEventSink(g_controller.get());
		}*/

		EngineExtensions::Initialize(
			g_controller,
			config);

		if (closeLog)
		{
			gLog.Close();
		}

		return true;
	}
}  // namespace IED