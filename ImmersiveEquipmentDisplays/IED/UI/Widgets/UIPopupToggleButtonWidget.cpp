#include "pch.h"

#include "UIPopupToggleButtonWidget.h"

namespace IED
{
	namespace UI
	{
		bool UIPopupToggleButtonWidget::DrawPopupToggleButton(
			const char* a_id,
			const char* a_popupId)
		{
			const auto dir = ImGui::IsPopupOpen(a_popupId) ?
                                     ImGuiDir_Down :
                                     ImGuiDir_Right;

			bool result = ImGui::ArrowButton(a_id, dir);

			if (result)
			{
				ImGui::OpenPopup(a_popupId);
			}

			return result;
		}

		bool UIPopupToggleButtonWidget::DrawPopupToggleButtonNoOpen(
			const char* a_id,
			const char* a_popupId)
		{

			const auto dir = ImGui::IsPopupOpen(a_popupId) ?
                                 ImGuiDir_Down :
                                 ImGuiDir_Right;

			return ImGui::ArrowButton(a_id, dir);
		}
	}
}