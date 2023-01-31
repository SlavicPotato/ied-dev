#include "pch.h"

#include "UIActorValueSelectorWidget.h"

#include "IED/UI/UIActorValueStrings.h"

namespace IED
{
	namespace UI
	{

		static constexpr auto s_data = stl::make_array(

			std::make_pair(RE::ActorValue::kAggresion, UIActorValueStrings::Aggresion),
			std::make_pair(RE::ActorValue::kConfidence, UIActorValueStrings::Confidence),
			std::make_pair(RE::ActorValue::kEnergy, UIActorValueStrings::Energy),
			std::make_pair(RE::ActorValue::kMorality, UIActorValueStrings::Morality),
			std::make_pair(RE::ActorValue::kMood, UIActorValueStrings::Mood),
			std::make_pair(RE::ActorValue::kAssistance, UIActorValueStrings::Assistance),
			std::make_pair(RE::ActorValue::kOneHanded, UIActorValueStrings::OneHanded),
			std::make_pair(RE::ActorValue::kTwoHanded, UIActorValueStrings::TwoHanded),
			std::make_pair(RE::ActorValue::kArchery, UIActorValueStrings::Archery),
			std::make_pair(RE::ActorValue::kBlock, UIActorValueStrings::Block),
			std::make_pair(RE::ActorValue::kSmithing, UIActorValueStrings::Smithing),
			std::make_pair(RE::ActorValue::kHeavyArmor, UIActorValueStrings::HeavyArmor),
			std::make_pair(RE::ActorValue::kLightArmor, UIActorValueStrings::LightArmor),
			std::make_pair(RE::ActorValue::kPickpocket, UIActorValueStrings::Pickpocket),
			std::make_pair(RE::ActorValue::kLockpicking, UIActorValueStrings::Lockpicking),
			std::make_pair(RE::ActorValue::kSneak, UIActorValueStrings::Sneak),
			std::make_pair(RE::ActorValue::kAlchemy, UIActorValueStrings::Alchemy),
			std::make_pair(RE::ActorValue::kSpeech, UIActorValueStrings::Speech),
			std::make_pair(RE::ActorValue::kAlteration, UIActorValueStrings::Alteration),
			std::make_pair(RE::ActorValue::kConjuration, UIActorValueStrings::Conjuration),
			std::make_pair(RE::ActorValue::kDestruction, UIActorValueStrings::Destruction),
			std::make_pair(RE::ActorValue::kIllusion, UIActorValueStrings::Illusion),
			std::make_pair(RE::ActorValue::kRestoration, UIActorValueStrings::Restoration),
			std::make_pair(RE::ActorValue::kEnchanting, UIActorValueStrings::Enchanting)

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
							UIL::LS<UIActorValueStrings, 3>(e, "1"),
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
				return UIL::LS(UIActorValueStrings::Aggresion);
			case RE::ActorValue::kConfidence:
				return UIL::LS(UIActorValueStrings::Confidence);
			case RE::ActorValue::kEnergy:
				return UIL::LS(UIActorValueStrings::Energy);
			case RE::ActorValue::kMorality:
				return UIL::LS(UIActorValueStrings::Morality);
			case RE::ActorValue::kMood:
				return UIL::LS(UIActorValueStrings::Mood);
			case RE::ActorValue::kAssistance:
				return UIL::LS(UIActorValueStrings::Assistance);
			case RE::ActorValue::kOneHanded:
				return UIL::LS(UIActorValueStrings::OneHanded);
			case RE::ActorValue::kTwoHanded:
				return UIL::LS(UIActorValueStrings::TwoHanded);
			case RE::ActorValue::kArchery:
				return UIL::LS(UIActorValueStrings::Archery);
			case RE::ActorValue::kBlock:
				return UIL::LS(UIActorValueStrings::Block);
			case RE::ActorValue::kSmithing:
				return UIL::LS(UIActorValueStrings::Smithing);
			case RE::ActorValue::kHeavyArmor:
				return UIL::LS(UIActorValueStrings::HeavyArmor);
			case RE::ActorValue::kLightArmor:
				return UIL::LS(UIActorValueStrings::LightArmor);
			case RE::ActorValue::kPickpocket:
				return UIL::LS(UIActorValueStrings::Pickpocket);
			case RE::ActorValue::kLockpicking:
				return UIL::LS(UIActorValueStrings::Lockpicking);
			case RE::ActorValue::kSneak:
				return UIL::LS(UIActorValueStrings::Sneak);
			case RE::ActorValue::kAlchemy:
				return UIL::LS(UIActorValueStrings::Alchemy);
			case RE::ActorValue::kSpeech:
				return UIL::LS(UIActorValueStrings::Speech);
			case RE::ActorValue::kAlteration:
				return UIL::LS(UIActorValueStrings::Alteration);
			case RE::ActorValue::kConjuration:
				return UIL::LS(UIActorValueStrings::Conjuration);
			case RE::ActorValue::kDestruction:
				return UIL::LS(UIActorValueStrings::Destruction);
			case RE::ActorValue::kIllusion:
				return UIL::LS(UIActorValueStrings::Illusion);
			case RE::ActorValue::kRestoration:
				return UIL::LS(UIActorValueStrings::Restoration);
			case RE::ActorValue::kEnchanting:
				return UIL::LS(UIActorValueStrings::Enchanting);
			default:
				return nullptr;
			}
		}
	}
}