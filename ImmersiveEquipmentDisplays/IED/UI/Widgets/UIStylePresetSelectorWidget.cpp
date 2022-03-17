#include "pch.h"

#include "UIStylePresetSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIStylePresetSelectorWidget::data_type UIStylePresetSelectorWidget::m_data{ {

			{ UIStylePreset::Dark, UIStylePresetSelectorWidgetStrings::Dark },
			{ UIStylePreset::Light, UIStylePresetSelectorWidgetStrings::Light },
			{ UIStylePreset::Classic, UIStylePresetSelectorWidgetStrings::Classic },
			{ UIStylePreset::DeepDark, UIStylePresetSelectorWidgetStrings::DeepDark },
			{ UIStylePreset::DarkRed, UIStylePresetSelectorWidgetStrings::DarkRed },
			{ UIStylePreset::SteamClassic, UIStylePresetSelectorWidgetStrings::SteamClassic },
			{ UIStylePreset::ItaDark, UIStylePresetSelectorWidgetStrings::ItaDark },
			{ UIStylePreset::ItaLight, UIStylePresetSelectorWidgetStrings::ItaLight },
			{ UIStylePreset::ItaClassic, UIStylePresetSelectorWidgetStrings::ItaClassic },
			{ UIStylePreset::S56, UIStylePresetSelectorWidgetStrings::S56 },
			{ UIStylePreset::CorpGrey, UIStylePresetSelectorWidgetStrings::CorpGrey },
			{ UIStylePreset::CorpGreyFlat, UIStylePresetSelectorWidgetStrings::CorpGreyFlat },

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
				for (auto& e : m_data)
				{
					ImGui::PushID(stl::underlying(e.first));

					bool selected = (e.first == a_preset);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							LS<UIStylePresetSelectorWidgetStrings, 3>(e.second, "1"),
							selected))
					{
						a_preset = e.first;
						result   = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UIStylePresetSelectorWidget::preset_to_desc(
			UIStylePreset a_preset) const
		{
			switch (a_preset)
			{
			case UIStylePreset::Dark:
				return LS(UIStylePresetSelectorWidgetStrings::Dark);
			case UIStylePreset::Light:
				return LS(UIStylePresetSelectorWidgetStrings::Light);
			case UIStylePreset::Classic:
				return LS(UIStylePresetSelectorWidgetStrings::Classic);
			case UIStylePreset::DeepDark:
				return LS(UIStylePresetSelectorWidgetStrings::DeepDark);
			case UIStylePreset::DarkRed:
				return LS(UIStylePresetSelectorWidgetStrings::DarkRed);
			case UIStylePreset::SteamClassic:
				return LS(UIStylePresetSelectorWidgetStrings::SteamClassic);
			case UIStylePreset::ItaDark:
				return LS(UIStylePresetSelectorWidgetStrings::ItaDark);
			case UIStylePreset::ItaLight:
				return LS(UIStylePresetSelectorWidgetStrings::ItaLight);
			case UIStylePreset::S56:
				return LS(UIStylePresetSelectorWidgetStrings::S56);
			case UIStylePreset::CorpGrey:
				return LS(UIStylePresetSelectorWidgetStrings::CorpGrey);
			case UIStylePreset::CorpGreyFlat:
				return LS(UIStylePresetSelectorWidgetStrings::CorpGreyFlat);
			case UIStylePreset::ItaClassic:
				return LS(UIStylePresetSelectorWidgetStrings::ItaClassic);
			default:
				return nullptr;
			}
		}
	}
}