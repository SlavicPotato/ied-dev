#include "pch.h"

#include "UILightingTemplateInheritanceFlagsWidget.h"

namespace IED
{
	namespace UI
	{
		static constexpr auto s_data = stl::make_array(

			std::make_pair(RE::INTERIOR_DATA::Inherit::kAmbientColor, UILightingTemplateInheritanceFlagsWidgetStrings::AmbientColor),
			std::make_pair(RE::INTERIOR_DATA::Inherit::kDirectionalColor, UILightingTemplateInheritanceFlagsWidgetStrings::DirectionalColor),
			std::make_pair(RE::INTERIOR_DATA::Inherit::kFogColor, UILightingTemplateInheritanceFlagsWidgetStrings::FogColor),
			std::make_pair(RE::INTERIOR_DATA::Inherit::kFogNear, UILightingTemplateInheritanceFlagsWidgetStrings::FogNear),
			std::make_pair(RE::INTERIOR_DATA::Inherit::kFogFar, UILightingTemplateInheritanceFlagsWidgetStrings::FogFar),
			std::make_pair(RE::INTERIOR_DATA::Inherit::kDirectionalRotation, UILightingTemplateInheritanceFlagsWidgetStrings::DirectionalRotation),
			std::make_pair(RE::INTERIOR_DATA::Inherit::kDirectionalFade, UILightingTemplateInheritanceFlagsWidgetStrings::DirectionalFade),
			std::make_pair(RE::INTERIOR_DATA::Inherit::kClipDistance, UILightingTemplateInheritanceFlagsWidgetStrings::ClipDistance),
			std::make_pair(RE::INTERIOR_DATA::Inherit::kFogPower, UILightingTemplateInheritanceFlagsWidgetStrings::FogPower),
			std::make_pair(RE::INTERIOR_DATA::Inherit::kFogMax, UILightingTemplateInheritanceFlagsWidgetStrings::FogMax),
			std::make_pair(RE::INTERIOR_DATA::Inherit::kLightFadeDistances, UILightingTemplateInheritanceFlagsWidgetStrings::LightFadeDistances)

		);

		bool UILightingTemplateInheritanceFlagsWidget::DrawLightingTemplateInheritanceFlags(
			RE::INTERIOR_DATA::Inherit& a_flags) const
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(UILightingTemplateInheritanceFlagsWidgetStrings::ComboLabel, "ex_clti_flg"),
					lt_inheritance_flags_to_desc(a_flags),
					ImGuiComboFlags_HeightLarge))
			{
				ImGui::PushID("1");

				if (ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::Any, "1"),
						stl::underlying(std::addressof(a_flags)),
						stl::underlying(RE::INTERIOR_DATA::Inherit::kAll)))
				{
					result = true;
				}

				ImGui::PopID();

				ImGui::Separator();

				ImGui::PushID("2");

				for (auto& [i, e] : s_data)
				{
					ImGui::PushID(stl::underlying(i));

					if (ImGui::CheckboxFlagsT(
							UIL::LS<UILightingTemplateInheritanceFlagsWidgetStrings, 3>(e, "1"),
							stl::underlying(std::addressof(a_flags)),
							stl::underlying(i)))
					{
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::PopID();

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UILightingTemplateInheritanceFlagsWidget::lt_inheritance_flags_to_desc(
			stl::flag<RE::INTERIOR_DATA::Inherit> a_flags) const
		{
			if (!a_flags.test_any(RE::INTERIOR_DATA::Inherit::kAll))
			{
				return UIL::LS(CommonStrings::None);
			}

			if (a_flags.test(RE::INTERIOR_DATA::Inherit::kAll))
			{
				return UIL::LS(CommonStrings::Any);
			}

			m_buf.clear();

			for (auto& e : s_data)
			{
				if (a_flags.test(e.first))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}

					m_buf += UIL::L(e.second);
				}
			}

			if (m_buf.empty())
			{
				return UIL::LS(CommonStrings::None);
			}

			return m_buf.c_str();
		}
	}
}