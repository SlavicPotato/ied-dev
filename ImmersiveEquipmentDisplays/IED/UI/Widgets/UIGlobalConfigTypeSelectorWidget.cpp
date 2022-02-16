#include "pch.h"

#include "UIGlobalConfigTypeSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIGlobalConfigTypeSelectorWidget::UIGlobalConfigTypeSelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIGlobalConfigTypeSelectorWidget::DrawTypeSelectorRadio(
			Data::GlobalConfigType& a_value)
		{
			bool result = false;

			ImGui::PushID("type_selector");

			if (ImGui::RadioButton(
					LS(CommonStrings::Player, "1"),
					a_value == Data::GlobalConfigType::Player))
			{
				a_value = Data::GlobalConfigType::Player;
				result  = true;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton(
					LS(CommonStrings::NPC, "2"),
					a_value == Data::GlobalConfigType::NPC))
			{
				a_value = Data::GlobalConfigType::NPC;
				result  = true;
			}

			ImGui::PopID();

			return result;
		}
	}
}