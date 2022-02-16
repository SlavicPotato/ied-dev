#include "pch.h"

#include "UIStylePresetSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIStylePresetSelectorWidget::data_type UIStylePresetSelectorWidget::m_data{ {

			UIStylePresetSelectorWidgetStrings::Dark,
			UIStylePresetSelectorWidgetStrings::Light,
			UIStylePresetSelectorWidgetStrings::Classic,
			UIStylePresetSelectorWidgetStrings::DeepDark,
			UIStylePresetSelectorWidgetStrings::DarkRed,
			UIStylePresetSelectorWidgetStrings::SteamClassic,
			UIStylePresetSelectorWidgetStrings::ItaDark,
			UIStylePresetSelectorWidgetStrings::ItaLight,
			UIStylePresetSelectorWidgetStrings::S56,
			UIStylePresetSelectorWidgetStrings::CorpGrey,
			UIStylePresetSelectorWidgetStrings::CorpGreyFlat,

		} };

		UIStylePresetSelectorWidget::UIStylePresetSelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIStylePresetSelectorWidget::DrawStylePresetSelector(
			UIStylePreset& a_preset)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(CommonStrings::Style, "ex_style_sel"),
					preset_to_desc(a_preset),
					ImGuiComboFlags_HeightLarge))
			{
				using enum_type = std::underlying_type_t<UIStylePreset>;

				for (enum_type i = 0; i < m_data.size(); i++)
				{
					auto e = static_cast<UIStylePreset>(i);

					ImGui::PushID(i);

					bool selected = (e == a_preset);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							LS<UIStylePresetSelectorWidgetStrings, 3>(m_data[i], "1"),
							selected))
					{
						a_preset = e;
						result   = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UIStylePresetSelectorWidget::preset_to_desc(
			UIStylePreset a_preset)
		{
			static_assert(!std::is_signed_v<
						  std::underlying_type_t<UIStylePreset>>);

			if (stl::underlying(a_preset) < m_data.size())
			{
				return LS(m_data[stl::underlying(a_preset)]);
			}
			else
			{
				return nullptr;
			}
		}
	}
}