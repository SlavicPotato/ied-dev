#include "pch.h"

#include "UIComparisonOperatorSelector.h"

namespace IED
{
	namespace UI
	{
		UIComparisonOperatorSelector::data_type UIComparisonOperatorSelector::m_data{ {

			{ Data::ExtraComparisonOperator::kEqual, UIComparisonOperatorSelectorStrings::Equal },
			{ Data::ExtraComparisonOperator::kNotEqual, UIComparisonOperatorSelectorStrings::NotEqual },
			{ Data::ExtraComparisonOperator::kGreater, UIComparisonOperatorSelectorStrings::Greater },
			{ Data::ExtraComparisonOperator::kLower, UIComparisonOperatorSelectorStrings::Lower },
			{ Data::ExtraComparisonOperator::kGreaterOrEqual, UIComparisonOperatorSelectorStrings::GreaterOrEqual },
			{ Data::ExtraComparisonOperator::kLowerOrEqual, UIComparisonOperatorSelectorStrings::LowerOrEqual },

		} };

		UIComparisonOperatorSelector::UIComparisonOperatorSelector(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIComparisonOperatorSelector::DrawComparisonOperatorSelector(
			Data::ExtraComparisonOperator& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					"##ex_co_sel",
					comp_operator_to_desc(a_type),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& [i, e] : m_data)
				{
					ImGui::PushID(stl::underlying(i));

					bool selected = (i == a_type);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							LS<UIComparisonOperatorSelectorStrings, 3>(e, "1"),
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

		const char* UIComparisonOperatorSelector::comp_operator_to_desc(
			Data::ExtraComparisonOperator a_comp)
		{
			switch (a_comp)
			{
			case Data::ExtraComparisonOperator::kEqual:
				return LS(UIComparisonOperatorSelectorStrings::Equal);
			case Data::ExtraComparisonOperator::kNotEqual:
				return LS(UIComparisonOperatorSelectorStrings::NotEqual);
			case Data::ExtraComparisonOperator::kGreater:
				return LS(UIComparisonOperatorSelectorStrings::Greater);
			case Data::ExtraComparisonOperator::kLower:
				return LS(UIComparisonOperatorSelectorStrings::Lower);
			case Data::ExtraComparisonOperator::kGreaterOrEqual:
				return LS(UIComparisonOperatorSelectorStrings::GreaterOrEqual);
			case Data::ExtraComparisonOperator::kLowerOrEqual:
				return LS(UIComparisonOperatorSelectorStrings::LowerOrEqual);
			default:
				return nullptr;
			}
		}
	}
}