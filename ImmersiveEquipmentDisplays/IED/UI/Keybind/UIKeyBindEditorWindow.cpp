#include "pch.h"

#include "UIKeyBindEditorWindow.h"

#include "IED/UI/Widgets/UIKeyBindIDSelectorWidget.h"
#include "IED/UI/Widgets/UIWidgetCommonStrings.h"

#include "IED/UI/UIMainStrings.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		UIKeyBindEditorWindow::UIKeyBindEditorWindow(Controller& a_controller) :
			m_controller(a_controller)
		{
		}

		void UIKeyBindEditorWindow::Draw()
		{
			SetWindowDimensions(0.0f, 800.0f, 600.0f, true);

			if (ImGui::Begin(
					UIL::LS<UIMainStrings, 3>(
						UIMainStrings::KeyBinds,
						WINDOW_ID),
					GetOpenState(),
					ImGuiWindowFlags_MenuBar))
			{
				DrawMenuBar();

				ImGui::PushItemWidth(ImGui::GetFontSize() * -8.0f);

				DrawEditor();

				ImGui::PopItemWidth();
			}

			ImGui::End();
		}

		void UIKeyBindEditorWindow::OnClose()
		{
			m_controller.SaveKeyBinds(true, true);
		}

		void UIKeyBindEditorWindow::DrawEditor()
		{
			if (DrawEditorImpl())
			{
				m_controller.GetKeyBindDataHolder()->MarkDirty();
				m_controller.RequestEvaluateAll();
			}
		}

		bool UIKeyBindEditorWindow::DrawEditorImpl()
		{
			auto& holder = m_controller.GetKeyBindDataHolder();

			const stl::lock_guard lock(holder->GetLock());

			return UIKeyBindEditorWidget::DrawKeyBindEditorWidget(holder->GetData());
		}

		void UIKeyBindEditorWindow::DrawMenuBar()
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(UIL::LS(CommonStrings::File, "1")))
				{
					DrawFileMenu();

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::BeginMenu(UIL::LS(CommonStrings::Actions, "2")))
				{
					DrawActionMenu();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
		}

		void UIKeyBindEditorWindow::DrawFileMenu()
		{
			if (ImGui::MenuItem(UIL::LS(CommonStrings::Save, "1")))
			{
				m_controller.SaveKeyBinds(true, false);
				m_controller.RequestEvaluateAll();
			}

			ImGui::Separator();

			if (ImGui::MenuItem(UIL::LS(CommonStrings::Exit, "2")))
			{
				SetOpenState(false);
			}
		}

		void UIKeyBindEditorWindow::DrawActionMenu()
		{
			if (ImGui::BeginMenu(UIL::LS(CommonStrings::Add, "1")))
			{
				DrawAddPopup();

				ImGui::EndMenu();
			}
		}

		void UIKeyBindEditorWindow::DrawAddPopup()
		{
			const bool result = UIKeyBindIDSelectorWidget::DrawKeyBindIDSelector(m_tmpID);

			if (result && !m_tmpID.empty())
			{
				AddKeyBind(std::move(m_tmpID));

				ImGui::CloseCurrentPopup();
			}
		}

		void UIKeyBindEditorWindow::AddKeyBind(std::string&& a_id)
		{
			auto& holder = m_controller.GetKeyBindDataHolder();

			const stl::lock_guard lock(holder->GetLock());

			auto& entries = holder->GetData().entries;

			entries.try_emplace(std::move(a_id));
			holder->MarkDirty();
		}
	}
}