#include "pch.h"

#include "UINodeSelectorWidget.h"

#include "IED/Controller/Controller.h"

#include "IED/NodeMap.h"

namespace IED
{
	namespace UI
	{
		UINodeSelectorWidget::UINodeSelectorWidget(
			Controller& a_controller) :
			UILocalizationInterface(a_controller),
			m_controller(a_controller),
			m_filter(true)
		{
			//m_filter.SetFlags(ImGuiInputTextFlags_EnterReturnsTrue);
		}

		bool UINodeSelectorWidget::DrawNodeSelector(
			const char*           a_label,
			bool                  a_hideManaged,
			Data::NodeDescriptor& a_data)
		{
			const char* currentName = nullptr;

			auto& nodeMap = Data::NodeMap::GetSingleton();

			nodeMap.SortIfDirty();

			const auto& data = nodeMap.GetData();

			if (a_data)
			{
				if (auto it = data.find(a_data.name); it != data.end())
				{
					currentName = Data::NodeMap::GetNodeDescription(
						it->second.name.c_str(),
						it->second.flags,
						m_buffer);
				}
			}

			bool result = false;

			ImGui::PushID("node_selector");

			if (ImGui::BeginCombo(a_label, currentName, ImGuiComboFlags_HeightLarge))
			{
				ImGui::Dummy({ 0.0f, 0.0f });
				ImGui::SameLine(0.0f, 4.0f);

				m_filter.Draw();

				ImGui::Spacing();
				ImGui::Separator();

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

				if (ImGui::BeginChild("combo", { -1.0f, 250.0f }))
				{
					for (auto& e : data.getvec())
					{
						if (!m_filter.Test(*e->second.name))
						{
							continue;
						}

						ImGui::PushID(e);

						bool selected = (e->first == a_data.name);
						if (selected)
						{
							if (ImGui::IsWindowAppearing())
								ImGui::SetScrollHereY();
						}

						auto desc = Data::NodeMap::GetNodeDescription(
							e->second.name.c_str(),
							e->second.flags,
							m_buffer);

						auto flags = a_hideManaged && e->second.managed() ?
						                 ImGuiSelectableFlags_Disabled :
                                         ImGuiSelectableFlags_None;

						if (ImGui::Selectable(LMKID<3>(desc, "1"), selected, flags))
						{
							a_data.name  = e->first;
							a_data.flags = e->second.flags;
							result       = true;

							ImGui::CloseCurrentPopup();
						}

						ImGui::PopID();
					}
				}
				ImGui::EndChild();

				ImGui::PopStyleVar();

				ImGui::EndCombo();
			}

			ImGui::PopID();

			return result;
		}

	}
}