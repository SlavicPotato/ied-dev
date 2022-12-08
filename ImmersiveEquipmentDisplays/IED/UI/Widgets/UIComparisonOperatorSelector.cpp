#include "pch.h"

#include "UIComparisonOperatorSelector.h"

namespace IED
{
	namespace UI
	{
		static constexpr auto s_data = stl::make_array(

			std::make_pair(Data::ComparisonOperator::kEqual, UIComparisonOperatorSelectorStrings::Equal),
			std::make_pair(Data::ComparisonOperator::kNotEqual, UIComparisonOperatorSelectorStrings::NotEqual),
			std::make_pair(Data::ComparisonOperator::kGreater, UIComparisonOperatorSelectorStrings::Greater),
			std::make_pair(Data::ComparisonOperator::kLower, UIComparisonOperatorSelectorStrings::Lower),
			std::make_pair(Data::ComparisonOperator::kGreaterOrEqual, UIComparisonOperatorSelectorStrings::GreaterOrEqual),
			std::make_pair(Data::ComparisonOperator::kLowerOrEqual, UIComparisonOperatorSelectorStrings::LowerOrEqual)

		);

		bool UIComparisonOperatorSelector::DrawComparisonOperatorSelector(
			Data::ComparisonOperator& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					"##ex_co_sel",
					comp_operator_to_desc(a_type),
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
							UIL::LS<UIComparisonOperatorSelectorStrings, 3>(e, "1"),
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
				return UIL::LS(UIComparisonOperatorSelectorStrings::Equal);
			case Data::ComparisonOperator::kNotEqual:
				return UIL::LS(UIComparisonOperatorSelectorStrings::NotEqual);
			case Data::ComparisonOperator::kGreater:
				return UIL::LS(UIComparisonOperatorSelectorStrings::Greater);
			case Data::ComparisonOperator::kLower:
				return UIL::LS(UIComparisonOperatorSelectorStrings::Lower);
			case Data::ComparisonOperator::kGreaterOrEqual:
				return UIL::LS(UIComparisonOperatorSelectorStrings::GreaterOrEqual);
			case Data::ComparisonOperator::kLowerOrEqual:
				return UIL::LS(UIComparisonOperatorSelectorStrings::LowerOrEqual);
			default:
				return nullptr;
			}
		}
	}
}