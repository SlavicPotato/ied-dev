#include "pch.h"

#include "UIConditionalVariablesEditorWindow.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIConditionalVariablesEditorWindow::UIConditionalVariablesEditorWindow(
			Controller& a_controller) :
			UIConditionalVariablesEditor(a_controller),
			UIEquipmentOverrideConditionsWidget(a_controller),
			UILocalizationInterface(a_controller),
			UITipsInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UIConditionalVariablesEditorWindow::Initialize()
		{
			EditorInitialize();
		}

		void UIConditionalVariablesEditorWindow::Reset()
		{
			EditorReset();
		}

		void UIConditionalVariablesEditorWindow::Draw()
		{
			SetWindowDimensions(200.0f, 800.0f);

			if (ImGui::Begin(
					LS<UIWidgetCommonStrings, 3>(
						UIWidgetCommonStrings::ConditionalVariables,
						WINDOW_ID),
					GetOpenState(),
					ImGuiWindowFlags_MenuBar))
			{
				DrawMenuBar();

				EditorDraw();
			}

			ImGui::End();
		}

		void UIConditionalVariablesEditorWindow::OnOpen()
		{
			EditorOnOpen();
		}

		void UIConditionalVariablesEditorWindow::OnClose()
		{
			EditorOnClose();
		}

		void UIConditionalVariablesEditorWindow::DrawMenuBar()
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(LS(CommonStrings::File, "1")))
				{
					if (ImGui::MenuItem(LS(CommonStrings::Exit, "1")))
					{
						SetOpenState(false);
					}

					ImGui::EndMenu();
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

				if (ImGui::BeginMenu(LS(CommonStrings::Actions, "2")))
				{
					EditorDrawMenuBarItems();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
		}

	}
}