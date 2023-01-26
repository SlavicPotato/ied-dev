#include "pch.h"

#include "UIEffectShaderFunctionEditorWidget.h"

#include "UIEffectShaderEditorWidgetStrings.h"
#include "UIEffectShaderFunctionEditorWidgetStrings.h"

#include "IED/UI/UICommon.h"

#include <ext/stl_math.h>

namespace IED
{
	namespace UI
	{
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
					UIL::LS(CommonStrings::Angle, "1"),
					std::addressof(angle),
					dragSpeed1,
					-degmax,
					degmax,
					"%.2f",
					ImGuiSliderFlags_AlwaysClamp))
			{
				a_data.angle = angle * (pi / 180.0f);
				result       = true;
			}

			if (ImGui::DragFloat(
					UIL::LS(CommonStrings::Speed, "2"),
					std::addressof(a_data.speed),
					dragSpeed2,
					0.01f,
					10.0f,
					"%.2f"))
			{
				a_data.speed = std::clamp(a_data.speed, 0.01f, 100.0f);
				result       = true;
			}

			return result;
		}

		ESFEditorResult UIEffectShaderFunctionEditorWidget::DrawFunc_Pulse(
			Data::configEffectShaderFunction_t& a_data)
		{
			ESFEditorResult result;

			float dragSpeed = ImGui::GetIO().KeyShift ? 0.0005f : 0.05f;

			ImGui::Text("%s:", UIL::LS(CommonStrings::Waveform));
			ImGui::Spacing();

			ImGui::Columns(3, nullptr, false);

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Sine, "0"),
					a_data.fbf.type == Data::EffectShaderWaveform::Sine))
			{
				a_data.fbf.type = Data::EffectShaderWaveform::Sine;
				result          = true;
			}

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Cosine, "1"),
					a_data.fbf.type == Data::EffectShaderWaveform::Cosine))
			{
				a_data.fbf.type = Data::EffectShaderWaveform::Cosine;
				result          = true;
			}

			ImGui::NextColumn();

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Triangle, "2"),
					a_data.fbf.type == Data::EffectShaderWaveform::Triangle))
			{
				a_data.fbf.type = Data::EffectShaderWaveform::Triangle;
				result          = true;
			}

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Sawtooth, "3"),
					a_data.fbf.type == Data::EffectShaderWaveform::Sawtooth))
			{
				a_data.fbf.type = Data::EffectShaderWaveform::Sawtooth;
				result          = true;
			}

			ImGui::NextColumn();

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Square, "4"),
					a_data.fbf.type == Data::EffectShaderWaveform::Square))
			{
				a_data.fbf.type = Data::EffectShaderWaveform::Square;
				result          = true;
			}

			ImGui::Columns();

			ImGui::Spacing();

			result |= ImGui::DragFloatRange2(
				UIL::LS(UIEffectShaderFunctionEditorWidgetStrings::Range, "R"),
				std::addressof(a_data.range[0]),
				std::addressof(a_data.range[1]),
				0.0005f,
				0.0f,
				1.0f,
				"%.3f",
				nullptr,
				ImGuiSliderFlags_AlwaysClamp);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Exponential, "5"),
				stl::underlying(std::addressof(a_data.flags.value)),
				stl::underlying(Data::EffectShaderFunctionFlags::kExponential));

			if (a_data.flags.test(Data::EffectShaderFunctionFlags::kExponential))
			{
				ImGui::Indent();
				ImGui::Spacing();

				bool warn = stl::is_equal(a_data.exponent, 0.0f);

				if (warn)
				{
					ImGui::PushStyleColor(
						ImGuiCol_Text,
						UICommon::g_colorError);
				}

				result |= ImGui::SliderFloat(
					UIL::LS(CommonStrings::Exponent, "6"),
					std::addressof(a_data.exponent),
					-20.0f,
					20.0f,
					"%.1f",
					ImGuiSliderFlags_AlwaysClamp);

				if (warn)
				{
					ImGui::PopStyleColor();
				}

				ImGui::Spacing();
				ImGui::Unindent();
			}

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::AdditiveInverse, "7"),
				stl::underlying(std::addressof(a_data.flags.value)),
				stl::underlying(Data::EffectShaderFunctionFlags::kAdditiveInverse));

			ImGui::Spacing();

			if (ImGui::DragFloat(
					UIL::LS(CommonStrings::Frequency, "8"),
					std::addressof(a_data.speed),
					dragSpeed,
					0.01f,
					60.0f,
					"%.2f Hz"))
			{
				a_data.speed = std::clamp(a_data.speed, 0.01f, 300.0f);
				result       = true;
			}

			if (ImGui::DragFloat(
					UIL::LS(CommonStrings::Offset, "9"),
					std::addressof(a_data.initpos),
					0.0005f,
					0.0f,
					1.0f,
					"%.2f",
					ImGuiSliderFlags_AlwaysClamp))
			{
				result       = true;
				result.reset = true;
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("%s:", UIL::LS(UIEffectShaderEditorWidgetStrings::FillColor));

			ImGui::Indent();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Red, "A"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kFillR));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Green, "B"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kFillG));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Blue, "C"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kFillB));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Alpha, "D"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kFillA));

			ImGui::Unindent();

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("%s:", UIL::LS(UIEffectShaderEditorWidgetStrings::RimColor));

			ImGui::Indent();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Red, "E"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kRimR));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Green, "F"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kRimG));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Blue, "G"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kRimB));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Alpha, "H"),
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::kRimA));

			ImGui::Unindent();

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text(
				"%s:",
				UIL::LS(UIEffectShaderFunctionEditorWidgetStrings::uvScale));

			ImGui::Indent();

			result |= ImGui::CheckboxFlagsT(
				"##J",
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::uScale));

			bool uvdisabled = (a_data.pulseFlags & Data::EffectShaderPulseFlags::uScale) != Data::EffectShaderPulseFlags::uScale;

			UICommon::PushDisabled(uvdisabled);

			ImGui::SameLine();

			result |= ImGui::DragFloatRange2(
				UIL::LS(UIEffectShaderFunctionEditorWidgetStrings::uMinMax, "K"),
				std::addressof(a_data.uMinMax[0]),
				std::addressof(a_data.uMinMax[1]),
				dragSpeed,
				0.0f,
				1000.0f,
				"%.3f",
				nullptr,
				ImGuiSliderFlags_AlwaysClamp);

			UICommon::PopDisabled(uvdisabled);

			result |= ImGui::CheckboxFlagsT(
				"##L",
				stl::underlying(std::addressof(a_data.pulseFlags)),
				stl::underlying(Data::EffectShaderPulseFlags::vScale));

			uvdisabled = (a_data.pulseFlags & Data::EffectShaderPulseFlags::vScale) != Data::EffectShaderPulseFlags::vScale;

			UICommon::PushDisabled(uvdisabled);

			ImGui::SameLine();

			result |= ImGui::DragFloatRange2(
				UIL::LS(UIEffectShaderFunctionEditorWidgetStrings::vMinMax, "M"),
				std::addressof(a_data.vMinMax[0]),
				std::addressof(a_data.vMinMax[1]),
				dragSpeed,
				0.0f,
				1000.0f,
				"%.3f",
				nullptr,
				ImGuiSliderFlags_AlwaysClamp);

			UICommon::PopDisabled(uvdisabled);

			ImGui::Unindent();

			return result;
		}
	}
}