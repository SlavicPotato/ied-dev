#include "pch.h"

#include "UIMain.h"

#include "Custom/Profile/UIProfileEditorCustom.h"
#include "EquipmentSlots/Profile/UIProfileEditorSlot.h"
#include "FormFilters/UIProfileEditorFormFilters.h"
#include "NodeOverride/Profile/UIProfileEditorNodeOverride.h"
#include "NodeOverride/UINodeOverrideEditorWindow.h"
#include "UIActorInfo.h"
#include "UIDialogImportExport.h"
#include "UIFormBrowser.h"
#include "UILog.h"
#include "UINodeMapEditor.h"
#include "UISettings.h"
#include "UISkeletonExplorer.h"
#include "UIStats.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIMain::UIMain(Controller& a_controller) :
			UIContextBase(a_controller),
			UILocalizationInterface(a_controller),
			UIAboutModal(a_controller),
			m_controller(a_controller),
			m_formLookupCache(a_controller),
#if defined(IED_ENABLE_I3DI)
			m_i3di(a_controller),
#endif
			m_popupQueue(a_controller)
		{
			CreateEditorPanel<UISlotEditorTabPanel>(a_controller);
			CreateEditorPanel<UICustomEditorTabPanel>(a_controller);

			CreateChildWindow<UIFormBrowser>(a_controller);
			CreateChildWindow<UISettings>(a_controller);
			CreateChildWindow<UIDialogImportExport>(a_controller);
			CreateChildWindow<UINodeMapEditor>(a_controller);
			CreateChildWindow<UILog>(a_controller);
			CreateChildWindow<UIStats>(a_controller);
			CreateChildWindow<UISkeletonExplorer>(a_controller);
			CreateChildWindow<UIActorInfo>(a_controller);

			CreateChildWindow<UIProfileEditorSlot>(a_controller);
			CreateChildWindow<UIProfileEditorCustom>(a_controller);
			CreateChildWindow<UIProfileEditorNodeOverride>(a_controller);
			CreateChildWindow<UIProfileEditorFormFilters>(a_controller);

			CreateChildWindow<UINodeOverrideEditorWindow>(
				a_controller,
				GetChildWindow<UIProfileEditorNodeOverride>());

			stl::snprintf(m_currentTitle, "%s###%s", TITLE_NAME, WINDOW_ID);
		}

		void UIMain::Initialize()
		{
			for (auto& e : m_editorPanels)
			{
				e->Initialize();
			}

			for (auto& e : m_childWindows)
			{
				e->Initialize();
			}

#if defined(IED_ENABLE_I3DI)
			m_i3di.Initialize();
			m_i3di.SetOpenState(true);
			m_i3di.OnOpen();
#endif

			auto& conf = m_controller.GetConfigStore().settings;

			switch (conf.data.ui.lastPanel)
			{
			case UIEditorPanel::Custom:
				m_currentEditorPanel = UIEditorPanel::Custom;
				SetTitle(stl::underlying(CommonStrings::Custom));
				break;
			default:
				m_currentEditorPanel = UIEditorPanel::Slot;
				SetTitle(stl::underlying(CommonStrings::Equipment));
				break;
			}
		}

		void UIMain::Reset()
		{
			for (auto& e : m_editorPanels)
			{
				e->Reset();
			}

			for (auto& e : m_childWindows)
			{
				e->Reset();
			}

#if defined(IED_ENABLE_I3DI)
			m_i3di.Reset();
#endif
		}

		void UIMain::Draw()
		{
			//ImGui::ShowDemoWindow();

			SetWindowDimensions(10.0f, 600.0f);

			if (ImGui::Begin(
					m_currentTitle,
					GetOpenState(),
					ImGuiWindowFlags_MenuBar))
			{
				DrawMenuBar();

				GetEditorPanelBase(m_currentEditorPanel).Draw();
			}

			ImGui::End();

			for (auto& e : m_childWindows)
			{
				e->DrawWrapper();
			}

#if defined(IED_ENABLE_I3DI)
			m_i3di.Draw();
#endif

			if (GetChildWindow<UIProfileEditorFormFilters>().ChangedConfig())
			{
				GetEditorPanelBase(m_currentEditorPanel).QueueUpdateCurrent();
			}

			m_popupQueue.run();

			m_formLookupCache.RunCleanup();
		}

		void UIMain::PrepareGameData()
		{
#if defined(IED_ENABLE_I3DI)
			m_i3di.PrepareGameData();
#endif
		}

		void UIMain::Render()
		{
#if defined(IED_ENABLE_I3DI)
			m_i3di.Render();
#endif
		}

		const Data::SettingHolder::UserInterface& UIMain::GetUISettings() noexcept
		{
			return m_controller.GetConfigStore().settings.data.ui;
		}

		void UIMain::DrawMenuBar()
		{
			if (ImGui::BeginMenuBar())
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

				ImGui::Separator();

				GetEditorPanelBase(m_currentEditorPanel).DrawMenuBarItems();

				ImGui::EndMenuBar();
			}
		}

		void UIMain::DrawFileMenu()
		{
			if (LCG_MI(UIMainStrings::ImportExport, "1"))
			{
				GetChildWindowBase<UIDialogImportExport>().ToggleOpenState();
			}

			ImGui::Separator();

			if (LCG_BM(UIMainStrings::DefaultConfig, "2"))
			{
				DrawDefaultConfigSubmenu();
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (LCG_MI(CommonStrings::Exit, "2"))
			{
				SetOpenState(false);
			}
		}

		void UIMain::DrawViewMenu()
		{
			if (ImGui::MenuItem(
					LS(UIMainStrings::EquipmentDisplays, "1"),
					nullptr,
					m_currentEditorPanel == UIEditorPanel::Slot))
			{
				OpenEditorPanel(UIEditorPanel::Slot);
			}

			if (ImGui::MenuItem(
					LS(UIMainStrings::CustomDisplays, "2"),
					nullptr,
					m_currentEditorPanel == UIEditorPanel::Custom))
			{
				OpenEditorPanel(UIEditorPanel::Custom);
			}

			ImGui::Separator();

			if (ImGui::MenuItem(
					LS(UIMainStrings::NodeOverride, "3"),
					nullptr,
					GetChildWindowBase<UINodeOverrideEditorWindow>().IsWindowOpen()))
			{
				GetChildWindowBase<UINodeOverrideEditorWindow>().ToggleOpenState();
			}
		}

		void UIMain::DrawToolsMenu()
		{
			if (LCG_BM(UIMainStrings::ProfileEditors, "1"))
			{
				if (ImGui::MenuItem(
						LS(CommonStrings::Equipment, "1"),
						nullptr,
						GetChildWindowBase<UIProfileEditorSlot>().IsWindowOpen()))
				{
					GetChildWindowBase<UIProfileEditorSlot>().ToggleOpenState();
				}

				if (ImGui::MenuItem(
						LS(CommonStrings::Custom, "2"),
						nullptr,
						GetChildWindowBase<UIProfileEditorCustom>().IsWindowOpen()))
				{
					GetChildWindowBase<UIProfileEditorCustom>().ToggleOpenState();
				}

				if (ImGui::MenuItem(
						LS(UIMainStrings::NodeOverride, "3"),
						nullptr,
						GetChildWindowBase<UIProfileEditorNodeOverride>().IsWindowOpen()))
				{
					GetChildWindowBase<UIProfileEditorNodeOverride>().ToggleOpenState();
				}

				if (ImGui::MenuItem(
						LS(UIMainStrings::FormFilters, "4"),
						nullptr,
						GetChildWindowBase<UIProfileEditorFormFilters>().IsWindowOpen()))
				{
					GetChildWindowBase<UIProfileEditorFormFilters>().ToggleOpenState();
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Nodes, "2"),
					nullptr,
					GetChildWindowBase<UINodeMapEditor>().IsWindowOpen()))
			{
				GetChildWindowBase<UINodeMapEditor>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Settings, "3"),
					nullptr,
					GetChildWindowBase<UISettings>().IsWindowOpen()))
			{
				GetChildWindowBase<UISettings>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Stats, "4"),
					nullptr,
					GetChildWindowBase<UIStats>().IsWindowOpen()))
			{
				GetChildWindowBase<UIStats>().ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Log, "5"),
					nullptr,
					GetChildWindowBase<UILog>().IsWindowOpen()))
			{
				GetChildWindowBase<UILog>().ToggleOpenState();
			}

			if (LCG_BM(UIMainStrings::Diagnostics, "6"))
			{
				if (ImGui::MenuItem(
						LS(UIWidgetCommonStrings::ActorInfo, "1"),
						nullptr,
						GetChildWindowBase<UIActorInfo>().IsWindowOpen()))
				{
					GetChildWindowBase<UIActorInfo>().ToggleOpenState();
				}

				if (ImGui::MenuItem(
						LS(UIWidgetCommonStrings::SkeletonExplorer, "2"),
						nullptr,
						GetChildWindowBase<UISkeletonExplorer>().IsWindowOpen()))
				{
					GetChildWindowBase<UISkeletonExplorer>().ToggleOpenState();
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (LCG_BM(CommonStrings::Actions, "X"))
			{
				DrawActionsMenu();
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

						return true;
					})
					.call([this](const auto&) {
						const auto& settings = m_controller.GetConfigStore().settings;

						switch (settings.data.ui.selectedDefaultConfImport)
						{
						case Data::DefaultConfigType::kDefault:
							GetChildWindow<UIDialogImportExport>().QueueImportPopup(
								PATHS::DEFAULT_CONFIG,
								LS(CommonStrings::Default));
							break;
						case Data::DefaultConfigType::kUser:
							GetChildWindow<UIDialogImportExport>().QueueImportPopup(
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
						const auto& conf = m_controller.GetConfigStore().settings.data.ui;

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

		void UIMain::OpenEditorPanel(UIEditorPanel a_panel)
		{
			if (m_currentEditorPanel == a_panel)
			{
				return;
			}

			auto oldPanel = m_currentEditorPanel;

			m_currentEditorPanel = a_panel;

			GetEditorPanelBase(oldPanel).OnClose();
			GetEditorPanelBase(a_panel).OnOpen();

			SetTitle(stl::underlying(CommonStrings::Equipment));

			auto& conf = m_controller.GetConfigStore().settings;
			conf.set(conf.data.ui.lastPanel, a_panel);
		}

		void UIMain::SetTitle(Localization::StringID a_strid)
		{
			stl::snprintf(
				m_currentTitle,
				"%s - %s",
				TITLE_NAME,
				LS<decltype(a_strid), 3>(
					a_strid,
					WINDOW_ID));
		}

		void UIMain::OnOpen()
		{
			GetEditorPanelBase(m_currentEditorPanel).OnOpen();

			for (auto& e : m_childWindows)
			{
				if (e->IsWindowOpen())
				{
					e->OnOpen();
				}
			}

#if defined(IED_ENABLE_I3DI)
			if (m_i3di.IsWindowOpen())
			{
				m_i3di.OnOpen();
			}
#endif
		}

		void UIMain::OnClose()
		{
			m_controller.GetConfigStore().settings.SaveIfDirty();
			ImGui::SaveIniSettingsToDisk(PATHS::IMGUI_INI);

			GetEditorPanelBase(m_currentEditorPanel).OnClose();

			for (auto& e : m_childWindows)
			{
				if (e->IsWindowOpen())
				{
					e->OnClose();
				}
			}

#if defined(IED_ENABLE_I3DI)
			m_i3di.OnClose();
#endif

			m_formLookupCache.clear();

			m_controller.ClearActorInfo();
		}

		void UIMain::OnMouseMove(const Handlers::MouseMoveEvent& a_evn)
		{
#if defined(IED_ENABLE_I3DI)
			m_i3di.OnMouseMove(a_evn);
#endif
		}

	}
}