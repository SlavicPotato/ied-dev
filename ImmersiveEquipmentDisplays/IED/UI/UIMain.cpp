#include "pch.h"

#include "UIMain.h"

#include "ConditionalVariables/Profile/UIProfileEditorConditionalVariables.h"
#include "ConditionalVariables/UIConditionalVariablesEditorWindow.h"
#include "Custom/Profile/UIProfileEditorCustom.h"
#include "EquipmentSlots/Profile/UIProfileEditorSlot.h"
#include "FormFilters/UIProfileEditorFormFilters.h"
#include "I3DI/I3DIMain.h"
#include "Keybind/Profile/UIProfileEditorKeybind.h"
#include "Keybind/UIKeyBindEditorWindow.h"
#include "NodeOverride/Profile/UIProfileEditorNodeOverride.h"
#include "NodeOverride/UINodeOverrideEditorWindow.h"
#include "Style/UIStyleEditorWindow.h"
#include "UIActorInfo.h"
#include "UIDialogImportExport.h"
#include "UIDisplayManagement.h"
#include "UIFormBrowser.h"
#include "UIGeneralInfo.h"
#include "UILog.h"
#include "UINodeMapEditor.h"
#include "UISettings.h"
#include "UISkeletonExplorer.h"
#include "UIStats.h"

#if defined(IED_ENABLE_OUTFIT)
#	include "IED/OM/UI/Profile/UIOutfitProfileEditor.h"
#	include "IED/OM/UI/UIOutfitEditorWindow.h"
#	if defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
#		include "IED/OM/UI/UIOutfitFormManager.h"
#	endif
#endif

#include "Custom/UICustomTabPanel.h"
#include "EquipmentSlots/UISlotTabPanel.h"

#include "Widgets/UIExportFilterWidget.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIMain::UIMain(
			Tasks::UIRenderTaskBase& a_owner,
			Controller&              a_controller) :
			UIKeyedInputLockReleaseHandler(a_owner),
			UIAboutModal(a_controller),
			m_childWindows
		{
			std::make_unique<UIFormBrowser>(a_controller),
				std::make_unique<UISettings>(a_owner, a_controller),
				std::make_unique<UIDialogImportExport>(a_controller),
				std::make_unique<UIProfileEditorSlot>(a_controller),
				std::make_unique<UIProfileEditorCustom>(a_controller),
				std::make_unique<UIProfileEditorNodeOverride>(a_controller),
				std::make_unique<UINodeMapEditor>(a_controller),
				std::make_unique<UINodeOverrideEditorWindow>(a_controller),
				std::make_unique<UIProfileEditorFormFilters>(a_controller),
				std::make_unique<UILog>(a_controller),
				std::make_unique<UIStats>(a_owner, a_controller),
				std::make_unique<UISkeletonExplorer>(a_controller),
				std::make_unique<UIActorInfo>(a_controller),
				std::make_unique<UIDisplayManagement>(a_controller),
				std::make_unique<UIConditionalVariablesEditorWindow>(a_controller),
				std::make_unique<UIProfileEditorConditionalVariables>(a_controller),
#if defined(IED_ENABLE_I3DI)
				a_controller.CPUHasSSE41() ?
					std::make_unique<I3DIMain>(a_controller) :
					nullptr
#else
				nullptr
#endif
				,
				std::make_unique<UIKeyBindEditorWindow>(a_controller),

#if defined(IED_ENABLE_OUTFIT)
#	if defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
				std::make_unique<OM::UIOutfitFormManager>(a_controller),
#	else
				nullptr,
#	endif
				std::make_unique<OM::UIOutfitEditorWindow>(a_controller),
				std::make_unique<OM::UIOutfitProfileEditor>(a_controller)
#else
				nullptr,
				nullptr,
				nullptr
#endif
					,
				std::make_unique<UIGeneralInfo>(a_controller),
				std::make_unique<UIProfileEditorKeybind>(a_controller),
				std::make_unique<UIStyleEditorWindow>(a_controller)
		}
		,
			m_formLookupCache(a_controller),
			m_owner(a_owner),
			m_controller(a_controller)
		{
#if defined(DEBUG)
			using enum_type = std::underlying_type_t<ChildWindowID>;

			for (enum_type i = 0; i < stl::underlying(ChildWindowID::kMax); i++)
			{
				if (const auto& window = m_childWindows[i])
				{
					assert(window->GetContextID() == i);
				}
			}
#endif
		}

		void UIMain::Initialize()
		{
			for (const auto& e : m_childWindows)
			{
				if (e)
				{
					e->Initialize();
					e->AddSink(this);
				}
			}

			const auto& settings = GetUISettings();

			using enum_type = std::underlying_type_t<ChildWindowID>;

			for (enum_type i = 0; i < stl::underlying(ChildWindowID::kMax); i++)
			{
				const auto& window = m_childWindows[i];

				if (!window)
				{
					continue;
				}

				if (static_cast<ChildWindowID>(i) != ChildWindowID::kUIFormBrowser)
				{
					window->SetOpenState(settings.windowOpenStates[i]);
				}
			}

			if (settings.releaseLockKeys)
			{
				ILRHSetKeys(
					settings.releaseLockKeys->key,
					settings.releaseLockKeys->comboKey);
			}

			ILRHSetLockedAlpha(settings.releaseLockAlpha);
			ILRHSetUnfreezeTime(settings.releaseLockUnfreezeTime);
		}

		void UIMain::Reset()
		{
			for (const auto& e : m_childWindows)
			{
				if (e)
				{
					e->Reset();
				}
			}
		}

		void UIMain::Draw()
		{
			ILRHBegin();

			DrawMenuBarMain();
			DrawChildWindows();

			if (auto window = GetChild<UIProfileEditorFormFilters>())
			{
				if (window->ChangedConfig())
				{
					if (auto dm = GetChildContext<UIDisplayManagement>())
					{
						dm->Notify(1);
					}
				}
			}

			m_popupQueue.run();

			ILRHEnd();

			m_formLookupCache.RunCleanup();
		}

		void UIMain::PrepareGameData()
		{
			for (auto& e : m_childWindows)
			{
				if (e && e->IsContextOpen())
				{
					e->PrepareGameData();
				}
			}
		}

		void UIMain::Render()
		{
			for (auto& e : m_childWindows)
			{
				if (e && e->IsContextOpen())
				{
					e->Render();
				}
			}
		}

		Data::SettingHolder::UserInterface& UIMain::GetUISettings() const noexcept
		{
			return m_controller.GetSettings().data.ui;
		}

		UIFormBrowser& UIMain::GetFormBrowser() noexcept
		{
			auto result = GetChild<UIFormBrowser>();
			assert(result);
			return *result;
		}

		void UIMain::Receive(const UIContextStateChangeEvent& a_evn)
		{
			const auto id = static_cast<ChildWindowID>(a_evn.context.GetContextID());

			if (id >= ChildWindowID::kMax)
			{
				return;
			}

			if (!a_evn.newState)
			{
				if (
					GetUISettings().exitOnLastWindowClose &&
					m_seenOpenChildThisSession &&
					!HasOpenChild())
				{
					m_lastClosedChild = id;

					SetOpenState(false);
				}
			}
			else
			{
				m_seenOpenChildThisSession = true;
			}
		}

		void UIMain::DrawChildWindows()
		{
			ImGui::PushID("cw");

			for (auto& e : m_childWindows)
			{
				if (e)
				{
					e->DrawWrapper();
				}
			}

			ImGui::PopID();
		}

		void UIMain::DrawMenuBarMain()
		{
			ImGui::PushID("menu_bar_main");

			if (ImGui::BeginMainMenuBar())
			{
				DrawMenuBarContents();

				ImGui::EndMainMenuBar();
			}

			ImGui::PopID();
		}

		void UIMain::DrawMenuBarContents()
		{
			if (UIL::LCG_BM(CommonStrings::File, "1"))
			{
				DrawFileMenu();

				ImGui::EndMenu();
			}

			if (UIL::LCG_BM(CommonStrings::View, "2"))
			{
				DrawViewMenu();

				ImGui::EndMenu();
			}

			if (UIL::LCG_BM(CommonStrings::Tools, "3"))
			{
				DrawToolsMenu();

				ImGui::EndMenu();
			}

			if (UIL::LCG_BM(CommonStrings::Help, "4"))
			{
				DrawHelpMenu();

				ImGui::EndMenu();
			}

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

			if (UIL::LCG_BM(CommonStrings::Actions, "X"))
			{
				DrawActionsMenu();

				ImGui::EndMenu();
			}
		}

		void UIMain::DrawFileMenu()
		{
			const bool enabled = !m_controller.IsDefaultConfigForced();

			DrawContextMenuItem<UIDialogImportExport>(UIMainStrings::ImportExport, "1", enabled);

			ImGui::Separator();

			if (ImGui::BeginMenu(UIL::LS(UIMainStrings::DefaultConfig, "2"), enabled))
			{
				DrawDefaultConfigSubmenu();

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (UIL::LCG_MI(CommonStrings::Exit, "3"))
			{
				SetOpenState(false);
			}
		}

		void UIMain::DrawViewMenu()
		{
			DrawContextMenuItem<UIDisplayManagement>(UIMainStrings::DisplayManagement, "1");
			DrawContextMenuItem<UINodeOverrideEditorWindow>(UIMainStrings::GearPositioning, "2");

			ImGui::Separator();

			DrawContextMenuItem<UIConditionalVariablesEditorWindow>(UIMainStrings::ConditionalVariables, "3");
			DrawContextMenuItem<UIKeyBindEditorWindow>(UIMainStrings::KeyBinds, "4");

#if defined(IED_ENABLE_OUTFIT)
			DrawContextMenuItem<OM::UIOutfitEditorWindow>(UIWidgetCommonStrings::OutfitConfig, "5");
#endif

			ImGui::Separator();

			DrawContextMenuItem<I3DIMain>(UIMainStrings::I3DI, "6");
		}

		void UIMain::DrawProfileEditorsSubmenu()
		{
			DrawContextMenuItem<UIProfileEditorSlot>(CommonStrings::Equipment, "1");
			DrawContextMenuItem<UIProfileEditorCustom>(CommonStrings::Custom, "2");
			DrawContextMenuItem<UIProfileEditorNodeOverride>(UIMainStrings::GearPositioning, "3");
			DrawContextMenuItem<UIProfileEditorFormFilters>(UIMainStrings::FormFilters, "4");
			DrawContextMenuItem<UIProfileEditorConditionalVariables>(UIMainStrings::ConditionalVariables, "5");
#if defined(IED_ENABLE_OUTFIT)
			DrawContextMenuItem<OM::UIOutfitProfileEditor>(UIWidgetCommonStrings::OutfitConfig, "6");
#endif
			DrawContextMenuItem<UIProfileEditorKeybind>(UIMainStrings::KeyBinds, "7");
		}

		void UIMain::DrawDiagnosticsSubmenu()
		{
			DrawContextMenuItem<UIGeneralInfo>(UIWidgetCommonStrings::GeneralInfo, "1");
			DrawContextMenuItem<UIActorInfo>(UIWidgetCommonStrings::ActorInfo, "2");
			DrawContextMenuItem<UISkeletonExplorer>(UIWidgetCommonStrings::SkeletonExplorer, "3");
		}

		void UIMain::DrawToolsMenu()
		{
			if (UIL::LCG_BM(UIMainStrings::ProfileEditors, "1"))
			{
				DrawProfileEditorsSubmenu();

				ImGui::EndMenu();
			}

			DrawContextMenuItem<UINodeMapEditor>(CommonStrings::Nodes, "2");
			DrawContextMenuItem<UISettings>(CommonStrings::Settings, "4");
			DrawContextMenuItem<UIStats>(CommonStrings::Stats, "5");
			DrawContextMenuItem<UILog>(CommonStrings::Log, "6");

#if defined(IED_ENABLE_OUTFIT) && defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
			DrawContextMenuItem<OM::UIOutfitFormManager>(UIMainStrings::OutfitFormManager, "7");
#endif

			if (UIL::LCG_BM(UIMainStrings::Diagnostics, "8"))
			{
				DrawDiagnosticsSubmenu();

				ImGui::EndMenu();
			}

			DrawContextMenuItem<UIStyleEditorWindow>(UIProfileStrings::TitleStyle, "9");
		}

		void UIMain::DrawActionsMenu()
		{
			if (UIL::LCG_MI(UIMainStrings::EvaluateAll, "1"))
			{
				m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
			}

			if (UIL::LCG_MI(UIMainStrings::ResetAll, "2"))
			{
				m_controller.QueueResetAll(ControllerUpdateFlags::kNone);
			}
		}

		void UIMain::DrawHelpMenu()
		{
			if (UIL::LCG_MI(CommonStrings::About, "1"))
			{
				QueueAboutPopup();
			}
		}

		void UIMain::DrawDefaultConfigSubmenu()
		{
			if (UIL::LCG_MI(UIMainStrings::LoadDefaultAsCurrent, "1"))
			{
				m_popupQueue.push(
								UIPopupType::Confirm,
								UIL::LS(CommonStrings::Confirm),
								"%s",
								UIL::LS(UIMainStrings::LoadDefaultAsCurrentPrompt))
					.draw([this,
				           c_exists  = Serialization::FileExists(PATHS::DEFAULT_CONFIG),
				           uc_exists = Serialization::FileExists(PATHS::DEFAULT_CONFIG_USER)] {
						auto& settings = m_controller.GetSettings();

						ImGui::PushID("dc_sel_radio");

						UICommon::PushDisabled(!c_exists);

						if (ImGui::RadioButton(
								UIL::LS(CommonStrings::Default, "1"),
								settings.data.ui.selectedDefaultConfImport ==
									Data::DefaultConfigType::kDefault))
						{
							settings.set(
								settings.data.ui.selectedDefaultConfImport,
								Data::DefaultConfigType::kDefault);
						}

						UICommon::PopDisabled(!c_exists);

						ImGui::SameLine();

						UICommon::PushDisabled(!uc_exists);

						if (ImGui::RadioButton(
								UIL::LS(CommonStrings::User, "2"),
								settings.data.ui.selectedDefaultConfImport ==
									Data::DefaultConfigType::kUser))
						{
							settings.set(
								settings.data.ui.selectedDefaultConfImport,
								Data::DefaultConfigType::kUser);
						}

						UICommon::PopDisabled(!uc_exists);

						ImGui::PopID();

						switch (settings.data.ui.selectedDefaultConfImport)
						{
						case Data::DefaultConfigType::kDefault:
							return c_exists;
						case Data::DefaultConfigType::kUser:
							return uc_exists;
						default:
							return false;
						}
					})
					.call([this](const auto&) {
						const auto& settings = m_controller.GetSettings();

						switch (settings.data.ui.selectedDefaultConfImport)
						{
						case Data::DefaultConfigType::kDefault:
							if (auto window = GetChild<UIDialogImportExport>())
							{
								window->QueueImportPopup(
									PATHS::DEFAULT_CONFIG,
									UIL::LS(CommonStrings::Default));
							}
							break;
						case Data::DefaultConfigType::kUser:
							if (auto window = GetChild<UIDialogImportExport>())
							{
								window->QueueImportPopup(
									PATHS::DEFAULT_CONFIG_USER,
									UIL::LS(CommonStrings::User));
							}
							break;
						}
					});
			}

			if (UIL::LCG_MI(UIMainStrings::SaveCurrentAsDefault, "2"))
			{
				m_popupQueue.push(
								UIPopupType::Confirm,
								UIL::LS(CommonStrings::Confirm),
								"%s",
								UIL::LS(UIMainStrings::SaveCurrentAsDefaultPrompt))
					.draw([this] {
						auto& conf = m_controller.GetSettings();

						conf.mark_if(UIExportFilterWidget::DrawExportFilters(conf.data.ui.defaultExportFlags));

						return true;
					})
					.call([this](const auto&) {
						const auto& conf = GetUISettings();

						if (!m_controller.SaveCurrentConfigAsDefault(
								ExportFlags::kNone,
								conf.defaultExportFlags))
						{
							m_popupQueue.push(
								UIPopupType::Message,
								UIL::LS(CommonStrings::Confirm),
								"%s\n\n%s",
								UIL::LS(UIMainStrings::SaveCurrentAsDefaultError),
								m_controller.JSGetLastException().what());
						}
					})
					.set_text_wrap_size(22.f);
			}
		}

		bool UIMain::HasOpenChild() const
		{
			for (const auto& e : m_childWindows)
			{
				if (e && e->IsContextOpen())
				{
					return true;
				}
			}

			return false;
		}

		bool UIMain::ILRHGetCurrentControlLockSetting() const
		{
			return GetUISettings().enableControlLock;
		}

		bool UIMain::ILRHGetCurrentFreezeTimeSetting() const
		{
			return GetUISettings().enableFreezeTime;
		}

		void UIMain::OnOpen()
		{
			m_seenOpenChildThisSession = false;

			for (const auto& e : m_childWindows)
			{
				if (e && e->IsContextOpen())
				{
					m_seenOpenChildThisSession = true;
					e->OnOpen();
				}
			}

			if (m_lastClosedChild)
			{
				if (GetUISettings().exitOnLastWindowClose &&
				    !m_seenOpenChildThisSession)
				{
					if (auto context = GetChildContext(*m_lastClosedChild))
					{
						context->SetOpenState(true);
					}
				}

				m_lastClosedChild.reset();
			}
		}

		void UIMain::OnClose()
		{
			auto& settings = m_controller.GetSettings();

			using enum_type = std::underlying_type_t<ChildWindowID>;
			for (enum_type i = 0; i < stl::underlying(ChildWindowID::kMax); i++)
			{
				const auto& window = m_childWindows[i];

				if (!window)
				{
					continue;
				}

				const auto id        = static_cast<ChildWindowID>(i);
				const auto openState = window->IsContextOpen();

				if (openState)
				{
					window->OnClose();
				}

				window->OnMainClose();

				auto& s = settings.data.ui.windowOpenStates[i];

				if (settings.data.ui.exitOnLastWindowClose &&
				    m_lastClosedChild == id)
				{
					if (!s)
					{
						settings.set(s, true);
					}
				}
				else
				{
					if (s != openState)
					{
						settings.set(s, openState);
					}
				}
			}

			m_formLookupCache.clear();

			ILRHReset();

			m_controller.SaveSettings(true, true);
			Drivers::UI::QueueImGuiSettingsSave();
		}

		void UIMain::OnMouseMove(const Handlers::MouseMoveEvent& a_evn)
		{
			for (auto& e : m_childWindows)
			{
				if (e && e->IsContextOpen())
				{
					e->OnMouseMove(a_evn);
				}
			}
		}

		void UIMain::OnKeyEvent(const Handlers::KeyEvent& a_evn)
		{
			ILRHReceive(a_evn);

			for (auto& e : m_childWindows)
			{
				if (e && e->IsContextOpen())
				{
					e->OnKeyEvent(a_evn);
				}
			}
		}

	}
}