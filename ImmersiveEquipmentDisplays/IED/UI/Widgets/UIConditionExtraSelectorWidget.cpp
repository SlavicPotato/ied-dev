#include "pch.h"

#include "UIConditionExtraSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIConditionExtraSelectorWidget::data_type UIConditionExtraSelectorWidget::m_data{ {

			{ Data::ExtraConditionType::kCanDualWield, UIConditionExtraSelectorWidgetStrings::CanDualWield },
			{ Data::ExtraConditionType::kIsDead, UIConditionExtraSelectorWidgetStrings::IsDead },
			{ Data::ExtraConditionType::kInInterior, UIConditionExtraSelectorWidgetStrings::InInterior },

		} };

		UIConditionExtraSelectorWidget::UIConditionExtraSelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIConditionExtraSelectorWidget::DrawExtraConditionSelector(
			Data::ExtraConditionType& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(CommonStrings::Type, "ex_cond_sel"),
					condition_type_to_desc(a_type),
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
							LS<UIConditionExtraSelectorWidgetStrings, 3>(e, "1"),
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

		const char* UIConditionExtraSelectorWidget::condition_type_to_desc(
			Data::ExtraConditionType a_type)
		{
			switch (a_type)
			{
			case Data::ExtraConditionType::kCanDualWield:
				return LS(UIConditionExtraSelectorWidgetStrings::CanDualWield);
			case Data::ExtraConditionType::kIsDead:
				return LS(UIConditionExtraSelectorWidgetStrings::IsDead);
			case Data::ExtraConditionType::kInInterior:
				return LS(UIConditionExtraSelectorWidgetStrings::InInterior);
			default:
				return nullptr;
			}
		}
	}
}