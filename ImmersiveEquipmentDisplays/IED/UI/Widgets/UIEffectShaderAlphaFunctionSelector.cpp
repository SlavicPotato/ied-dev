#include "pch.h"

#include "UIEffectShaderAlphaFunctionSelector.h"

namespace IED
{
	namespace UI
	{
		static constexpr auto s_data = stl::make_array(

			std::make_pair(NiProperty::AlphaFunction::kOne, "One"),
			std::make_pair(NiProperty::AlphaFunction::kZero, "Zero"),
			std::make_pair(NiProperty::AlphaFunction::kSrcColor, "SrcColor"),
			std::make_pair(NiProperty::AlphaFunction::kInvSrcColor, "InvSrcColor"),
			std::make_pair(NiProperty::AlphaFunction::kDestColor, "DestColor"),
			std::make_pair(NiProperty::AlphaFunction::kInvDestColor, "InvDestColor"),
			std::make_pair(NiProperty::AlphaFunction::kSrcAlpha, "SrcAlpha"),
			std::make_pair(NiProperty::AlphaFunction::kInvSrcAlpha, "InvSrcAlpha"),
			std::make_pair(NiProperty::AlphaFunction::kDestAlpha, "DestAlpha"),
			std::make_pair(NiProperty::AlphaFunction::kInvDestAlpha, "InvDestAlpha"),
			std::make_pair(NiProperty::AlphaFunction::kSrcAlphaSat, "SrcAlphaSat")

		);

		UIEffectShaderAlphaFunctionSelector::UIEffectShaderAlphaFunctionSelector(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIEffectShaderAlphaFunctionSelector::DrawAlphaFuncSelector(
			Localization::StringID     a_label,
			NiProperty::AlphaFunction& a_data)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(a_label, "afn_sel"),
					alpha_func_to_desc(a_data),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& [i, e] : s_data)
				{
					ImGui::PushID(stl::underlying(i));

					bool selected = (i == a_data);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							e,
							selected))
					{
						a_data = i;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UIEffectShaderAlphaFunctionSelector::alpha_func_to_desc(
			NiProperty::AlphaFunction a_data) const
		{
			switch (a_data)
			{
			case NiProperty::AlphaFunction::kOne:
				return "One";
			case NiProperty::AlphaFunction::kZero:
				return "Zero";
			case NiProperty::AlphaFunction::kSrcColor:
				return "SrcColor";
			case NiProperty::AlphaFunction::kInvSrcColor:
				return "InvSrcColor";
			case NiProperty::AlphaFunction::kDestColor:
				return "DestColor";
			case NiProperty::AlphaFunction::kInvDestColor:
				return "InvDestColor";
			case NiProperty::AlphaFunction::kSrcAlpha:
				return "SrcAlpha";
			case NiProperty::AlphaFunction::kInvSrcAlpha:
				return "InvSrcAlpha";
			case NiProperty::AlphaFunction::kDestAlpha:
				return "DestAlpha";
			case NiProperty::AlphaFunction::kInvDestAlpha:
				return "InvDestAlpha";
			case NiProperty::AlphaFunction::kSrcAlphaSat:
				return "SrcAlphaSat";
			default:
				return nullptr;
			}
		}
	}
}