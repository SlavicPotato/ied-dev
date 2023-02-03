#include "pch.h"

#include "UIExtraLightEditorWidget.h"

#include "IED/ConfigExtraLight.h"

#include "IED/UI/UILocalizationInterface.h"
#include "IED/UI/UITips.h"

namespace IED
{
	namespace UI
	{
		bool UIExtraLightEditorWidget::DrawExtraLightEditor(
			Data::configExtraLight_t& a_data)
		{
			const bool result = DrawImpl(a_data.data);

			if (result)
			{
				a_data.update_tag();
			}

			return result;
		}

		bool UIExtraLightEditorWidget::DrawImpl(
			Data::ExtraLightData& a_data)
		{
			bool result = false;

			ImGui::Columns(2, nullptr, false);

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIExtraLightEditorWidgetStrings::TargetSelf, "1"),
				stl::underlying(std::addressof(a_data.flags.value)),
				stl::underlying(Data::ExtraLightFlags::kTargetSelf));

			UITipsInterface::DrawTip(UITip::LightTargetSelf);

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIExtraLightEditorWidgetStrings::DontLightWater, "2"),
				stl::underlying(std::addressof(a_data.flags.value)),
				stl::underlying(Data::ExtraLightFlags::kDontLightWater));

			ImGui::NextColumn();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIExtraLightEditorWidgetStrings::DontLightLand, "3"),
				stl::underlying(std::addressof(a_data.flags.value)),
				stl::underlying(Data::ExtraLightFlags::kDontLightLandscape));

			ImGui::Columns();

			ImGui::Spacing();

			result |= ImGui::InputScalar(
				UIL::LS(UIExtraLightEditorWidgetStrings::FieldOfView, "4"),
				ImGuiDataType_Float,
				std::addressof(a_data.fieldOfView),
				nullptr,
				nullptr,
				"%f",
				ImGuiInputTextFlags_EnterReturnsTrue);

			UITipsInterface::DrawTip(UITip::LightFieldOfView);

			result |= ImGui::InputScalar(
				UIL::LS(UIExtraLightEditorWidgetStrings::ShadowDepthBias, "5"),
				ImGuiDataType_Float,
				std::addressof(a_data.shadowDepthBias),
				nullptr,
				nullptr,
				"%f",
				ImGuiInputTextFlags_EnterReturnsTrue);

			return result;
		}
	}

}
