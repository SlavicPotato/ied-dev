#include "pch.h"

#include "UIConditionExtraSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIConditionExtraSelectorWidget::data_type UIConditionExtraSelectorWidget::m_data{ {

			{ Data::ExtraConditionType::kCanDualWield, UIConditionExtraSelectorWidgetStrings::CanDualWield },
			{ Data::ExtraConditionType::kIsDead, UIConditionExtraSelectorWidgetStrings::IsDead },
			{ Data::ExtraConditionType::kInInterior, UIConditionExtraSelectorWidgetStrings::InInterior },
			{ Data::ExtraConditionType::kIsPlayerTeammate, UIConditionExtraSelectorWidgetStrings::IsPlayerTeammate },
			{ Data::ExtraConditionType::kIsGuard, UIConditionExtraSelectorWidgetStrings::IsGuard },
			{ Data::ExtraConditionType::kIsMount, UIConditionExtraSelectorWidgetStrings::IsMount },
			{ Data::ExtraConditionType::kIsOnMount, UIConditionExtraSelectorWidgetStrings::IsOnMount },
			{ Data::ExtraConditionType::kShoutEquipped, UIConditionExtraSelectorWidgetStrings::ShoutEquipped },
			//{ Data::ExtraConditionType::kInMerchantFaction, UIConditionExtraSelectorWidgetStrings::InMerchantFaction },
			{ Data::ExtraConditionType::kCombatStyle, UIConditionExtraSelectorWidgetStrings::CombatStyle },
			{ Data::ExtraConditionType::kClass, UIConditionExtraSelectorWidgetStrings::Class },
			{ Data::ExtraConditionType::kTimeOfDay, UIConditionExtraSelectorWidgetStrings::TimeOfDay },
			{ Data::ExtraConditionType::kIsInFirstPerson, UIConditionExtraSelectorWidgetStrings::InFirstPerson },
			{ Data::ExtraConditionType::kInCombat, UIConditionExtraSelectorWidgetStrings::InCombat },
			{ Data::ExtraConditionType::kIsFemale, UIConditionExtraSelectorWidgetStrings::IsFemale },
#if defined(IED_ENABLE_CONDITION_EN)
			{ Data::ExtraConditionType::kPlayerEnemiesNearby, UIConditionExtraSelectorWidgetStrings::PlayerEnemiesNearby },
#endif
			{ Data::ExtraConditionType::kInWater, UIConditionExtraSelectorWidgetStrings::InWater },
			{ Data::ExtraConditionType::kUnderwater, UIConditionExtraSelectorWidgetStrings::Underwater },
			{ Data::ExtraConditionType::kSwimming, UIConditionExtraSelectorWidgetStrings::Swimming },
			{ Data::ExtraConditionType::kBleedingOut, UIConditionExtraSelectorWidgetStrings::BleedingOut },
			{ Data::ExtraConditionType::kTresspassing, UIConditionExtraSelectorWidgetStrings::Tresspassing },
			{ Data::ExtraConditionType::kIsCommanded, UIConditionExtraSelectorWidgetStrings::IsCommanded },
			{ Data::ExtraConditionType::kParalyzed, UIConditionExtraSelectorWidgetStrings::Paralyzed },

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
			default:
				return nullptr;
			}
		}
	}
}