#include "pch.h"

#include "UINodeOverrideEditorWindow.h"
#include "UINodeOverrideEditorWindowStrings.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UINodeOverrideEditorWindow::UINodeOverrideEditorWindow(
			Controller&   a_controller,
			UIWindowBase& a_profileEditor) :
			UILocalizationInterface(a_controller),
			m_tabPanel(a_controller),
			m_profileEditor(a_profileEditor),
			m_controller(a_controller)
		{
		}

		void UINodeOverrideEditorWindow::Initialize()
		{
			m_tabPanel.Initialize();
		}

		void UINodeOverrideEditorWindow::Reset()
		{
			m_tabPanel.Reset();
		}

		void UINodeOverrideEditorWindow::Draw()
		{
			if (!IsWindowOpen())
			{
				return;
			}

			SetWindowDimensions(100.0f, 550.0f);

			if (ImGui::Begin(
					LS<UIWidgetCommonStrings, 3>(
						UIWidgetCommonStrings::GearPositioning,
						WINDOW_ID),
					GetOpenState(),
					ImGuiWindowFlags_MenuBar))
			{
				DrawMenuBar();

				auto disabled = !m_controller.GetNodeOverrideEnabled();

				if (disabled)
				{
					ImGui::TextColored(
						UICommon::g_colorWarning,
						LS(UINodeOverrideEditorWindowStrings::DisabledInINI));

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();
				}

				UICommon::PushDisabled(disabled);

				m_tabPanel.Draw();

				UICommon::PopDisabled(disabled);

				if (!IsWindowOpen())
				{
					OnClose();
				}
			}

			ImGui::End();
		}

		void UINodeOverrideEditorWindow::OnOpen()
		{
			m_tabPanel.OnOpen();
		}

		void UINodeOverrideEditorWindow::OnClose()
		{
			m_tabPanel.OnClose();
		}

		void UINodeOverrideEditorWindow::DrawMenuBar()
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(LS(CommonStrings::File, "1")))
				{
					DrawFileMenu();

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(LS(CommonStrings::Tools, "2")))
				{
					DrawToolsMenu();

					ImGui::EndMenu();
				}

				ImGui::Separator();

				m_tabPanel.DrawMenuBarItems();

				ImGui::EndMenuBar();
			}
		}

		void UINodeOverrideEditorWindow::DrawFileMenu()
		{
			if (ImGui::MenuItem(LS(CommonStrings::Exit, "1")))
			{
				SetOpenState(false);
			}
		}

		void UINodeOverrideEditorWindow::DrawToolsMenu()
		{
			if (ImGui::MenuItem(
					LS(UIWidgetCommonStrings::ProfileEditor, "1"),
					nullptr,
					m_profileEditor.IsWindowOpen()))
			{
				m_profileEditor.ToggleOpenState();
			}
		}
	}
}
