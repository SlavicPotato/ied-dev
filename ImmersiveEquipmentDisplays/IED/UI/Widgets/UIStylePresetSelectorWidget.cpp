#include "pch.h"

#include "UIStylePresetSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		static constexpr auto s_data = stl::make_array(

			std::make_pair(UIStylePreset::Dark, UIStylePresetSelectorWidgetStrings::Dark),
			std::make_pair(UIStylePreset::Light, UIStylePresetSelectorWidgetStrings::Light),
			std::make_pair(UIStylePreset::Classic, UIStylePresetSelectorWidgetStrings::Classic),
			std::make_pair(UIStylePreset::DeepDark, UIStylePresetSelectorWidgetStrings::DeepDark),
			std::make_pair(UIStylePreset::DarkRed, UIStylePresetSelectorWidgetStrings::DarkRed),
			std::make_pair(UIStylePreset::SteamClassic, UIStylePresetSelectorWidgetStrings::SteamClassic),
			std::make_pair(UIStylePreset::ItaDark, UIStylePresetSelectorWidgetStrings::ItaDark),
			std::make_pair(UIStylePreset::ItaLight, UIStylePresetSelectorWidgetStrings::ItaLight),
			std::make_pair(UIStylePreset::ItaClassic, UIStylePresetSelectorWidgetStrings::ItaClassic),
			std::make_pair(UIStylePreset::S56, UIStylePresetSelectorWidgetStrings::S56),
			std::make_pair(UIStylePreset::S562, UIStylePresetSelectorWidgetStrings::S562),
			std::make_pair(UIStylePreset::CorpGrey, UIStylePresetSelectorWidgetStrings::CorpGrey),
			std::make_pair(UIStylePreset::CorpGreyFlat, UIStylePresetSelectorWidgetStrings::CorpGreyFlat),
			std::make_pair(UIStylePreset::SpectrumDark, UIStylePresetSelectorWidgetStrings::SpectrumDark),
			std::make_pair(UIStylePreset::EnemyMouse, UIStylePresetSelectorWidgetStrings::EnemyMouse)

		);

		bool UIStylePresetSelectorWidget::DrawStylePresetSelector(
			UIStylePreset& a_preset)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Style, "ex_style_sel"),
					preset_to_desc(a_preset),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : s_data)
				{
					ImGui::PushID(stl::underlying(e.first));

					bool selected = (e.first == a_preset);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							UIL::LS<UIStylePresetSelectorWidgetStrings, 3>(e.second, "1"),
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
			UIStylePreset a_preset)
		{
			switch (a_preset)
			{
			case UIStylePreset::Dark:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::Dark);
			case UIStylePreset::Light:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::Light);
			case UIStylePreset::Classic:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::Classic);
			case UIStylePreset::DeepDark:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::DeepDark);
			case UIStylePreset::DarkRed:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::DarkRed);
			case UIStylePreset::SteamClassic:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::SteamClassic);
			case UIStylePreset::ItaDark:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::ItaDark);
			case UIStylePreset::ItaLight:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::ItaLight);
			case UIStylePreset::S56:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::S56);
			case UIStylePreset::CorpGrey:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::CorpGrey);
			case UIStylePreset::CorpGreyFlat:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::CorpGreyFlat);
			case UIStylePreset::ItaClassic:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::ItaClassic);
			case UIStylePreset::SpectrumDark:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::SpectrumDark);
			case UIStylePreset::EnemyMouse:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::EnemyMouse);
			case UIStylePreset::S562:
				return UIL::LS(UIStylePresetSelectorWidgetStrings::S562);
			default:
				return nullptr;
			}
		}
	}
}