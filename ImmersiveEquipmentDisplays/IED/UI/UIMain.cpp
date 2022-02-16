#include "pch.h"

#include "UIMain.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIMain::UIMain(Controller& a_controller) :
			UILocalizationInterface(a_controller),
			UIAboutModal(a_controller),
			m_controller(a_controller),
			m_slotTabPanel(a_controller),
			m_customTabPanel(a_controller),
			m_formBrowser(a_controller),
			m_slotProfileEditor(a_controller),
			m_customProfileEditor(a_controller),
			m_settings(a_controller),
			m_importExport(a_controller),
			m_nodeMapEditor(a_controller),
			m_formLookupCache(a_controller),
			m_stats(a_controller),
			m_log(a_controller),
			m_nodeOverrideEditor(a_controller, m_nodeOverrideProfileEditor),
			m_nodeOverrideProfileEditor(a_controller),
			m_formFiltersProfileEditor(a_controller),
			m_popupQueue(a_controller)
		{
			stl::snprintf(m_currentTitle, "%s###%s", TITLE_NAME, WINDOW_ID);
		}

		void UIMain::Initialize()
		{
			m_slotTabPanel.Initialize();
			m_customTabPanel.Initialize();
			m_slotProfileEditor.Initialize();
			m_customProfileEditor.Initialize();
			m_nodeOverrideProfileEditor.Initialize();
			m_log.Initialize();
			m_nodeOverrideEditor.Initialize();
			m_formFiltersProfileEditor.Initialize();

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
			m_slotTabPanel.Reset();
			m_customTabPanel.Reset();
			m_nodeOverrideEditor.Reset();
			m_nodeOverrideProfileEditor.Reset();
			m_slotProfileEditor.Reset();
			m_customProfileEditor.Reset();
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

				switch (m_currentEditorPanel)
				{
				case UIEditorPanel::Slot:
					m_slotTabPanel.Draw();
					break;
				case UIEditorPanel::Custom:
					m_customTabPanel.Draw();
					break;
				}
			}

			ImGui::End();

			m_slotProfileEditor.DrawProfileEditor();
			m_customProfileEditor.DrawProfileEditor();
			m_nodeOverrideProfileEditor.DrawProfileEditor();
			m_settings.Draw();
			m_importExport.Draw();
			m_nodeMapEditor.Draw();
			m_stats.Draw();
			m_log.Draw();
			m_nodeOverrideEditor.Draw();
			m_formFiltersProfileEditor.DrawProfileEditor();

			if (m_formFiltersProfileEditor.ChangedConfig())
			{
				m_slotTabPanel.QueueUpdateCurrent();
				m_customTabPanel.QueueUpdateCurrent();
			}

			m_popupQueue.run();

			m_formLookupCache.RunCleanup();
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

				switch (m_currentEditorPanel)
				{
				case UIEditorPanel::Slot:
					m_slotTabPanel.DrawMenuBarItems();
					break;
				case UIEditorPanel::Custom:
					m_customTabPanel.DrawMenuBarItems();
					break;
				}

				ImGui::EndMenuBar();
			}
		}

		void UIMain::DrawFileMenu()
		{
			if (LCG_MI(UIMainStrings::ImportExport, "1"))
			{
				m_importExport.ToggleOpenState();
				if (m_importExport.IsWindowOpen())
				{
					m_importExport.OnOpen();
				}
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
					LS(CommonStrings::Equipment, "1"),
					nullptr,
					m_currentEditorPanel == UIEditorPanel::Slot))
			{
				OpenEditorPanel(UIEditorPanel::Slot);
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Custom, "2"),
					nullptr,
					m_currentEditorPanel == UIEditorPanel::Custom))
			{
				OpenEditorPanel(UIEditorPanel::Custom);
			}

			ImGui::Separator();

			if (ImGui::MenuItem(
					LS(UIMainStrings::NodeOverride, "3"),
					nullptr,
					m_nodeOverrideEditor.IsWindowOpen()))
			{
				m_nodeOverrideEditor.ToggleOpenState();
				if (m_nodeOverrideEditor.IsWindowOpen())
				{
					m_nodeOverrideEditor.OnOpen();
				}
			}
		}

		void UIMain::DrawToolsMenu()
		{
			if (LCG_BM(UIMainStrings::ProfileEditors, "1"))
			{
				if (ImGui::MenuItem(
						LS(CommonStrings::Equipment, "1"),
						nullptr,
						m_slotProfileEditor.IsWindowOpen()))
				{
					m_slotProfileEditor.ToggleOpenState();
				}

				if (ImGui::MenuItem(
						LS(CommonStrings::Custom, "2"),
						nullptr,
						m_customProfileEditor.IsWindowOpen()))
				{
					m_customProfileEditor.ToggleOpenState();
				}

				if (ImGui::MenuItem(
						LS(UIMainStrings::NodeOverride, "3"),
						nullptr,
						m_nodeOverrideProfileEditor.IsWindowOpen()))
				{
					m_nodeOverrideProfileEditor.ToggleOpenState();
				}

				if (ImGui::MenuItem(
						LS(UIMainStrings::FormFilters, "4"),
						nullptr,
						m_formFiltersProfileEditor.IsWindowOpen()))
				{
					m_formFiltersProfileEditor.ToggleOpenState();
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Nodes, "2"),
					nullptr,
					m_nodeMapEditor.IsWindowOpen()))
			{
				m_nodeMapEditor.ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Settings, "3"),
					nullptr,
					m_settings.IsWindowOpen()))
			{
				m_settings.ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Stats, "4"),
					nullptr,
					m_stats.IsWindowOpen()))
			{
				m_stats.ToggleOpenState();
			}

			if (ImGui::MenuItem(
					LS(CommonStrings::Log, "5"),
					nullptr,
					m_log.IsWindowOpen()))
			{
				m_log.ToggleOpenState();
			}

			ImGui::Separator();

			if (LCG_BM(CommonStrings::Actions, "6"))
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
							m_importExport.QueueImportPopup(
								PATHS::DEFAULT_CONFIG,
								LS(CommonStrings::Default));
							break;
						case Data::DefaultConfigType::kUser:
							m_importExport.QueueImportPopup(
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
						auto& conf = m_controller.GetConfigStore().settings.data.ui;
						if (!m_controller.SaveCurrentConfigAsDefault(conf.defaultExportFlags))
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

			auto& conf = m_controller.GetConfigStore().settings;
			conf.set(conf.data.ui.lastPanel, a_panel);

			switch (oldPanel)
			{
			case UIEditorPanel::Slot:
				m_slotTabPanel.OnClose();
				break;
			case UIEditorPanel::Custom:
				m_customTabPanel.OnClose();
				break;
			}

			OnOpen();
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
			switch (m_currentEditorPanel)
			{
			case UIEditorPanel::Slot:
				m_slotTabPanel.OnOpen();
				SetTitle(stl::underlying(CommonStrings::Equipment));
				break;
			case UIEditorPanel::Custom:
				m_customTabPanel.OnOpen();
				SetTitle(stl::underlying(CommonStrings::Custom));
				break;
			}

			if (m_nodeOverrideEditor.IsWindowOpen())
			{
				m_nodeOverrideEditor.OnOpen();
			}
		}

		void UIMain::OnClose()
		{
			m_controller.GetConfigStore().settings.SaveIfDirty();
			ImGui::SaveIniSettingsToDisk(PATHS::IMGUI_INI);

			m_slotTabPanel.OnClose();
			m_customTabPanel.OnClose();
			m_formBrowser.OnClose();

			m_formLookupCache.clear();

			if (m_nodeOverrideEditor.IsWindowOpen())
			{
				m_nodeOverrideEditor.OnClose();
			}

			m_controller.ClearActorInfo();
		}

	}
}