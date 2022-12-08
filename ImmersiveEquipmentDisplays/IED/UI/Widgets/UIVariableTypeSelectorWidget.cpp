#include "pch.h"

#include "UIVariableTypeSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		static constexpr auto s_data = stl::make_array(

			std::make_pair(ConditionalVariableType::kInt32, UIVariableTypeSelectorWidgetStrings::i32),
			std::make_pair(ConditionalVariableType::kFloat, UIVariableTypeSelectorWidgetStrings::f32),
			std::make_pair(ConditionalVariableType::kForm, UIVariableTypeSelectorWidgetStrings::Form)

		);

		bool UIVariableTypeSelectorWidget::DrawVariableTypeSelectorWidget(
			ConditionalVariableType& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Type, "ex_cvt_sel"),
					variable_type_to_desc(a_type),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : s_data)
				{
					ImGui::PushID(stl::underlying(e.first));

					bool selected = (e.first == a_type);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							UIL::LS<UIVariableTypeSelectorWidgetStrings, 3>(e.second, "1"),
							selected))
					{
						a_type = e.first;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UIVariableTypeSelectorWidget::variable_type_to_desc(
			ConditionalVariableType a_type)
		{
			switch (a_type)
			{
			case ConditionalVariableType::kInt32:
				return UIL::LS(UIVariableTypeSelectorWidgetStrings::i32);
			case ConditionalVariableType::kFloat:
				return UIL::LS(UIVariableTypeSelectorWidgetStrings::f32);
			case ConditionalVariableType::kForm:
				return UIL::LS(UIVariableTypeSelectorWidgetStrings::Form);
			default:
				return nullptr;
			}
		}
	}
}