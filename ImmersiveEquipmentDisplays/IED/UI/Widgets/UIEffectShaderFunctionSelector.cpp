#include "pch.h"

#include "UIEffectShaderFunctionSelector.h"

namespace IED
{
	namespace UI
	{
		static constexpr auto s_data = stl::make_array(

			std::make_pair(Data::EffectShaderFunctionType::UVLinearMotion, UIEffectShaderFunctionSelectorStrings::UVLinearMotion),
			std::make_pair(Data::EffectShaderFunctionType::Pulse, UIEffectShaderFunctionSelectorStrings::Pulse)

		);

		UIEffectShaderFunctionSelector::UIEffectShaderFunctionSelector(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIEffectShaderFunctionSelector::DrawEffectShaderFunctionSelector(
			Data::EffectShaderFunctionType& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					"##ex_esf_sel",
					esf_to_desc(a_type),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& [i, e] : s_data)
				{
					ImGui::PushID(stl::underlying(i));

					bool selected = (i == a_type);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							LS<UIEffectShaderFunctionSelectorStrings, 3>(e, "1"),
							selected))
					{
						a_type = i;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UIEffectShaderFunctionSelector::esf_to_desc(
			Data::EffectShaderFunctionType a_type) const
		{
			switch (a_type)
			{
			case Data::EffectShaderFunctionType::UVLinearMotion:
				return LS(UIEffectShaderFunctionSelectorStrings::UVLinearMotion);
			case Data::EffectShaderFunctionType::Pulse:
				return LS(UIEffectShaderFunctionSelectorStrings::Pulse);
			default:
				return nullptr;
			}
		}
	}
}