#pragma once

#include "IED/UI/UICommon.h"

#include "IED/NodeOverrideData.h"

namespace IED
{
	namespace UI
	{
		class UICMNodeSelectorWidget
		{
		public:
			template <class T>
			static bool DrawCMNodeSelector(
				const char*                           a_label,
				stl::fixed_string&                    a_current,
				const T&                              a_data,
				const stl::fixed_string*              a_skip       = nullptr,
				const NodeOverrideData::cm_data_type* a_descLookup = nullptr);
		};

		template <class T>
		bool UICMNodeSelectorWidget::DrawCMNodeSelector(
			const char*                           a_label,
			stl::fixed_string&                    a_current,
			const T&                              a_data,
			const stl::fixed_string*              a_skip,
			const NodeOverrideData::cm_data_type* a_descLookup)
		{
			const char* descName{ nullptr };

			if (!a_current.empty())
			{
				if (a_descLookup)
				{
					if (auto it = a_descLookup->find(a_current);
					    it != a_descLookup->end())
					{
						descName = it->second.desc.c_str();
					}
				}
				else
				{
					if (auto it = a_data.find(a_current);
					    it != a_data.end())
					{
						if constexpr (std::is_same_v<T, NodeOverrideData::cm_data_type>)
						{
							descName = it->second.desc.c_str();
						}
						else
						{
							descName = it->second.desc;
						}
					}
				}

				if (!descName)
				{
					descName = a_current.c_str();
				}
			}

			bool result = false;

			if (ImGui::BeginCombo(a_label, descName, ImGuiComboFlags_HeightLarge))
			{
				for (auto e : a_data.getvec())
				{
					if (a_skip && e->first == *a_skip)
					{
						continue;
					}

					ImGui::PushID(e);

					bool selected = (e->first == a_current);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					const char* desc;

					if constexpr (std::is_same_v<T, NodeOverrideData::cm_data_type>)
					{
						desc = e->second.desc.c_str();
					}
					else
					{
						desc = e->second.desc;
					}

					if (ImGui::Selectable(desc, selected))
					{
						a_current = e->first;
						result    = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}
	}
}