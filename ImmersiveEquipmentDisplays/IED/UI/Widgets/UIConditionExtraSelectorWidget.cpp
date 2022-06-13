#include "pch.h"

#include "UIConditionExtraSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIConditionExtraSelectorWidget::data_type UIConditionExtraSelectorWidget::m_data{ {

			{ Data::ExtraConditionType::kAngryWithPlayer, UIConditionExtraSelectorWidgetStrings::AngryWithPlayer },
			{ Data::ExtraConditionType::kBleedingOut, UIConditionExtraSelectorWidgetStrings::BleedingOut },
			{ Data::ExtraConditionType::kBribedByPlayer, UIConditionExtraSelectorWidgetStrings::BribedByPlayer },
			{ Data::ExtraConditionType::kCanDualWield, UIConditionExtraSelectorWidgetStrings::CanDualWield },
			{ Data::ExtraConditionType::kClass, UIConditionExtraSelectorWidgetStrings::Class },
			{ Data::ExtraConditionType::kCombatStyle, UIConditionExtraSelectorWidgetStrings::CombatStyle },
			{ Data::ExtraConditionType::kEssential, UIConditionExtraSelectorWidgetStrings::Essential },
			{ Data::ExtraConditionType::kHumanoidSkeleton, UIConditionExtraSelectorWidgetStrings::HumanoidSkeleton },
			{ Data::ExtraConditionType::kInCombat, UIConditionExtraSelectorWidgetStrings::InCombat },
			{ Data::ExtraConditionType::kInInterior, UIConditionExtraSelectorWidgetStrings::InInterior },
			//{ Data::ExtraConditionType::kInMerchantFaction, UIConditionExtraSelectorWidgetStrings::InMerchantFaction },
			{ Data::ExtraConditionType::kInWater, UIConditionExtraSelectorWidgetStrings::InWater },
			{ Data::ExtraConditionType::kIsCommanded, UIConditionExtraSelectorWidgetStrings::IsCommanded },
			{ Data::ExtraConditionType::kIsDead, UIConditionExtraSelectorWidgetStrings::IsDead },
			{ Data::ExtraConditionType::kIsFemale, UIConditionExtraSelectorWidgetStrings::IsFemale },
			{ Data::ExtraConditionType::kIsGuard, UIConditionExtraSelectorWidgetStrings::IsGuard },
			{ Data::ExtraConditionType::kIsInFirstPerson, UIConditionExtraSelectorWidgetStrings::InFirstPerson },
			{ Data::ExtraConditionType::kIsMount, UIConditionExtraSelectorWidgetStrings::IsMount },
			{ Data::ExtraConditionType::kIsOnMount, UIConditionExtraSelectorWidgetStrings::IsOnMount },
			{ Data::ExtraConditionType::kIsPlayer, UIConditionExtraSelectorWidgetStrings::IsPlayer },
			{ Data::ExtraConditionType::kIsPlayerTeammate, UIConditionExtraSelectorWidgetStrings::IsPlayerTeammate },
			{ Data::ExtraConditionType::kParalyzed, UIConditionExtraSelectorWidgetStrings::Paralyzed },
			{ Data::ExtraConditionType::kProtected, UIConditionExtraSelectorWidgetStrings::Protected },
#if defined(IED_ENABLE_CONDITION_EN)
			{ Data::ExtraConditionType::kPlayerEnemiesNearby, UIConditionExtraSelectorWidgetStrings::PlayerEnemiesNearby },
#endif
			{ Data::ExtraConditionType::kShoutEquipped, UIConditionExtraSelectorWidgetStrings::ShoutEquipped },
			{ Data::ExtraConditionType::kSitting, UIConditionExtraSelectorWidgetStrings::Sitting },
			{ Data::ExtraConditionType::kSleeping, UIConditionExtraSelectorWidgetStrings::Sleeping },
			{ Data::ExtraConditionType::kSwimming, UIConditionExtraSelectorWidgetStrings::Swimming },
			{ Data::ExtraConditionType::kTimeOfDay, UIConditionExtraSelectorWidgetStrings::TimeOfDay },
			{ Data::ExtraConditionType::kTresspassing, UIConditionExtraSelectorWidgetStrings::Tresspassing },
			{ Data::ExtraConditionType::kUnderwater, UIConditionExtraSelectorWidgetStrings::Underwater },

		} };

		UIConditionExtraSelectorWidget::UIConditionExtraSelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIConditionExtraSelectorWidget::DrawExtraConditionSelector(
			Data::ExtraConditionType& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(CommonStrings::Type, "ex_cond_sel"),
					condition_type_to_desc(a_type),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& [i, e] : m_data)
				{
					ImGui::PushID(stl::underlying(i));

					bool selected = (i == a_type);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							LS<UIConditionExtraSelectorWidgetStrings, 3>(e, "1"),
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

		const char* UIConditionExtraSelectorWidget::condition_type_to_desc(
			Data::ExtraConditionType a_type) const
		{
			switch (a_type)
			{
			case Data::ExtraConditionType::kCanDualWield:
				return LS(UIConditionExtraSelectorWidgetStrings::CanDualWield);
			case Data::ExtraConditionType::kIsDead:
				return LS(UIConditionExtraSelectorWidgetStrings::IsDead);
			case Data::ExtraConditionType::kInInterior:
				return LS(UIConditionExtraSelectorWidgetStrings::InInterior);
			case Data::ExtraConditionType::kIsPlayerTeammate:
				return LS(UIConditionExtraSelectorWidgetStrings::IsPlayerTeammate);
			case Data::ExtraConditionType::kIsGuard:
				return LS(UIConditionExtraSelectorWidgetStrings::IsGuard);
			case Data::ExtraConditionType::kIsMount:
				return LS(UIConditionExtraSelectorWidgetStrings::IsMount);
			case Data::ExtraConditionType::kShoutEquipped:
				return LS(UIConditionExtraSelectorWidgetStrings::ShoutEquipped);
			/*case Data::ExtraConditionType::kInMerchantFaction:
				return LS(UIConditionExtraSelectorWidgetStrings::InMerchantFaction);*/
			case Data::ExtraConditionType::kCombatStyle:
				return LS(UIConditionExtraSelectorWidgetStrings::CombatStyle);
			case Data::ExtraConditionType::kClass:
				return LS(UIConditionExtraSelectorWidgetStrings::Class);
			case Data::ExtraConditionType::kTimeOfDay:
				return LS(UIConditionExtraSelectorWidgetStrings::TimeOfDay);
			case Data::ExtraConditionType::kIsInFirstPerson:
				return LS(UIConditionExtraSelectorWidgetStrings::InFirstPerson);
			case Data::ExtraConditionType::kInCombat:
				return LS(UIConditionExtraSelectorWidgetStrings::InCombat);
			case Data::ExtraConditionType::kIsFemale:
				return LS(UIConditionExtraSelectorWidgetStrings::IsFemale);
#if defined(IED_ENABLE_CONDITION_EN)
			case Data::ExtraConditionType::kPlayerEnemiesNearby:
				return LS(UIConditionExtraSelectorWidgetStrings::PlayerEnemiesNearby);
#endif
			case Data::ExtraConditionType::kInWater:
				return LS(UIConditionExtraSelectorWidgetStrings::InWater);
			case Data::ExtraConditionType::kUnderwater:
				return LS(UIConditionExtraSelectorWidgetStrings::Underwater);
			case Data::ExtraConditionType::kSwimming:
				return LS(UIConditionExtraSelectorWidgetStrings::Swimming);
			case Data::ExtraConditionType::kBleedingOut:
				return LS(UIConditionExtraSelectorWidgetStrings::BleedingOut);
			case Data::ExtraConditionType::kTresspassing:
				return LS(UIConditionExtraSelectorWidgetStrings::Tresspassing);
			case Data::ExtraConditionType::kIsCommanded:
				return LS(UIConditionExtraSelectorWidgetStrings::IsCommanded);
			case Data::ExtraConditionType::kParalyzed:
				return LS(UIConditionExtraSelectorWidgetStrings::Paralyzed);
			case Data::ExtraConditionType::kIsOnMount:
				return LS(UIConditionExtraSelectorWidgetStrings::IsOnMount);
			case Data::ExtraConditionType::kHumanoidSkeleton:
				return LS(UIConditionExtraSelectorWidgetStrings::HumanoidSkeleton);
			case Data::ExtraConditionType::kIsPlayer:
				return LS(UIConditionExtraSelectorWidgetStrings::IsPlayer);
			case Data::ExtraConditionType::kBribedByPlayer:
				return LS(UIConditionExtraSelectorWidgetStrings::BribedByPlayer);
			case Data::ExtraConditionType::kAngryWithPlayer:
				return LS(UIConditionExtraSelectorWidgetStrings::AngryWithPlayer);
			case Data::ExtraConditionType::kEssential:
				return LS(UIConditionExtraSelectorWidgetStrings::Essential);
			case Data::ExtraConditionType::kProtected:
				return LS(UIConditionExtraSelectorWidgetStrings::Protected);
			case Data::ExtraConditionType::kSitting:
				return LS(UIConditionExtraSelectorWidgetStrings::Sitting);
			case Data::ExtraConditionType::kSleeping:
				return LS(UIConditionExtraSelectorWidgetStrings::Sleeping);
			default:
				return nullptr;
			}
		}
	}
}