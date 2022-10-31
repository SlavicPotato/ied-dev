#include "pch.h"

#include "UIMain.h"

#include "ConditionalVariables/Profile/UIProfileEditorConditionalVariables.h"
#include "ConditionalVariables/UIConditionalVariablesEditorWindow.h"
#include "Custom/Profile/UIProfileEditorCustom.h"
#include "EquipmentSlots/Profile/UIProfileEditorSlot.h"
#include "FormFilters/UIProfileEditorFormFilters.h"
#include "NodeOverride/Profile/UIProfileEditorNodeOverride.h"
#include "NodeOverride/UINodeOverrideEditorWindow.h"
#include "UIActorInfo.h"
#include "UIDialogImportExport.h"
#include "UIDisplayManagement.h"
#include "UIFormBrowser.h"
#include "UILog.h"
#include "UINodeMapEditor.h"
#include "UISettings.h"
#include "UISkeletonExplorer.h"
#include "UIStats.h"

#include "IED/Controller/Controller.h"

#include "I3DI/I3DIMain.h"

namespace IED
{
	namespace UI
	{
		UIMain::UIMain(
			Tasks::UIRenderTaskBase& a_owner,
			Controller&              a_controller) :
			UILocalizationInterface(a_controller),
			UIAboutModal(a_controller),
			UIKeyedInputLockReleaseHandler(a_owner),
			m_owner(a_owner),
			m_controller(a_controller),
			m_formLookupCache(a_controller),
			m_popupQueue(a_controller)
		{
			CreateChild<UIFormBrowser>(a_controller);
			CreateChild<UISettings>(a_owner, a_controller);
			CreateChild<UIDialogImportExport>(a_controller);
			CreateChild<UINodeMapEditor>(a_controller);
			CreateChild<UILog>(a_controller);
			CreateChild<UIStats>(a_owner, a_controller);
			CreateChild<UISkeletonExplorer>(a_controller);
			CreateChild<UIActorInfo>(a_controller);

			CreateChild<UIProfileEditorSlot>(a_controller);
			CreateChild<UIProfileEditorCustom>(a_controller);
			CreateChild<UIProfileEditorNodeOverride>(a_controller);
			CreateChild<UIProfileEditorFormFilters>(a_controller);
			CreateChild<UIProfileEditorConditionalVariables>(a_controller);

			CreateChild<UIDisplayManagement>(a_controller);
			CreateChild<UINodeOverrideEditorWindow>(
				a_controller,
				GetChild<UIProfileEditorNodeOverride>());
			CreateChild<UIConditionalVariablesEditorWindow>(a_controller);

			CreateChild<I3DIMain>(a_controller);
		}

		void UIMain::Initialize()
		{
			for (const auto& e : m_childWindows)
			{
				e->Initialize();
				e->AddSink(this);
			}

			const auto& settings = GetUISettings();

			using enum_type = std::underlying_type_t<ChildWindowID>;

			for (enum_type i = 0; i < stl::underlying(ChildWindowID::kMax); i++)
			{
				const auto& window = m_childWindows[i];

				window->Initialize();

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
				e->Reset();
			}
		}

		void UIMain::Draw()
		{
			//ImGui::ShowDemoWindow();

			ILRHBegin();

			DrawMenuBarMain();
			DrawChildWindows();

			if (GetChild<UIProfileEditorFormFilters>().ChangedConfig())
			{
				GetChildContext<UIDisplayManagement>().Notify(1);
			}

			m_popupQueue.run();

			ILRHEnd();

			m_formLookupCache.RunCleanup();
		}

		void UIMain::PrepareGameData()
		{
			for (auto& e : m_childWindows)
			{
				if (e->IsContextOpen())
				{
					e->PrepareGameData();
				}
			}
		}

		void UIMain::Render()
		{
			for (auto& e : m_childWindows)
			{
				if (e->IsContextOpen())
				{
					e->Render();
				}
			}
		}

		Data::SettingHolder::UserInterface& UIMain::GetUISettings() noexcept
		{
			return m_controller.GetConfigStore().settings.data.ui;
		}

		void UIMain::Receive(const UIContextStateChangeEvent& a_evn)
		{
			const auto id = static_cast<ChildWindowID>(a_evn.context.GetContextID());

			assert(id < ChildWindowID::kMax);

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
				e->DrawWrapper();
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
			if (LCG_BM(CommonStrings::File, "1"))
			{
				DrawFileMenu();

				ImGui::EndMenu();
			}

			if (LCG_BM(CommonStrings::View, "2"))
			{
				DrawViewMenu();

				ImGui::EndMenu();
			}

			if (LCG_BM(CommonStrings::Tools, "3"))
			{
				DrawToolsMenu();

				ImGui::EndMenu();
			}

			if (LCG_BM(CommonStrings::Help, "4"))
			{
				DrawHelpMenu();

				ImGui::EndMenu();
			}

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

			if (LCG_BM(CommonStrings::Actions, "X"))
			{
				DrawActionsMenu();

				ImGui::EndMenu();
			}
		}

		void UIMain::DrawFileMenu()
		{
			if (LCG_MI(UIMainStrings::ImportExport, "1"))
			{
				GetChildContext<UIDialogImportExport>().ToggleOpenState();
			}

			ImGui::Separator();

			if (LCG_BM(UIMainStrings::DefaultConfig, "2"))
			{
				DrawDefaultConfigSubmenu();

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (LCG_MI(CommonStrings::Exit, "3"))
			{
				SetOpenState(false);
			}
		}

		void UIMain::DrawViewMenu()
		{
			if (ImGui::MenuItem(
					LS(UIMainStrings::DisplayManagement, "1"),
					nullptr,
					GetChildContext<UIDisplayManagement>().IsContextOpen()))
			{
				GetChildContext<UIDisplayManagement>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(UIMainStrings::NodeOverride, "2"),
					nullptr,
					GetChildContext<UINodeOverrideEditorWindow>().IsContextOpen()))
			{
				GetChildContext<UINodeOverrideEditorWindow>().ToggleOpenState();
			}

			ImGui::Separator();

			if (ImGui::MenuItem(
					LS(UIMainStrings::ConditionalVariables, "3"),
					nullptr,
					GetChildContext<UIConditionalVariablesEditorWindow>().IsContextOpen()))
			{
				GetChildContext<UIConditionalVariablesEditorWindow>().ToggleOpenState();
			}

#if defined(IED_ENABLE_I3DI)
			if (ImGui::MenuItem(
					LS(UIMainStrings::I3DI, "4"),
					nullptr,
					GetChildContext<I3DIMain>().IsContextOpen()))
			{
				GetChildContext<I3DIMain>().ToggleOpenState();
			}
#endif
		}

		void UIMain::DrawProfileEditorsSubmenu()
		{
			if (ImGui::MenuItem(
					LS(CommonStrings::Equipment, "1"),
					nullptr,
					GetChildContext<UIProfileEditorSlot>().IsContextOpen()))
			{
				GetChildContext<UIProfileEditorSlot>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Custom, "2"),
					nullptr,
					GetChildContext<UIProfileEditorCustom>().IsContextOpen()))
			{
				GetChildContext<UIProfileEditorCustom>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(UIMainStrings::NodeOverride, "3"),
					nullptr,
					GetChildContext<UIProfileEditorNodeOverride>().IsContextOpen()))
			{
				GetChildContext<UIProfileEditorNodeOverride>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(UIMainStrings::FormFilters, "4"),
					nullptr,
					GetChildContext<UIProfileEditorFormFilters>().IsContextOpen()))
			{
				GetChildContext<UIProfileEditorFormFilters>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(UIMainStrings::ConditionalVariables, "5"),
					nullptr,
					GetChildContext<UIProfileEditorConditionalVariables>().IsContextOpen()))
			{
				GetChildContext<UIProfileEditorConditionalVariables>().ToggleOpenState();
			}
		}

		void UIMain::DrawDiagnosticsSubmenu()
		{
			if (ImGui::MenuItem(
					LS(UIWidgetCommonStrings::ActorInfo, "1"),
					nullptr,
					GetChildContext<UIActorInfo>().IsContextOpen()))
			{
				GetChildContext<UIActorInfo>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(UIWidgetCommonStrings::SkeletonExplorer, "2"),
					nullptr,
					GetChildContext<UISkeletonExplorer>().IsContextOpen()))
			{
				GetChildContext<UISkeletonExplorer>().ToggleOpenState();
			}
		}

		void UIMain::DrawToolsMenu()
		{
			if (LCG_BM(UIMainStrings::ProfileEditors, "1"))
			{
				DrawProfileEditorsSubmenu();

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Nodes, "2"),
					nullptr,
					GetChildContext<UINodeMapEditor>().IsContextOpen()))
			{
				GetChildContext<UINodeMapEditor>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Settings, "3"),
					nullptr,
					GetChildContext<UISettings>().IsContextOpen()))
			{
				GetChildContext<UISettings>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Stats, "4"),
					nullptr,
					GetChildContext<UIStats>().IsContextOpen()))
			{
				GetChildContext<UIStats>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Log, "5"),
					nullptr,
					GetChildContext<UILog>().IsContextOpen()))
			{
				GetChildContext<UILog>().ToggleOpenState();
			}

			if (LCG_BM(UIMainStrings::Diagnostics, "6"))
			{
				DrawDiagnosticsSubmenu();

				ImGui::EndMenu();
			}
		}

		void UIMain::DrawActionsMenu()
		{
			if (LCG_MI(UIMainStrings::EvaluateAll, "1"))
			{
				m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
			}

			if (LCG_MI(UIMainStrings::ResetAll, "2"))
			{
				m_controller.QueueResetAll(ControllerUpdateFlags::kNone);
			}
		}

		void UIMain::DrawHelpMenu()
		{
			if (LCG_MI(CommonStrings::About, "1"))
			{
				QueueAboutPopup();
			}
		}

		void UIMain::DrawDefaultConfigSubmenu()
		{
			if (LCG_MI(UIMainStrings::LoadDefaultAsCurrent, "1"))
			{
				m_popupQueue.push(
								UIPopupType::Confirm,
								LS(CommonStrings::Confirm),
								"%s",
								LS(UIMainStrings::LoadDefaultAsCurrentPrompt))
					.draw([this,
				           c_exists  = Serialization::FileExists(PATHS::DEFAULT_CONFIG),
				           uc_exists = Serialization::FileExists(PATHS::DEFAULT_CONFIG_USER)] {
						auto& settings = m_controller.GetConfigStore().settings;

						ImGui::PushID("dc_sel_radio");

						UICommon::PushDisabled(!c_exists);

						if (ImGui::RadioButton(
								LS(CommonStrings::Default, "1"),
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
								LS(CommonStrings::User, "2"),
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
						const auto& settings = m_controller.GetConfigStore().settings;

						switch (settings.data.ui.selectedDefaultConfImport)
						{
						case Data::DefaultConfigType::kDefault:
							GetChild<UIDialogImportExport>().QueueImportPopup(
								PATHS::DEFAULT_CONFIG,
								LS(CommonStrings::Default));
							break;
						case Data::DefaultConfigType::kUser:
							GetChild<UIDialogImportExport>().QueueImportPopup(
								PATHS::DEFAULT_CONFIG_USER,
								LS(CommonStrings::User));
							break;
						}
					});
			}

			if (LCG_MI(UIMainStrings::SaveCurrentAsDefault, "2"))
			{
				m_popupQueue.push(
								UIPopupType::Confirm,
								LS(CommonStrings::Confirm),
								"%s",
								LS(UIMainStrings::SaveCurrentAsDefaultPrompt))
					.draw([this] {
						auto& conf = m_controller.GetConfigStore().settings;

						conf.mark_if(DrawExportFilters(conf.data.ui.defaultExportFlags));

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
								LS(CommonStrings::Confirm),
								"%s\n\n%s",
								LS(UIMainStrings::SaveCurrentAsDefaultError),
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
				if (e->IsContextOpen())
				{
					return true;
				}
			}

			return false;
		}

		bool UIMain::ILRHGetCurrentControlLockSetting()
		{
			return GetUISettings().enableControlLock;
		}

		bool UIMain::ILRHGetCurrentFreezeTimeSetting()
		{
			return GetUISettings().enableFreezeTime;
		}

		void UIMain::OnOpen()
		{
			m_seenOpenChildThisSession = false;

			for (const auto& e : m_childWindows)
			{
				if (e->IsContextOpen())
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
					GetChildContext(*m_lastClosedChild).SetOpenState(true);
				}

				m_lastClosedChild.reset();
			}
		}

		void UIMain::OnClose()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			using enum_type = std::underlying_type_t<ChildWindowID>;
			for (enum_type i = 0; i < stl::underlying(ChildWindowID::kMax); i++)
			{
				const auto  id        = static_cast<ChildWindowID>(i);
				const auto& window    = m_childWindows[i];
				const auto  openState = window->IsContextOpen();

				if (openState)
				{
					window->OnClose();
				}

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
				if (e->IsContextOpen())
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
				if (e->IsContextOpen())
				{
					e->OnKeyEvent(a_evn);
				}
			}
		}

	}
}