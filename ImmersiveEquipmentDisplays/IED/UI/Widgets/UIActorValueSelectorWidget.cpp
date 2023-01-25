#include "pch.h"

#include "UIActorValueSelectorWidget.h"

namespace IED
{
	namespace UI
	{

		static constexpr auto s_data = stl::make_array(

			std::make_pair(RE::ActorValue::kAggresion, UIActorValueSelectorWidgetStrings::Aggresion),
			std::make_pair(RE::ActorValue::kConfidence, UIActorValueSelectorWidgetStrings::Confidence),
			std::make_pair(RE::ActorValue::kEnergy, UIActorValueSelectorWidgetStrings::Energy),
			std::make_pair(RE::ActorValue::kMorality, UIActorValueSelectorWidgetStrings::Morality),
			std::make_pair(RE::ActorValue::kMood, UIActorValueSelectorWidgetStrings::Mood),
			std::make_pair(RE::ActorValue::kAssistance, UIActorValueSelectorWidgetStrings::Assistance),
			std::make_pair(RE::ActorValue::kOneHanded, UIActorValueSelectorWidgetStrings::OneHanded),
			std::make_pair(RE::ActorValue::kTwoHanded, UIActorValueSelectorWidgetStrings::TwoHanded),
			std::make_pair(RE::ActorValue::kArchery, UIActorValueSelectorWidgetStrings::Archery),
			std::make_pair(RE::ActorValue::kBlock, UIActorValueSelectorWidgetStrings::Block),
			std::make_pair(RE::ActorValue::kSmithing, UIActorValueSelectorWidgetStrings::Smithing),
			std::make_pair(RE::ActorValue::kHeavyArmor, UIActorValueSelectorWidgetStrings::HeavyArmor),
			std::make_pair(RE::ActorValue::kLightArmor, UIActorValueSelectorWidgetStrings::LightArmor),
			std::make_pair(RE::ActorValue::kPickpocket, UIActorValueSelectorWidgetStrings::Pickpocket),
			std::make_pair(RE::ActorValue::kLockpicking, UIActorValueSelectorWidgetStrings::Lockpicking),
			std::make_pair(RE::ActorValue::kSneak, UIActorValueSelectorWidgetStrings::Sneak),
			std::make_pair(RE::ActorValue::kAlchemy, UIActorValueSelectorWidgetStrings::Alchemy),
			std::make_pair(RE::ActorValue::kSpeech, UIActorValueSelectorWidgetStrings::Speech),
			std::make_pair(RE::ActorValue::kAlteration, UIActorValueSelectorWidgetStrings::Alteration),
			std::make_pair(RE::ActorValue::kConjuration, UIActorValueSelectorWidgetStrings::Conjuration),
			std::make_pair(RE::ActorValue::kDestruction, UIActorValueSelectorWidgetStrings::Destruction),
			std::make_pair(RE::ActorValue::kIllusion, UIActorValueSelectorWidgetStrings::Illusion),
			std::make_pair(RE::ActorValue::kRestoration, UIActorValueSelectorWidgetStrings::Restoration),
			std::make_pair(RE::ActorValue::kEnchanting, UIActorValueSelectorWidgetStrings::Enchanting)

		);

		bool UIActorValueSelectorWidget::DrawActorValueSelector(
			RE::ActorValue& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Value, "av_sel"),
					actor_value_to_desc(a_type),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& [i, e] : s_data)
				{
					ImGui::PushID(stl::underlying(i));

					const bool selected = (i == a_type);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							UIL::LS<UIActorValueSelectorWidgetStrings, 3>(e, "1"),
							selected))
					{
						a_type = i;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UIActorValueSelectorWidget::actor_value_to_desc(
			RE::ActorValue a_type)
		{
			switch (a_type)
			{
			case RE::ActorValue::kAggresion:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Aggresion);
			case RE::ActorValue::kConfidence:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Confidence);
			case RE::ActorValue::kEnergy:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Energy);
			case RE::ActorValue::kMorality:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Morality);
			case RE::ActorValue::kMood:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Mood);
			case RE::ActorValue::kAssistance:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Assistance);
			case RE::ActorValue::kOneHanded:
				return UIL::LS(UIActorValueSelectorWidgetStrings::OneHanded);
			case RE::ActorValue::kTwoHanded:
				return UIL::LS(UIActorValueSelectorWidgetStrings::TwoHanded);
			case RE::ActorValue::kArchery:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Archery);
			case RE::ActorValue::kBlock:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Block);
			case RE::ActorValue::kSmithing:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Smithing);
			case RE::ActorValue::kHeavyArmor:
				return UIL::LS(UIActorValueSelectorWidgetStrings::HeavyArmor);
			case RE::ActorValue::kLightArmor:
				return UIL::LS(UIActorValueSelectorWidgetStrings::LightArmor);
			case RE::ActorValue::kPickpocket:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Pickpocket);
			case RE::ActorValue::kLockpicking:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Lockpicking);
			case RE::ActorValue::kSneak:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Sneak);
			case RE::ActorValue::kAlchemy:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Alchemy);
			case RE::ActorValue::kSpeech:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Speech);
			case RE::ActorValue::kAlteration:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Alteration);
			case RE::ActorValue::kConjuration:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Conjuration);
			case RE::ActorValue::kDestruction:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Destruction);
			case RE::ActorValue::kIllusion:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Illusion);
			case RE::ActorValue::kRestoration:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Restoration);
			case RE::ActorValue::kEnchanting:
				return UIL::LS(UIActorValueSelectorWidgetStrings::Enchanting);
			default:
				return nullptr;
			}
		}
	}
}