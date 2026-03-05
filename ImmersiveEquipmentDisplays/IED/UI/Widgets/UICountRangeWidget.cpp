#include "pch.h"

#include "UICountRangeWidget.h"

#include "UIPopupToggleButtonWidget.h"
#include "UIWidgetCommonStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		bool UICountRangeWidget::DrawCountRange(
			Data::configRange_t& a_configRange)
		{
			bool result = false;

			ImGui::PushID("cr_main");

			result |= DrawCountRangeContextMenu(a_configRange);

			const Data::configRange_t lim{
				0,
				UINT32_MAX
			};

			if (ImGui::DragScalarN(
					UIL::LS(CommonStrings::Limits, "A"),
					ImGuiDataType_U32,
					std::addressof(a_configRange),
					2,
					0.175f,
					std::addressof(lim.min),
					std::addressof(lim.max),
					"%u",
					ImGuiSliderFlags_AlwaysClamp))
			{
				result = true;
			}

			ImGui::PopID();

			return result;
		}


		bool UICountRangeWidget::DrawCountRangeContextMenu(
			Data::configRange_t& a_configRange)
		{
			bool result = false;

			ImGui::PushID("cr_context_area");

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");
			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem(UIL::LS(UIWidgetCommonStrings::ResetToZero, "1")))
				{
					a_configRange = {};

					result = true;
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}
	}
}