#include "pch.h"

#include "UIConditionExtraSelectorWidget.h"

namespace IED
{
	namespace UI
	{

		static constexpr auto s_data = stl::make_array(

			std::make_pair(Data::ExtraConditionType::kActorValue, UIConditionExtraSelectorWidgetStrings::ActorValue),
			std::make_pair(Data::ExtraConditionType::kAngryWithPlayer, UIConditionExtraSelectorWidgetStrings::AngryWithPlayer),
			std::make_pair(Data::ExtraConditionType::kBleedingOut, UIConditionExtraSelectorWidgetStrings::BleedingOut),
			std::make_pair(Data::ExtraConditionType::kBribedByPlayer, UIConditionExtraSelectorWidgetStrings::BribedByPlayer),
			std::make_pair(Data::ExtraConditionType::kCanDualWield, UIConditionExtraSelectorWidgetStrings::CanDualWield),
			std::make_pair(Data::ExtraConditionType::kClass, UIConditionExtraSelectorWidgetStrings::Class),
			std::make_pair(Data::ExtraConditionType::kCombatStyle, UIConditionExtraSelectorWidgetStrings::CombatStyle),
			std::make_pair(Data::ExtraConditionType::kDayOfWeek, UIConditionExtraSelectorWidgetStrings::DayOfWeek),
			std::make_pair(Data::ExtraConditionType::kEssential, UIConditionExtraSelectorWidgetStrings::Essential),
			std::make_pair(Data::ExtraConditionType::kHumanoidSkeleton, UIConditionExtraSelectorWidgetStrings::HumanoidSkeleton),
			std::make_pair(Data::ExtraConditionType::kInCombat, UIConditionExtraSelectorWidgetStrings::InCombat),
			std::make_pair(Data::ExtraConditionType::kInDarkArea, UIConditionExtraSelectorWidgetStrings::InDarkArea),
			std::make_pair(Data::ExtraConditionType::kInDialogue, UIConditionExtraSelectorWidgetStrings::InDialogue),
			std::make_pair(Data::ExtraConditionType::kIsInFirstPerson, UIConditionExtraSelectorWidgetStrings::InFirstPerson),
			std::make_pair(Data::ExtraConditionType::kInInterior, UIConditionExtraSelectorWidgetStrings::InInterior),
			std::make_pair(Data::ExtraConditionType::kInKillmove, UIConditionExtraSelectorWidgetStrings::InKillmove),
			std::make_pair(Data::ExtraConditionType::kInMerchantFaction, UIConditionExtraSelectorWidgetStrings::InMerchantFaction),
			std::make_pair(Data::ExtraConditionType::kInOwnedCell, UIConditionExtraSelectorWidgetStrings::InOwnedCell),
			std::make_pair(Data::ExtraConditionType::kInPlayerEnemyFaction, UIConditionExtraSelectorWidgetStrings::InPlayerEnemyFaction),
			std::make_pair(Data::ExtraConditionType::kInPublicCell, UIConditionExtraSelectorWidgetStrings::InPublicCell),
			std::make_pair(Data::ExtraConditionType::kInWater, UIConditionExtraSelectorWidgetStrings::InWater),
			std::make_pair(Data::ExtraConditionType::kArrested, UIConditionExtraSelectorWidgetStrings::IsArrested),
			std::make_pair(Data::ExtraConditionType::kIsCellOwner, UIConditionExtraSelectorWidgetStrings::IsCellOwner),
			std::make_pair(Data::ExtraConditionType::kIsChild, UIConditionExtraSelectorWidgetStrings::IsChild),
			std::make_pair(Data::ExtraConditionType::kIsCommanded, UIConditionExtraSelectorWidgetStrings::IsCommanded),
			std::make_pair(Data::ExtraConditionType::kIsDead, UIConditionExtraSelectorWidgetStrings::IsDead),
			std::make_pair(Data::ExtraConditionType::kIsFemale, UIConditionExtraSelectorWidgetStrings::IsFemale),
			std::make_pair(Data::ExtraConditionType::kIsFlying, UIConditionExtraSelectorWidgetStrings::IsFlying),
			std::make_pair(Data::ExtraConditionType::kIsGuard, UIConditionExtraSelectorWidgetStrings::IsGuard),
			std::make_pair(Data::ExtraConditionType::kIsHorse, UIConditionExtraSelectorWidgetStrings::IsHorse),
			std::make_pair(Data::ExtraConditionType::kIsInvulnerable, UIConditionExtraSelectorWidgetStrings::IsInvulnerable),
			std::make_pair(Data::ExtraConditionType::kIsLayingDown, UIConditionExtraSelectorWidgetStrings::IsLayingDown),
			std::make_pair(Data::ExtraConditionType::kIsMount, UIConditionExtraSelectorWidgetStrings::IsMount),
			std::make_pair(Data::ExtraConditionType::kIsRidingMount, UIConditionExtraSelectorWidgetStrings::IsRidingMount),
			std::make_pair(Data::ExtraConditionType::kIsNPCCellOwner, UIConditionExtraSelectorWidgetStrings::IsNPCCellOwner),
			std::make_pair(Data::ExtraConditionType::kIsMountRidden, UIConditionExtraSelectorWidgetStrings::IsMountRidden),
			std::make_pair(Data::ExtraConditionType::kIsPlayer, UIConditionExtraSelectorWidgetStrings::IsPlayer),
			std::make_pair(Data::ExtraConditionType::kIsPlayerLastRiddenMount, UIConditionExtraSelectorWidgetStrings::IsPlayerLastRiddenMount),
			std::make_pair(Data::ExtraConditionType::kIsPlayerTeammate, UIConditionExtraSelectorWidgetStrings::IsPlayerTeammate),
			std::make_pair(Data::ExtraConditionType::kIsSneaking, UIConditionExtraSelectorWidgetStrings::IsSneaking),
			std::make_pair(Data::ExtraConditionType::kIsSunAboveHorizon, UIConditionExtraSelectorWidgetStrings::IsSunAboveHorizon),
			std::make_pair(Data::ExtraConditionType::kIsSummonable, UIConditionExtraSelectorWidgetStrings::IsSummonable),
			std::make_pair(Data::ExtraConditionType::kIsUnconscious, UIConditionExtraSelectorWidgetStrings::IsUnconscious),
			std::make_pair(Data::ExtraConditionType::kIsUnique, UIConditionExtraSelectorWidgetStrings::IsUnique),
			std::make_pair(Data::ExtraConditionType::kKeyIDToggled, UIConditionExtraSelectorWidgetStrings::KeyIDToggled),
			std::make_pair(Data::ExtraConditionType::kLevel, UIConditionExtraSelectorWidgetStrings::Level),
			std::make_pair(Data::ExtraConditionType::kLifeState, UIConditionExtraSelectorWidgetStrings::LifeState),
			std::make_pair(Data::ExtraConditionType::kLightingTemplate, UIConditionExtraSelectorWidgetStrings::LightingTemplate),
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
			std::make_pair(Data::ExtraConditionType::kSunAngle, UIConditionExtraSelectorWidgetStrings::SunAngle),
			std::make_pair(Data::ExtraConditionType::kSwimming, UIConditionExtraSelectorWidgetStrings::Swimming),
			std::make_pair(Data::ExtraConditionType::kTimeOfDay, UIConditionExtraSelectorWidgetStrings::TimeOfDay),
			std::make_pair(Data::ExtraConditionType::kTresspassing, UIConditionExtraSelectorWidgetStrings::Tresspassing),
			std::make_pair(Data::ExtraConditionType::kUnderwater, UIConditionExtraSelectorWidgetStrings::Underwater),
			std::make_pair(Data::ExtraConditionType::kWeaponDrawn, UIConditionExtraSelectorWidgetStrings::WeaponDrawn),
			std::make_pair(Data::ExtraConditionType::kXP32Skeleton, UIConditionExtraSelectorWidgetStrings::XP32Skeleton)

		);

		bool UIConditionExtraSelectorWidget::DrawExtraConditionSelector(
			Data::ExtraConditionType& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Type, "ex_cond_sel"),
					condition_type_to_desc(a_type),
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
							UIL::LS<UIConditionExtraSelectorWidgetStrings, 3>(e, "1"),
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
			Data::ExtraConditionType a_type)
		{
			switch (a_type)
			{
			case Data::ExtraConditionType::kCanDualWield:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::CanDualWield);
			case Data::ExtraConditionType::kIsDead:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsDead);
			case Data::ExtraConditionType::kInInterior:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::InInterior);
			case Data::ExtraConditionType::kIsPlayerTeammate:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsPlayerTeammate);
			case Data::ExtraConditionType::kIsGuard:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsGuard);
			case Data::ExtraConditionType::kIsMount:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsMount);
			case Data::ExtraConditionType::kShoutEquipped:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::ShoutEquipped);
			case Data::ExtraConditionType::kCombatStyle:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::CombatStyle);
			case Data::ExtraConditionType::kClass:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::Class);
			case Data::ExtraConditionType::kTimeOfDay:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::TimeOfDay);
			case Data::ExtraConditionType::kIsInFirstPerson:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::InFirstPerson);
			case Data::ExtraConditionType::kInCombat:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::InCombat);
			case Data::ExtraConditionType::kIsFemale:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsFemale);
#if defined(IED_ENABLE_CONDITION_EN)
			case Data::ExtraConditionType::kPlayerEnemiesNearby:
				return UILI::LS(UIConditionExtraSelectorWidgetStrings::PlayerEnemiesNearby);
#endif
			case Data::ExtraConditionType::kInWater:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::InWater);
			case Data::ExtraConditionType::kUnderwater:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::Underwater);
			case Data::ExtraConditionType::kSwimming:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::Swimming);
			case Data::ExtraConditionType::kBleedingOut:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::BleedingOut);
			case Data::ExtraConditionType::kTresspassing:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::Tresspassing);
			case Data::ExtraConditionType::kIsCommanded:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsCommanded);
			case Data::ExtraConditionType::kParalyzed:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::Paralyzed);
			case Data::ExtraConditionType::kIsRidingMount:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsRidingMount);
			case Data::ExtraConditionType::kHumanoidSkeleton:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::HumanoidSkeleton);
			case Data::ExtraConditionType::kIsPlayer:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsPlayer);
			case Data::ExtraConditionType::kBribedByPlayer:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::BribedByPlayer);
			case Data::ExtraConditionType::kAngryWithPlayer:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::AngryWithPlayer);
			case Data::ExtraConditionType::kEssential:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::Essential);
			case Data::ExtraConditionType::kProtected:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::Protected);
			case Data::ExtraConditionType::kSitting:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::Sitting);
			case Data::ExtraConditionType::kSleeping:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::Sleeping);
			case Data::ExtraConditionType::kIsMountRidden:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsMountRidden);
			case Data::ExtraConditionType::kWeaponDrawn:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::WeaponDrawn);
			case Data::ExtraConditionType::kRandomPercent:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::RandomPercent);
			case Data::ExtraConditionType::kNodeMonitor:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::NodeMonitor);
			case Data::ExtraConditionType::kArrested:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsArrested);
			case Data::ExtraConditionType::kIsChild:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsChild);
			case Data::ExtraConditionType::kInKillmove:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::InKillmove);
			case Data::ExtraConditionType::kInMerchantFaction:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::InMerchantFaction);
			case Data::ExtraConditionType::kIsUnconscious:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsUnconscious);
			case Data::ExtraConditionType::kIsPlayerLastRiddenMount:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsPlayerLastRiddenMount);
			case Data::ExtraConditionType::kSDSShieldOnBackEnabled:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::SDSShieldOnBackEnabled);
			case Data::ExtraConditionType::kIsFlying:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsFlying);
			case Data::ExtraConditionType::kIsLayingDown:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsLayingDown);
			case Data::ExtraConditionType::kInPlayerEnemyFaction:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::InPlayerEnemyFaction);
			case Data::ExtraConditionType::kIsHorse:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsHorse);
			case Data::ExtraConditionType::kIsUnique:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsUnique);
			case Data::ExtraConditionType::kIsSummonable:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsSummonable);
			case Data::ExtraConditionType::kIsInvulnerable:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsInvulnerable);
			case Data::ExtraConditionType::kLevel:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::Level);
			case Data::ExtraConditionType::kDayOfWeek:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::DayOfWeek);
			case Data::ExtraConditionType::kIsSneaking:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsSneaking);
			case Data::ExtraConditionType::kInDialogue:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::InDialogue);
			case Data::ExtraConditionType::kLifeState:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::LifeState);
			case Data::ExtraConditionType::kActorValue:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::ActorValue);
			case Data::ExtraConditionType::kXP32Skeleton:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::XP32Skeleton);
			case Data::ExtraConditionType::kInDarkArea:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::InDarkArea);
			case Data::ExtraConditionType::kInPublicCell:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::InPublicCell);
			case Data::ExtraConditionType::kIsCellOwner:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsCellOwner);
			case Data::ExtraConditionType::kInOwnedCell:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::InOwnedCell);
			case Data::ExtraConditionType::kIsNPCCellOwner:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsNPCCellOwner);
			case Data::ExtraConditionType::kSunAngle:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::SunAngle);
			case Data::ExtraConditionType::kIsSunAboveHorizon:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::IsSunAboveHorizon);
			case Data::ExtraConditionType::kKeyIDToggled:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::KeyIDToggled);
			case Data::ExtraConditionType::kLightingTemplate:
				return UIL::LS(UIConditionExtraSelectorWidgetStrings::LightingTemplate);
			default:
				return nullptr;
			}
		}
	}
}