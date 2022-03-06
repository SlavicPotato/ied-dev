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
		{}

		void UIEditorTabPanel::Initialize()
		{
			for (auto& e : m_interfaces)
			{
				if (e.ptr)
				{
					e.ptr->Initialize();
				}
			}

			auto& conf = GetEditorConfig();

			if (stl::underlying(conf.lastConfigClass) < std::size(m_interfaces))
			{
				m_currentClass = conf.lastConfigClass;
			}

			if (!m_interfaces[stl::underlying(m_currentClass)].ptr)
			{
				using enum_type = std::underlying_type_t<Data::ConfigClass>;
				for (enum_type i = 0; i < Data::CONFIG_CLASS_MAX; i++)
				{
					auto& e = m_interfaces[i];
					if (e.ptr)
					{
						m_currentClass = static_cast<Data::ConfigClass>(i);
					}
				}
			}
		}

		void UIEditorTabPanel::Reset()
		{
			for (auto& e : m_interfaces)
			{
				if (e.ptr)
				{
					e.ptr->Reset();
				}
			}
		}

		void UIEditorTabPanel::QueueUpdateCurrent()
		{
			for (auto& e : m_interfaces)
			{
				if (e.ptr)
				{
					e.ptr->QueueUpdateCurrent();
				}
			}
		}

		void UIEditorTabPanel::Draw()
		{
			if (ImGui::BeginTabBar("gen_editor_panel"))
			{
				using enum_type = std::underlying_type_t<Data::ConfigClass>;

				enum_type i = std::size(m_interfaces);

				while (i)
				{
					i--;

					auto& e = m_interfaces[i];

					if (!e.ptr)
					{
						continue;
					}

					ImGui::PushID(i);

					if (ImGui::BeginTabItem(LS(e.label, "_"), nullptr, e.flags))
					{
						if ((e.flags & ImGuiTabItemFlags_SetSelected) ==
						    ImGuiTabItemFlags_SetSelected)
						{
							e.flags &= ~ImGuiTabItemFlags_SetSelected;
						}

						EvaluateTabSwitch(static_cast<Data::ConfigClass>(i));

						ImGui::Spacing();

						bool disabled = m_controller.IsDefaultConfigForced();

						if (disabled)
						{
							ImGui::TextColored(
								UICommon::g_colorWarning,
								LS(UIWidgetCommonStrings::DefaultConfForced));

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();
						}

						UICommon::PushDisabled(disabled);

						e.ptr->Draw();

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
			if (LCG_BM(CommonStrings::Actions, "et_mb"))
			{
				auto  i = stl::underlying(m_currentClass);
				auto& e = m_interfaces[i];

				if (e.ptr)
				{
					e.ptr->DrawMenuBarItems();
				}

				ImGui::EndMenu();
			}
		}

		void UIEditorTabPanel::OnOpen()
		{
			auto& e = m_interfaces[stl::underlying(m_currentClass)];

			if (e.ptr)
			{
				e.ptr->OnOpen();
				e.flags = ImGuiTabItemFlags_SetSelected;
			}
		}

		void UIEditorTabPanel::OnClose()
		{
			for (auto& e : m_interfaces)
			{
				if (e.ptr)
				{
					e.ptr->OnClose();
				}
			}
		}

		void UIEditorTabPanel::SetEditor(
			Data::ConfigClass      a_class,
			UIEditorInterface&     a_interface,
			Localization::StringID a_label)
		{
			m_interfaces[stl::underlying(a_class)] = {
				std::addressof(a_interface),
				a_label
			};
		}

		void UIEditorTabPanel::SetTabSelected(
			Data::ConfigClass a_class)
		{
			m_interfaces[stl::underlying(a_class)].flags =
				ImGuiTabItemFlags_SetSelected;
		}

		void UIEditorTabPanel::EvaluateTabSwitch(
			Data::ConfigClass a_class)
		{
			if (m_currentClass == a_class)
			{
				return;
			}

			auto oldClass = m_currentClass;

			m_currentClass = a_class;

			auto& conf = GetEditorConfig();

			conf.lastConfigClass = a_class;
			m_controller.GetConfigStore().settings.mark_dirty();

			const auto& inew = m_interfaces[stl::underlying(a_class)];
			const auto& iold = m_interfaces[stl::underlying(oldClass)];

			if (iold.ptr)
			{
				iold.ptr->OnClose();
			}

			if (inew.ptr)
			{
				inew.ptr->OnOpen();
			}
		}
	}
}