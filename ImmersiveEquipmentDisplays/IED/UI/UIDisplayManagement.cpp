#include "pch.h"

#include "UIDisplayManagement.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/UIMainStrings.h"

namespace IED
{
	namespace UI
	{
		UIDisplayManagement::UIDisplayManagement(
			Controller& a_controller) :
			m_editorPanels{
				std::make_unique<UISlotEditorTabPanel>(a_controller),
				std::make_unique<UICustomEditorTabPanel>(a_controller)
			},
			m_controller(a_controller)
		{
			stl::snprintf(m_currentTitle, "###%s", WINDOW_ID);
		}

		void UIDisplayManagement::Initialize()
		{
			for (auto& e : m_editorPanels)
			{
				e->Initialize();
			}

			auto& conf = m_controller.GetSettings();

			switch (conf.data.ui.lastPanel)
			{
			case UIDisplayManagementEditorPanel::Custom:
				m_currentEditorPanel = UIDisplayManagementEditorPanel::Custom;
				SetTitle(stl::underlying(CommonStrings::Custom));
				break;
			default:
				m_currentEditorPanel = UIDisplayManagementEditorPanel::Slot;
				SetTitle(stl::underlying(CommonStrings::Equipment));
				break;
			}
		}

		void UIDisplayManagement::Reset()
		{
			for (auto& e : m_editorPanels)
			{
				e->Reset();
			}
		}

		void UIDisplayManagement::Draw()
		{
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
		}

		void UIDisplayManagement::OnOpen()
		{
			GetEditorPanelBase(m_currentEditorPanel).OnOpen();
		}

		void UIDisplayManagement::OnClose()
		{
			GetEditorPanelBase(m_currentEditorPanel).OnClose();
		}

		void UIDisplayManagement::Notify(std::uint32_t a_code, void* a_params)
		{
			if (a_code == 1)
			{
				GetEditorPanelBase(m_currentEditorPanel).QueueUpdateCurrent();
			}
		}

		void UIDisplayManagement::DrawMenuBar()
		{
			ImGui::PushID("menu_bar");

			if (ImGui::BeginMenuBar())
			{
				if (UIL::LCG_BM(CommonStrings::File, "1"))
				{
					if (UIL::LCG_MI(CommonStrings::Close, "1"))
					{
						SetOpenState(false);
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_BM(CommonStrings::View, "2"))
				{
					DrawViewMenu();

					ImGui::EndMenu();
				}

				ImGui::Separator();

				GetEditorPanelBase(m_currentEditorPanel).DrawMenuBarItems();

				ImGui::EndMenuBar();
			}

			ImGui::PopID();
		}

		void UIDisplayManagement::DrawViewMenu()
		{
			if (ImGui::MenuItem(
					UIL::LS(UIMainStrings::EquipmentDisplays, "1"),
					nullptr,
					m_currentEditorPanel == UIDisplayManagementEditorPanel::Slot))
			{
				OpenEditorPanel(UIDisplayManagementEditorPanel::Slot);
			}

			if (ImGui::MenuItem(
					UIL::LS(UIMainStrings::CustomDisplays, "2"),
					nullptr,
					m_currentEditorPanel == UIDisplayManagementEditorPanel::Custom))
			{
				OpenEditorPanel(UIDisplayManagementEditorPanel::Custom);
			}
		}

		void UIDisplayManagement::OpenEditorPanel(UIDisplayManagementEditorPanel a_panel)
		{
			const auto oldPanel = m_currentEditorPanel;

			if (oldPanel == a_panel)
			{
				return;
			}

			m_currentEditorPanel = a_panel;

			GetEditorPanelBase(oldPanel).OnClose();
			GetEditorPanelBase(a_panel).OnOpen();

			switch (a_panel)
			{
			case UIDisplayManagementEditorPanel::Custom:
				SetTitle(stl::underlying(CommonStrings::Custom));
				break;
			default:
				SetTitle(stl::underlying(CommonStrings::Equipment));
				break;
			}

			auto& conf = m_controller.GetSettings();
			conf.set(conf.data.ui.lastPanel, a_panel);
		}

		void UIDisplayManagement::SetTitle(Localization::StringID a_strid)
		{
			constexpr std::size_t BUFFER_SIZE = 64;

			const auto buf = std::make_unique_for_overwrite<char[]>(BUFFER_SIZE);

			::_snprintf_s(
				buf.get(),
				BUFFER_SIZE,
				_TRUNCATE,
				"%s - %s",
				UIL::LS(UIWidgetCommonStrings::DisplayManagement),
				UIL::LS(a_strid));

			stl::snprintf(
				m_currentTitle,
				"%s",
				UIL::LMKID<3>(buf.get(), WINDOW_ID));
		}

	}
}
