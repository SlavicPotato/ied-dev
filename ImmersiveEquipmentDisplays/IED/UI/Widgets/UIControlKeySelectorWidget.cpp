#include "pch.h"

#include "UIControlKeySelectorWidget.h"

namespace IED
{
	namespace UI
	{
		bool UIControlKeySelectorWidget::DrawKeySelector(
			const char*                      a_label,
			const UIData::controlDescData_t& a_data,
			std::uint32_t&                   a_key,
			bool                             a_allowNone)
		{
			char        buf[12];
			const char* preview;

			bool result = false;

			auto it = a_data.find(a_key);
			if (it != a_data.end())
			{
				preview = it->second;
			}
			else
			{
				stl::snprintf(buf, "0x%X", a_key);
				preview = buf;
			}

			ImGui::PushID("key_sel");

			if (ImGui::BeginCombo(
					a_label,
					preview,
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : a_data.getvec())
				{
					if (!a_allowNone && !e->first)
					{
						continue;
					}

					ImGui::PushID(e);

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
							a_key  = e->first;
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
	}
}
