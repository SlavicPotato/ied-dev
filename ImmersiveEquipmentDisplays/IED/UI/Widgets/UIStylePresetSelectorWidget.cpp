#include "pch.h"

#include "UIStylePresetSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		bool UIStylePresetSelectorWidget::DrawStylePresetSelector(
			const stl::vectormap<stl::fixed_string, StyleProfile>& a_data,
			stl::fixed_string&                                     a_preset)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Style, "ex_style_sel"),
					a_preset.c_str(),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : a_data.getvec())
				{
					ImGui::PushID(e->first.c_str());

					const bool selected = (e->first == a_preset);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							UIL::LMKID<2>(e->first.c_str(), "1"),
							selected))
					{
						a_preset = e->first;
						result   = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

	}
}