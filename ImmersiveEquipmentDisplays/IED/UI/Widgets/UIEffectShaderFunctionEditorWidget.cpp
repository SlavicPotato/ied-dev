#include "pch.h"

#include "UIEffectShaderFunctionEditorWidget.h"

#include "UIEffectShaderEditorWidgetStrings.h"
#include "UIEffectShaderFunctionEditorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		UIEffectShaderFunctionEditorWidget::UIEffectShaderFunctionEditorWidget(
			Localization::ILocalization& a_localization) :
			UIEffectShaderFunctionSelector(a_localization),
			UILocalizationInterface(a_localization)
		{
		}

		void UIEffectShaderFunctionEditorWidget::OpenEffectShaderFunctionEditor()
		{
			ImGui::OpenPopup(POPUP_ID);
		}

		ESFEditorResult UIEffectShaderFunctionEditorWidget::DrawEffectShaderFunction(
			Data::configEffectShaderFunction_t& a_data)
		{
			ESFEditorResult result;

			if (!ImGui::IsPopupOpen(POPUP_ID))
			{
				return result;
			}

			const float w = ImGui::GetFontSize() * 34.0f;

			ImGui::SetNextWindowSizeConstraints(
				{ w, 0.0f },
				{ w, 800.0f });

			if (ImGui::BeginPopup(POPUP_ID))
			{
				ImGui::PushItemWidth(ImGui::GetFontSize() * -6.0f);

				switch (a_data.type)
				{
				case Data::EffectShaderFunctionType::UVLinearMotion:
					result = DrawFunc_UVLinearMotion(a_data);
					break;
				case Data::EffectShaderFunctionType::Pulse:
					result = DrawFunc_Pulse(a_data);
					break;
				}

				ImGui::PopItemWidth();

				ImGui::EndPopup();
			}

			return result;
		}

		ESFEditorResult UIEffectShaderFunctionEditorWidget::DrawFunc_UVLinearMotion(
			Data::configEffectShaderFunction_t& a_data)
		{
			ESFEditorResult result;

			float dragSpeed1 = ImGui::GetIO().KeyShift ? 0.005f : 0.5f;
			float dragSpeed2 = ImGui::GetIO().KeyShift ? 0.0001f : 0.01f;

			constexpr auto pi     = std::numbers::pi_v<float>;
			constexpr auto degmax = (pi * 2.0f) * (180.0f / pi);

			float angle = a_data.angle * (180.0f / pi);

			if (ImGui::DragFloat(
					LS(CommonStrings::Angle, "1"),
					std::addressof(angle),
					dragSpeed1,
					-degmax,
					degmax,
					"%.2f",
					ImGuiSliderFlags_AlwaysClamp |
						ImGuiSliderFlags_NoRoundToFormat))
			{
				a_data.angle   = angle * (pi / 180.0f);
				result.changed = true;
			}

			if (ImGui::DragFloat(
					LS(CommonStrings::Speed, "2"),
					std::addressof(a_data.speed),
					dragSpeed2,
					0.01f,
					10.0f,
					"%.2f"))
			{
				a_data.speed   = std::clamp(a_data.speed, 0.01f, 100.0f);
				result.changed = true;
			}

			return result;
		}

		ESFEditorResult UIEffectShaderFunctionEditorWidget::DrawFunc_Pulse(
			Data::configEffectShaderFunction_t& a_data)
		{
			ESFEditorResult result;

			constexpr auto pi2 = std::numbers::pi_v<float> * 2.0f;

			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(LS(CommonStrings::Function));

			ImGui::SameLine();
			if (ImGui::RadioButton(
					LS(CommonStrings::Sine, "1"),
					!a_data.flags.test(Data::EffectShaderFunctionFlags::kOpt1)))
			{
				a_data.flags.clear(Data::EffectShaderFunctionFlags::kOpt1);
				result = true;
			}

			ImGui::SameLine();
			if (ImGui::RadioButton(
					LS(CommonStrings::Cosine, "2"),
					a_data.flags.test(Data::EffectShaderFunctionFlags::kOpt1)))
			{
				a_data.flags.set(Data::EffectShaderFunctionFlags::kOpt1);
				result = true;
			}

			ImGui::Spacing();

			float dragSpeed = ImGui::GetIO().KeyShift ? 0.0005f : 0.05f;

			if (ImGui::DragFloat(
					LS(CommonStrings::Speed, "3"),
					std::addressof(a_data.speed),
					dragSpeed,
					0.01f,
					50.0f,
					"%.2f"))
			{
				a_data.speed = std::clamp(a_data.speed, 0.01f, 100.0f);
				result       = true;
			}

			if (ImGui::DragFloat(
					LS(CommonStrings::Offset, "4"),
					std::addressof(a_data.initmod),
					dragSpeed,
					0.0f,
					pi2,
					"%.2f",
					ImGuiSliderFlags_AlwaysClamp))
			{
				result       = true;
				result.reset = true;
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("%s:", LS(UIEffectShaderEditorWidgetStrings::FillColor));

			ImGui::Indent();

			result |= ImGui::CheckboxFlagsT(
				LS(CommonStrings::Red, "A"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kFillR));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				LS(CommonStrings::Green, "B"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kFillG));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				LS(CommonStrings::Blue, "C"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kFillB));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				LS(CommonStrings::Alpha, "D"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kFillA));

			ImGui::Unindent();

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("%s:", LS(UIEffectShaderEditorWidgetStrings::RimColor));

			ImGui::Indent();

			result |= ImGui::CheckboxFlagsT(
				LS(CommonStrings::Red, "E"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kRimR));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				LS(CommonStrings::Green, "F"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kRimG));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				LS(CommonStrings::Blue, "G"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kRimB));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				LS(CommonStrings::Alpha, "H"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kRimA));

			ImGui::Unindent();

			return result;
		}
	}
}