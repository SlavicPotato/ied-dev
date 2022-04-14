#include "pch.h"

#include "UIPackageTypeSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIPackageTypeSelectorWidget::data_type UIPackageTypeSelectorWidget::m_data{ {

			{ PACKAGE_PROCEDURE_TYPE::kNone, static_cast<UIPackageTypeSelectorWidgetStrings>(CommonStrings::None) },
			{ PACKAGE_PROCEDURE_TYPE::kFind, UIPackageTypeSelectorWidgetStrings::Find },
			{ PACKAGE_PROCEDURE_TYPE::kFollow, UIPackageTypeSelectorWidgetStrings::Follow },
			{ PACKAGE_PROCEDURE_TYPE::kEscort, UIPackageTypeSelectorWidgetStrings::Escort },
			{ PACKAGE_PROCEDURE_TYPE::kEat, UIPackageTypeSelectorWidgetStrings::Eat },
			{ PACKAGE_PROCEDURE_TYPE::kSleep, UIPackageTypeSelectorWidgetStrings::Sleep },
			{ PACKAGE_PROCEDURE_TYPE::kWander, UIPackageTypeSelectorWidgetStrings::Wander },
			{ PACKAGE_PROCEDURE_TYPE::kTravel, UIPackageTypeSelectorWidgetStrings::Travel },
			{ PACKAGE_PROCEDURE_TYPE::kAccompany, UIPackageTypeSelectorWidgetStrings::Accompany },
			{ PACKAGE_PROCEDURE_TYPE::kUseItemAt, UIPackageTypeSelectorWidgetStrings::UseItemAt },
			{ PACKAGE_PROCEDURE_TYPE::kAmbush, UIPackageTypeSelectorWidgetStrings::Ambush },
			{ PACKAGE_PROCEDURE_TYPE::kFleeNotCombat, UIPackageTypeSelectorWidgetStrings::FleeNotCombat },
			{ PACKAGE_PROCEDURE_TYPE::kCastMagic, UIPackageTypeSelectorWidgetStrings::CastMagic },
			{ PACKAGE_PROCEDURE_TYPE::kSandbox, UIPackageTypeSelectorWidgetStrings::Sandbox },
			{ PACKAGE_PROCEDURE_TYPE::kPatrol, UIPackageTypeSelectorWidgetStrings::Patrol },
			{ PACKAGE_PROCEDURE_TYPE::kGuard, UIPackageTypeSelectorWidgetStrings::Guard },
			{ PACKAGE_PROCEDURE_TYPE::kDialogue, UIPackageTypeSelectorWidgetStrings::Dialogue },
			{ PACKAGE_PROCEDURE_TYPE::kUseWeapon, UIPackageTypeSelectorWidgetStrings::UseWeapon },
			{ PACKAGE_PROCEDURE_TYPE::kFind2, UIPackageTypeSelectorWidgetStrings::Find2 },
			{ PACKAGE_PROCEDURE_TYPE::kPackage, UIPackageTypeSelectorWidgetStrings::Package },
			{ PACKAGE_PROCEDURE_TYPE::kPackageTemplate, UIPackageTypeSelectorWidgetStrings::PackageTemplate },
			{ PACKAGE_PROCEDURE_TYPE::kActivate, UIPackageTypeSelectorWidgetStrings::Activate },
			{ PACKAGE_PROCEDURE_TYPE::kAlarm, UIPackageTypeSelectorWidgetStrings::Alarm },
			{ PACKAGE_PROCEDURE_TYPE::kFlee, UIPackageTypeSelectorWidgetStrings::Flee },
			{ PACKAGE_PROCEDURE_TYPE::kTrespass, UIPackageTypeSelectorWidgetStrings::Trespass },
			{ PACKAGE_PROCEDURE_TYPE::kSpectator, UIPackageTypeSelectorWidgetStrings::Spectator },
			{ PACKAGE_PROCEDURE_TYPE::kReactToDead, UIPackageTypeSelectorWidgetStrings::ReactToDead },
			{ PACKAGE_PROCEDURE_TYPE::kGetUpFromChairBed, UIPackageTypeSelectorWidgetStrings::GetUpFromChairBed },
			{ PACKAGE_PROCEDURE_TYPE::kDoNothing, UIPackageTypeSelectorWidgetStrings::DoNothing },
			{ PACKAGE_PROCEDURE_TYPE::kInGameDialogue, UIPackageTypeSelectorWidgetStrings::InGameDialogue },
			{ PACKAGE_PROCEDURE_TYPE::kSurface, UIPackageTypeSelectorWidgetStrings::Surface },
			{ PACKAGE_PROCEDURE_TYPE::kSearchForAttacker, UIPackageTypeSelectorWidgetStrings::SearchForAttacker },
			{ PACKAGE_PROCEDURE_TYPE::kAvoidPlayer, UIPackageTypeSelectorWidgetStrings::AvoidPlayer },
			{ PACKAGE_PROCEDURE_TYPE::kReactToDestroyedObject, UIPackageTypeSelectorWidgetStrings::ReactToDestroyedObject },
			{ PACKAGE_PROCEDURE_TYPE::kReactToGrenadeOrMine, UIPackageTypeSelectorWidgetStrings::ReactToGrenadeOrMine },
			{ PACKAGE_PROCEDURE_TYPE::kStealWarning, UIPackageTypeSelectorWidgetStrings::StealWarning },
			{ PACKAGE_PROCEDURE_TYPE::kPickPocketWarning, UIPackageTypeSelectorWidgetStrings::PickPocketWarning },
			{ PACKAGE_PROCEDURE_TYPE::kMovementBlocked, UIPackageTypeSelectorWidgetStrings::MovementBlocked },
			{ PACKAGE_PROCEDURE_TYPE::kVampireFeed, UIPackageTypeSelectorWidgetStrings::VampireFeed },
			{ PACKAGE_PROCEDURE_TYPE::kCannibal, UIPackageTypeSelectorWidgetStrings::Cannibal },

		} };

		UIPackageTypeSelectorWidget::UIPackageTypeSelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIPackageTypeSelectorWidget::DrawPackageTypeSelector(
			PACKAGE_PROCEDURE_TYPE& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(CommonStrings::Type, "pc_type_sel"),
					procedure_type_to_desc(a_type),
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
							LS<UIPackageTypeSelectorWidgetStrings, 3>(e, "1"),
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

		const char* UIPackageTypeSelectorWidget::procedure_type_to_desc(
			PACKAGE_PROCEDURE_TYPE a_type) const
		{
			switch (a_type)
			{
			case PACKAGE_PROCEDURE_TYPE::kFind:
				return LS(UIPackageTypeSelectorWidgetStrings::Find);
			case PACKAGE_PROCEDURE_TYPE::kFollow:
				return LS(UIPackageTypeSelectorWidgetStrings::Follow);
			case PACKAGE_PROCEDURE_TYPE::kEscort:
				return LS(UIPackageTypeSelectorWidgetStrings::Escort);
			case PACKAGE_PROCEDURE_TYPE::kEat:
				return LS(UIPackageTypeSelectorWidgetStrings::Eat);
			case PACKAGE_PROCEDURE_TYPE::kSleep:
				return LS(UIPackageTypeSelectorWidgetStrings::Sleep);
			case PACKAGE_PROCEDURE_TYPE::kWander:
				return LS(UIPackageTypeSelectorWidgetStrings::Wander);
			case PACKAGE_PROCEDURE_TYPE::kTravel:
				return LS(UIPackageTypeSelectorWidgetStrings::Travel);
			case PACKAGE_PROCEDURE_TYPE::kAccompany:
				return LS(UIPackageTypeSelectorWidgetStrings::Accompany);
			case PACKAGE_PROCEDURE_TYPE::kUseItemAt:
				return LS(UIPackageTypeSelectorWidgetStrings::UseItemAt);
			case PACKAGE_PROCEDURE_TYPE::kAmbush:
				return LS(UIPackageTypeSelectorWidgetStrings::Ambush);
			case PACKAGE_PROCEDURE_TYPE::kFleeNotCombat:
				return LS(UIPackageTypeSelectorWidgetStrings::FleeNotCombat);
			case PACKAGE_PROCEDURE_TYPE::kCastMagic:
				return LS(UIPackageTypeSelectorWidgetStrings::CastMagic);
			case PACKAGE_PROCEDURE_TYPE::kSandbox:
				return LS(UIPackageTypeSelectorWidgetStrings::Sandbox);
			case PACKAGE_PROCEDURE_TYPE::kGuard:
				return LS(UIPackageTypeSelectorWidgetStrings::Guard);
			case PACKAGE_PROCEDURE_TYPE::kDialogue:
				return LS(UIPackageTypeSelectorWidgetStrings::Dialogue);
			case PACKAGE_PROCEDURE_TYPE::kUseWeapon:
				return LS(UIPackageTypeSelectorWidgetStrings::UseWeapon);
			case PACKAGE_PROCEDURE_TYPE::kFind2:
				return LS(UIPackageTypeSelectorWidgetStrings::Find2);
			case PACKAGE_PROCEDURE_TYPE::kPackage:
				return LS(UIPackageTypeSelectorWidgetStrings::Package);
			case PACKAGE_PROCEDURE_TYPE::kPackageTemplate:
				return LS(UIPackageTypeSelectorWidgetStrings::PackageTemplate);
			case PACKAGE_PROCEDURE_TYPE::kActivate:
				return LS(UIPackageTypeSelectorWidgetStrings::Activate);
			case PACKAGE_PROCEDURE_TYPE::kAlarm:
				return LS(UIPackageTypeSelectorWidgetStrings::Alarm);
			case PACKAGE_PROCEDURE_TYPE::kFlee:
				return LS(UIPackageTypeSelectorWidgetStrings::Flee);
			case PACKAGE_PROCEDURE_TYPE::kTrespass:
				return LS(UIPackageTypeSelectorWidgetStrings::Trespass);
			case PACKAGE_PROCEDURE_TYPE::kSpectator:
				return LS(UIPackageTypeSelectorWidgetStrings::Spectator);
			case PACKAGE_PROCEDURE_TYPE::kReactToDead:
				return LS(UIPackageTypeSelectorWidgetStrings::ReactToDead);
			case PACKAGE_PROCEDURE_TYPE::kGetUpFromChairBed:
				return LS(UIPackageTypeSelectorWidgetStrings::GetUpFromChairBed);
			case PACKAGE_PROCEDURE_TYPE::kDoNothing:
				return LS(UIPackageTypeSelectorWidgetStrings::DoNothing);
			case PACKAGE_PROCEDURE_TYPE::kInGameDialogue:
				return LS(UIPackageTypeSelectorWidgetStrings::InGameDialogue);
			case PACKAGE_PROCEDURE_TYPE::kSurface:
				return LS(UIPackageTypeSelectorWidgetStrings::Surface);
			case PACKAGE_PROCEDURE_TYPE::kSearchForAttacker:
				return LS(UIPackageTypeSelectorWidgetStrings::SearchForAttacker);
			case PACKAGE_PROCEDURE_TYPE::kAvoidPlayer:
				return LS(UIPackageTypeSelectorWidgetStrings::AvoidPlayer);
			case PACKAGE_PROCEDURE_TYPE::kReactToDestroyedObject:
				return LS(UIPackageTypeSelectorWidgetStrings::ReactToDestroyedObject);
			case PACKAGE_PROCEDURE_TYPE::kReactToGrenadeOrMine:
				return LS(UIPackageTypeSelectorWidgetStrings::ReactToGrenadeOrMine);
			case PACKAGE_PROCEDURE_TYPE::kStealWarning:
				return LS(UIPackageTypeSelectorWidgetStrings::StealWarning);
			case PACKAGE_PROCEDURE_TYPE::kPickPocketWarning:
				return LS(UIPackageTypeSelectorWidgetStrings::PickPocketWarning);
			case PACKAGE_PROCEDURE_TYPE::kMovementBlocked:
				return LS(UIPackageTypeSelectorWidgetStrings::MovementBlocked);
			case PACKAGE_PROCEDURE_TYPE::kVampireFeed:
				return LS(UIPackageTypeSelectorWidgetStrings::VampireFeed);
			case PACKAGE_PROCEDURE_TYPE::kCannibal:
				return LS(UIPackageTypeSelectorWidgetStrings::Cannibal);
			case PACKAGE_PROCEDURE_TYPE::kNone:
				return LS(CommonStrings::None);
			default:
				return nullptr;
			}
		}
	}
}