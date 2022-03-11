#include "pch.h"

#include "Config.h"
#include "EngineExtensions.h"

#include "ConfigOverride.h"
#include "Controller/Controller.h"
#include "Controller/NodeOverrideData.h"
#include "FormHolder.h"
#include "GlobalProfileManager.h"
#include "LocaleData.h"
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
	Controller* g_controller{ nullptr };

	static bool s_loaded = false;

	static bool SetLocaleFromLang()
	{
		auto e = *g_iniSettingCollection;
		if (!e)
		{
			return false;
		}

		auto f = e->Get("sLanguage:General");
		if (!f)
		{
			return false;
		}

		if (f->GetType() != Setting::kType_String)
		{
			return false;
		}

		if (auto s = f->data.s)
		{
			LocaleData::GetSingleton()->SetFromLang(s);

			return true;
		}
		else
		{
			return false;
		}
	}

	static void SKSE_MessageHandler(SKSEMessagingInterface::Message* a_message)
	{
		switch (a_message->type)
		{
		case SKSEMessagingInterface::kMessage_InputLoaded:

			ASSERT(StringCache::IsInitialized());

			NodeOverrideData::Create();
			g_controller->InitializeBSFixedStringTable();

			ASSERT(Drivers::Input::SinkToInputDispatcher());
			ASSERT(g_controller->SinkEventsT1());

			break;
		case SKSEMessagingInterface::kMessage_DataLoaded:
			{
				LocaleData::CreateSingleton();
				if (!SetLocaleFromLang())
				{
					LocaleData::GetSingleton()->SetFromLang("english");
				}

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
		gLog.Debug("Loading INI..");

		auto config = std::make_shared<ConfigINI>(PLUGIN_INI_FILE_NOEXT);

		if (!config->IsLoaded())
		{
			gLog.Warning("Couldn't load '%s', using defaults", PLUGIN_INI_FILE);
		}

		if (!ITaskPool::ValidateMemory())
		{
			gLog.FatalError("ITaskPool: memory validation failed");
			return false;
		}

		gLog.SetLogLevel(config->m_logLevel);

		auto& skse = ISKSE::GetSingleton();

		gLog.Debug("Creating trampolines..");

		if (!skse.CreateTrampolines(a_skse))
		{
			return false;
		}

		ITaskPool::Install(
			ISKSE::GetBranchTrampoline(),
			ISKSE::GetLocalTrampoline());

		if (config->m_enableUI)
		{
			gLog.Debug("Initializing render interface..");

			if (Drivers::Render::Initialize())
			{
				gLog.Debug("Installing priority input hook..");

				Drivers::Input::InstallPriorityHook();

				gLog.Debug("Initializing UI driver..");

				Drivers::UI::Initialize();
				Drivers::UI::SetImGuiIni(PATHS::IMGUI_INI);

				gLog.Message("Graphical user interface enabled");

				if (config->m_dpiAwareness)
				{
					ImGui_ImplWin32_EnableDpiAwareness();
					gLog.Debug("Enabled process DPI awareness");
				}
			}
			else
			{
				config->m_enableUI = false;

				WinApi::MessageBoxErrorLog(
					PLUGIN_NAME,
					"Failed initializing render driver, UI disabled");
			}
		}

		if (!config->m_enableUI)
		{
			ISKSE::CloseBacklog();
		}

		gLog.Debug("Creating controller..");

		g_controller = new Controller(config);

		gLog.Debug("Sinking controller events..");

		g_controller->SinkEventsT0();
		g_controller->SinkInputEvents();

		gLog.Debug("Registering SKSE listeners..");

		auto handle = skse.GetPluginHandle();

		auto mi = skse.GetInterface<SKSEMessagingInterface>();

		ASSERT(mi != nullptr);

		mi->RegisterListener(handle, "SKSE", SKSE_MessageHandler);

		gLog.Debug("Registering papyrus functions..");

		auto pi = skse.GetInterface<SKSEPapyrusInterface>();

		ASSERT(pi != nullptr);

		pi->Register(Papyrus::Register);

		gLog.Debug("Setting serialization callbacks..");

		auto si = skse.GetInterface<SKSESerializationInterface>();

		ASSERT(si != nullptr);

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

		gLog.Debug("Installing engine extensions..");

		EngineExtensions::Install(
			g_controller,
			config);

		if (config->m_closeLogFile)
		{
			gLog.Debug("Closing log..");

			gLog.Close();
		}

		gLog.Debug("Initialization done");

		return true;
	}
}