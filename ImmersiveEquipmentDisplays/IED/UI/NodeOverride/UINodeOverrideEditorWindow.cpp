#include "pch.h"

#include "UINodeOverrideEditorWindow.h"
#include "UINodeOverrideEditorWindowStrings.h"

#include "IED/UI/NodeOverride/Profile/UIProfileEditorNodeOverride.h"
#include "IED/UI/UIMain.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UINodeOverrideEditorWindow::UINodeOverrideEditorWindow(
			Controller& a_controller) :
			m_tabPanel(a_controller),
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
			SetWindowDimensions(100.0f, 550.0f);

			if (ImGui::Begin(
					UIL::LS<UIWidgetCommonStrings, 3>(
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
						UIL::LS(UINodeOverrideEditorWindowStrings::DisabledInINI));

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();
				}

				UICommon::PushDisabled(disabled);

				m_tabPanel.Draw();

				UICommon::PopDisabled(disabled);
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
				if (ImGui::BeginMenu(UIL::LS(CommonStrings::File, "1")))
				{
					DrawFileMenu();

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(UIL::LS(CommonStrings::Tools, "2")))
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
			if (ImGui::MenuItem(UIL::LS(CommonStrings::Close, "1")))
			{
				SetOpenState(false);
			}
		}

		void UINodeOverrideEditorWindow::DrawToolsMenu()
		{
			auto& rt = m_controller.UIGetRenderTask();

			auto context = rt ? rt->GetContext().GetChildContext<UIProfileEditorNodeOverride>() : nullptr;

			if (ImGui::MenuItem(
					UIL::LS(UIWidgetCommonStrings::ProfileEditor, "1"),
					nullptr,
					context && context->IsContextOpen(),
					static_cast<bool>(context)))
			{
				if (context)
				{
					context->ToggleOpenState();
				}
			}
		}
	}
}
