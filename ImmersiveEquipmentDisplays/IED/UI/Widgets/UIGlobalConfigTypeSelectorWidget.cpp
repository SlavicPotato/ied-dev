#include "pch.h"

#include "UIGlobalConfigTypeSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		bool UIGlobalConfigTypeSelectorWidget::DrawTypeSelectorRadio(
			Data::GlobalConfigType& a_value)
		{
			bool result = false;

			ImGui::PushID("type_selector");

			if (ImGui::RadioButton(
					"Player",
					a_value == Data::GlobalConfigType::Player))
			{
				a_value = Data::GlobalConfigType::Player;
				result = true;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton(
					"NPCs",
					a_value == Data::GlobalConfigType::NPC))
			{
				a_value = Data::GlobalConfigType::NPC;
				result = true;
			}

			ImGui::PopID();

			return result;
		}
	}
}