#include "pch.h"

#include "UIEditorTabPanel.h"

#include "IED/Controller/Controller.h"

#include "UICommon.h"

#include "Widgets/UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		UIEditorTabPanel::UIEditorTabPanel(
			Controller&            a_controller,
			Localization::StringID a_menuName) :
			m_controller(a_controller),
			m_menuName(a_menuName)
		{
		}

		void UIEditorTabPanel::Initialize()
		{
			for (const auto& e : m_interfaces)
			{
				if (e)
				{
					e->EditorInitialize();
				}
			}

			auto& conf = GetEditorConfig();

			if (stl::underlying(conf.lastConfigClass) < std::size(m_interfaces))
			{
				m_currentClass = conf.lastConfigClass;
			}
			else
			{
				m_currentClass = Data::ConfigClass::Global;
			}

			if (!GetInterface(m_currentClass))
			{
				auto i = Data::CONFIG_CLASS_MAX;

				while (i--)
				{
					if (auto& e = m_interfaces[i])
					{
						m_currentClass = static_cast<Data::ConfigClass>(i);
						break;
					}
				}
			}
		}

		void UIEditorTabPanel::Reset()
		{
			for (const auto& e : m_interfaces)
			{
				if (e)
				{
					e->EditorReset();
				}
			}
		}

		void UIEditorTabPanel::QueueUpdateCurrent()
		{
			for (const auto& e : m_interfaces)
			{
				if (e)
				{
					e->EditorQueueUpdateCurrent();
				}
			}
		}

		void UIEditorTabPanel::Draw()
		{
			if (ImGui::BeginTabBar(
					"gen_editor_panel",
					ImGuiTabBarFlags_NoTooltip |
						ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
			{
				auto i = Data::CONFIG_CLASS_MAX;

				while (i--)
				{
					auto& e = m_interfaces[i];

					if (!e)
					{
						continue;
					}

					ImGui::PushID(i);

					if (ImGui::BeginTabItem(UIL::LS(e.label, "_"), nullptr, e.flags))
					{
						if ((e.flags & ImGuiTabItemFlags_SetSelected) ==
						    ImGuiTabItemFlags_SetSelected)
						{
							e.flags &= ~ImGuiTabItemFlags_SetSelected;
						}

						EvaluateTabSwitch(static_cast<Data::ConfigClass>(i));

						ImGui::Spacing();

						const bool disabled = m_controller.IsDefaultConfigForced();

						if (disabled)
						{
							ImGui::TextColored(
								UICommon::g_colorWarning,
								UIL::LS(UIWidgetCommonStrings::DefaultConfForced));

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();
						}

						UICommon::PushDisabled(disabled);

						e->EditorDraw();

						UICommon::PopDisabled(disabled);

						ImGui::EndTabItem();
					}

					ImGui::PopID();
				}

				ImGui::EndTabBar();
			}
		}

		void UIEditorTabPanel::DrawMenuBarItems()
		{
			if (UIL::LCG_BM(CommonStrings::Actions, "et_mb"))
			{
				if (const auto& e = GetInterface(m_currentClass))
				{
					e->EditorDrawMenuBarItems();
				}

				ImGui::EndMenu();
			}
		}

		void UIEditorTabPanel::OnOpen()
		{
			if (auto& e = GetInterface(m_currentClass))
			{
				e->EditorOnOpen();
				e.flags |= ImGuiTabItemFlags_SetSelected;
			}
		}

		void UIEditorTabPanel::OnClose()
		{
			if (const auto& e = GetInterface(m_currentClass))
			{
				e->EditorOnClose();
			}

			StoreCurrentTab();
		}

		void UIEditorTabPanel::SetTabSelected(
			Data::ConfigClass a_class)
		{
			auto& e = GetInterface(a_class);
			e.flags |= ImGuiTabItemFlags_SetSelected;
		}

		void UIEditorTabPanel::EvaluateTabSwitch(
			Data::ConfigClass a_class)
		{
			const auto oldClass = m_currentClass;

			if (oldClass == a_class)
			{
				return;
			}

			m_currentClass = a_class;

			const auto& inew = GetInterface(a_class);
			const auto& iold = GetInterface(oldClass);

			if (iold)
			{
				iold->EditorOnClose();
			}

			if (inew)
			{
				inew->EditorOnOpen();
			}

			StoreCurrentTab();
		}

		void UIEditorTabPanel::StoreCurrentTab()
		{
			auto& conf = GetEditorConfig();

			if (m_currentClass != conf.lastConfigClass)
			{
				conf.lastConfigClass = m_currentClass;
				m_controller.GetSettings().mark_dirty();
			}
		}
	}
}