#include "pch.h"

#include "UIOutfitEditorWindow.h"

#include "IED/UI/Widgets/UIWidgetCommonStrings.h"

#include "IED/UI/UILocalizationInterface.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		namespace OM
		{
			UIOutfitEditorWindow::UIOutfitEditorWindow(
				Controller& a_controller) :
				m_tabPanel(a_controller),
				m_controller(a_controller)
			{
			}

			void UIOutfitEditorWindow::Initialize()
			{
				m_tabPanel.Initialize();
			}

			void UIOutfitEditorWindow::Reset()
			{
				m_tabPanel.Reset();
			}

			void UIOutfitEditorWindow::Draw()
			{
				SetWindowDimensions(0, 1000, 800, true);

				if (ImGui::Begin(
						UIL::LS<UIWidgetCommonStrings, 3>(
							UIWidgetCommonStrings::OutfitConfig,
							WINDOW_ID),
						GetOpenState(),
						ImGuiWindowFlags_MenuBar))
				{
					DrawMenuBar();

					m_tabPanel.Draw();
				}

				ImGui::End();
			}

			void UIOutfitEditorWindow::OnOpen()
			{
				m_tabPanel.OnOpen();
			}

			void UIOutfitEditorWindow::OnClose()
			{
				m_tabPanel.OnClose();
			}

			void UIOutfitEditorWindow::DrawMenuBar()
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

			void UIOutfitEditorWindow::DrawFileMenu()
			{
				if (ImGui::MenuItem(UIL::LS(CommonStrings::Close, "1")))
				{
					SetOpenState(false);
				}
			}

			void UIOutfitEditorWindow::DrawToolsMenu()
			{
			}
		}
	}
}