#include "pch.h"

#include "UIVariableTypeSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIVariableTypeSelectorWidget::data_type UIVariableTypeSelectorWidget::m_data{ {

			{ ConditionalVariableType::kInt32, UIVariableTypeSelectorWidgetStrings::i32 },
			{ ConditionalVariableType::kFloat, UIVariableTypeSelectorWidgetStrings::f32 },
			{ ConditionalVariableType::kForm, UIVariableTypeSelectorWidgetStrings::Form },

		} };

		UIVariableTypeSelectorWidget::UIVariableTypeSelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIVariableTypeSelectorWidget::DrawVariableTypeSelectorWidget(
			ConditionalVariableType& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(CommonStrings::Type, "ex_cvt_sel"),
					variable_type_to_desc(a_type),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : m_data)
				{
					ImGui::PushID(stl::underlying(e.first));

					bool selected = (e.first == a_type);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							LS<UIVariableTypeSelectorWidgetStrings, 3>(e.second, "1"),
							selected))
					{
						a_type = e.first;
						result   = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UIVariableTypeSelectorWidget::variable_type_to_desc(
			ConditionalVariableType a_type) const
		{
			switch (a_type)
			{
			case ConditionalVariableType::kInt32:
				return LS(UIVariableTypeSelectorWidgetStrings::i32);
			case ConditionalVariableType::kFloat:
				return LS(UIVariableTypeSelectorWidgetStrings::f32);
			case ConditionalVariableType::kForm:
				return LS(UIVariableTypeSelectorWidgetStrings::Form);
			default:
				return nullptr;
			}
		}
	}
}