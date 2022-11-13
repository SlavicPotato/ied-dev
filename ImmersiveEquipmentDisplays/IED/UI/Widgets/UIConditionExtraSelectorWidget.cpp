#include "pch.h"

#include "UIConditionExtraSelectorWidget.h"

namespace IED
{
	namespace UI
	{

		static constexpr auto s_data = stl::make_array(

			std::make_pair(Data::ExtraConditionType::kAngryWithPlayer, UIConditionExtraSelectorWidgetStrings::AngryWithPlayer),
			std::make_pair(Data::ExtraConditionType::kBleedingOut, UIConditionExtraSelectorWidgetStrings::BleedingOut),
			std::make_pair(Data::ExtraConditionType::kBribedByPlayer, UIConditionExtraSelectorWidgetStrings::BribedByPlayer),
			std::make_pair(Data::ExtraConditionType::kCanDualWield, UIConditionExtraSelectorWidgetStrings::CanDualWield),
			std::make_pair(Data::ExtraConditionType::kClass, UIConditionExtraSelectorWidgetStrings::Class),
			std::make_pair(Data::ExtraConditionType::kCombatStyle, UIConditionExtraSelectorWidgetStrings::CombatStyle),
			std::make_pair(Data::ExtraConditionType::kEssential, UIConditionExtraSelectorWidgetStrings::Essential),
			std::make_pair(Data::ExtraConditionType::kHumanoidSkeleton, UIConditionExtraSelectorWidgetStrings::HumanoidSkeleton),
			std::make_pair(Data::ExtraConditionType::kInCombat, UIConditionExtraSelectorWidgetStrings::InCombat),
			std::make_pair(Data::ExtraConditionType::kIsInFirstPerson, UIConditionExtraSelectorWidgetStrings::InFirstPerson),
			std::make_pair(Data::ExtraConditionType::kInInterior, UIConditionExtraSelectorWidgetStrings::InInterior),
			std::make_pair(Data::ExtraConditionType::kInKillmove, UIConditionExtraSelectorWidgetStrings::InKillmove),
			std::make_pair(Data::ExtraConditionType::kInMerchantFaction, UIConditionExtraSelectorWidgetStrings::InMerchantFaction),
			std::make_pair(Data::ExtraConditionType::kInPlayerEnemyFaction, UIConditionExtraSelectorWidgetStrings::InPlayerEnemyFaction),
			std::make_pair(Data::ExtraConditionType::kInWater, UIConditionExtraSelectorWidgetStrings::InWater),
			std::make_pair(Data::ExtraConditionType::kArrested, UIConditionExtraSelectorWidgetStrings::IsArrested),
			std::make_pair(Data::ExtraConditionType::kIsChild, UIConditionExtraSelectorWidgetStrings::IsChild),
			std::make_pair(Data::ExtraConditionType::kIsCommanded, UIConditionExtraSelectorWidgetStrings::IsCommanded),
			std::make_pair(Data::ExtraConditionType::kIsDead, UIConditionExtraSelectorWidgetStrings::IsDead),
			std::make_pair(Data::ExtraConditionType::kIsFemale, UIConditionExtraSelectorWidgetStrings::IsFemale),
			std::make_pair(Data::ExtraConditionType::kIsFlying, UIConditionExtraSelectorWidgetStrings::IsFlying),
			std::make_pair(Data::ExtraConditionType::kIsGuard, UIConditionExtraSelectorWidgetStrings::IsGuard),
			std::make_pair(Data::ExtraConditionType::kIsHorse, UIConditionExtraSelectorWidgetStrings::IsHorse),
			std::make_pair(Data::ExtraConditionType::kIsLayingDown, UIConditionExtraSelectorWidgetStrings::IsLayingDown),
			std::make_pair(Data::ExtraConditionType::kIsMount, UIConditionExtraSelectorWidgetStrings::IsMount),
			std::make_pair(Data::ExtraConditionType::kIsRidingMount, UIConditionExtraSelectorWidgetStrings::IsRidingMount),
			std::make_pair(Data::ExtraConditionType::kBeingRidden, UIConditionExtraSelectorWidgetStrings::IsBeingRidden),
			std::make_pair(Data::ExtraConditionType::kIsPlayer, UIConditionExtraSelectorWidgetStrings::IsPlayer),
			std::make_pair(Data::ExtraConditionType::kIsPlayerLastRiddenMount, UIConditionExtraSelectorWidgetStrings::IsPlayerLastRiddenMount),
			std::make_pair(Data::ExtraConditionType::kIsPlayerTeammate, UIConditionExtraSelectorWidgetStrings::IsPlayerTeammate),
			std::make_pair(Data::ExtraConditionType::kIsUnconscious, UIConditionExtraSelectorWidgetStrings::IsUnconscious),
			std::make_pair(Data::ExtraConditionType::kNodeMonitor, UIConditionExtraSelectorWidgetStrings::NodeMonitor),
			std::make_pair(Data::ExtraConditionType::kParalyzed, UIConditionExtraSelectorWidgetStrings::Paralyzed),
			std::make_pair(Data::ExtraConditionType::kProtected, UIConditionExtraSelectorWidgetStrings::Protected),
			std::make_pair(Data::ExtraConditionType::kRandomPercent, UIConditionExtraSelectorWidgetStrings::RandomPercent),
#if defined(IED_ENABLE_CONDITION_EN)
			std::make_pair(Data::ExtraConditionType::kPlayerEnemiesNearby, UIConditionExtraSelectorWidgetStrings::PlayerEnemiesNearby),
#endif
			std::make_pair(Data::ExtraConditionType::kShoutEquipped, UIConditionExtraSelectorWidgetStrings::ShoutEquipped),
			std::make_pair(Data::ExtraConditionType::kSDSShieldOnBackEnabled, UIConditionExtraSelectorWidgetStrings::SDSShieldOnBackEnabled),
			std::make_pair(Data::ExtraConditionType::kSitting, UIConditionExtraSelectorWidgetStrings::Sitting),
			std::make_pair(Data::ExtraConditionType::kSleeping, UIConditionExtraSelectorWidgetStrings::Sleeping),
			std::make_pair(Data::ExtraConditionType::kSwimming, UIConditionExtraSelectorWidgetStrings::Swimming),
			std::make_pair(Data::ExtraConditionType::kTimeOfDay, UIConditionExtraSelectorWidgetStrings::TimeOfDay),
			std::make_pair(Data::ExtraConditionType::kTresspassing, UIConditionExtraSelectorWidgetStrings::Tresspassing),
			std::make_pair(Data::ExtraConditionType::kUnderwater, UIConditionExtraSelectorWidgetStrings::Underwater),
			std::make_pair(Data::ExtraConditionType::kWeaponDrawn, UIConditionExtraSelectorWidgetStrings::WeaponDrawn)

		);

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
				for (auto& [i, e] : s_data)
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
			case Data::ExtraConditionType::kIsRidingMount:
				return LS(UIConditionExtraSelectorWidgetStrings::IsRidingMount);
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
			case Data::ExtraConditionType::kBeingRidden:
				return LS(UIConditionExtraSelectorWidgetStrings::IsBeingRidden);
			case Data::ExtraConditionType::kWeaponDrawn:
				return LS(UIConditionExtraSelectorWidgetStrings::WeaponDrawn);
			case Data::ExtraConditionType::kRandomPercent:
				return LS(UIConditionExtraSelectorWidgetStrings::RandomPercent);
			case Data::ExtraConditionType::kNodeMonitor:
				return LS(UIConditionExtraSelectorWidgetStrings::NodeMonitor);
			case Data::ExtraConditionType::kArrested:
				return LS(UIConditionExtraSelectorWidgetStrings::IsArrested);
			case Data::ExtraConditionType::kIsChild:
				return LS(UIConditionExtraSelectorWidgetStrings::IsChild);
			case Data::ExtraConditionType::kInKillmove:
				return LS(UIConditionExtraSelectorWidgetStrings::InKillmove);
			case Data::ExtraConditionType::kInMerchantFaction:
				return LS(UIConditionExtraSelectorWidgetStrings::InMerchantFaction);
			case Data::ExtraConditionType::kIsUnconscious:
				return LS(UIConditionExtraSelectorWidgetStrings::IsUnconscious);
			case Data::ExtraConditionType::kIsPlayerLastRiddenMount:
				return LS(UIConditionExtraSelectorWidgetStrings::IsPlayerLastRiddenMount);
			case Data::ExtraConditionType::kSDSShieldOnBackEnabled:
				return LS(UIConditionExtraSelectorWidgetStrings::SDSShieldOnBackEnabled);
			case Data::ExtraConditionType::kIsFlying:
				return LS(UIConditionExtraSelectorWidgetStrings::IsFlying);
			case Data::ExtraConditionType::kIsLayingDown:
				return LS(UIConditionExtraSelectorWidgetStrings::IsLayingDown);
			case Data::ExtraConditionType::kInPlayerEnemyFaction:
				return LS(UIConditionExtraSelectorWidgetStrings::InPlayerEnemyFaction);
			case Data::ExtraConditionType::kIsHorse:
				return LS(UIConditionExtraSelectorWidgetStrings::IsHorse);
			default:
				return nullptr;
			}
		}
	}
}