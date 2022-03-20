#include "pch.h"

#include "UIComparisonOperatorSelector.h"

namespace IED
{
	namespace UI
	{
		UIComparisonOperatorSelector::data_type UIComparisonOperatorSelector::m_data{ {

			{ Data::ComparisonOperator::kEqual, UIComparisonOperatorSelectorStrings::Equal },
			{ Data::ComparisonOperator::kNotEqual, UIComparisonOperatorSelectorStrings::NotEqual },
			{ Data::ComparisonOperator::kGreater, UIComparisonOperatorSelectorStrings::Greater },
			{ Data::ComparisonOperator::kLower, UIComparisonOperatorSelectorStrings::Lower },
			{ Data::ComparisonOperator::kGreaterOrEqual, UIComparisonOperatorSelectorStrings::GreaterOrEqual },
			{ Data::ComparisonOperator::kLowerOrEqual, UIComparisonOperatorSelectorStrings::LowerOrEqual },

		} };

		UIComparisonOperatorSelector::UIComparisonOperatorSelector(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIComparisonOperatorSelector::DrawComparisonOperatorSelector(
			Data::ComparisonOperator& a_type)
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
			Data::ComparisonOperator a_comp)
		{
			switch (a_comp)
			{
			case Data::ComparisonOperator::kEqual:
				return LS(UIComparisonOperatorSelectorStrings::Equal);
			case Data::ComparisonOperator::kNotEqual:
				return LS(UIComparisonOperatorSelectorStrings::NotEqual);
			case Data::ComparisonOperator::kGreater:
				return LS(UIComparisonOperatorSelectorStrings::Greater);
			case Data::ComparisonOperator::kLower:
				return LS(UIComparisonOperatorSelectorStrings::Lower);
			case Data::ComparisonOperator::kGreaterOrEqual:
				return LS(UIComparisonOperatorSelectorStrings::GreaterOrEqual);
			case Data::ComparisonOperator::kLowerOrEqual:
				return LS(UIComparisonOperatorSelectorStrings::LowerOrEqual);
			default:
				return nullptr;
			}
		}
	}
}