#include "pch.h"

#include "UIControlKeySelectorWidget.h"

namespace IED
{
	namespace UI
	{
		bool UIControlKeySelectorWidget::DrawKeySelector(
			const char* a_label,
			const UIData::controlDescData_t& a_data,
			std::uint32_t& a_key)
		{
			char buf[9];
			const char* curSelName;

			bool result = false;

			auto it = a_data.find(a_key);
			if (it != a_data.end())
			{
				curSelName = it->second;
			}
			else
			{
				stl::snprintf(buf, "%.2X", a_key);
				curSelName = buf;
			}

			ImGui::PushID("key_sel");

			if (ImGui::BeginCombo(
					a_label,
					curSelName,
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : a_data.getvec())
				{
					ImGui::PushID(e->first);

					bool selected = e->first == a_key;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
						{
							ImGui::SetScrollHereY();
						}
					}

					if (ImGui::Selectable(e->second, selected))
					{
						if (a_key != e->first)
						{
							a_key = e->first;
							result = true;
						}
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			ImGui::PopID();

			return result;
		}
	}  // namespace UI
}  // namespace IED
