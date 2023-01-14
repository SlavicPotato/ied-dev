#include "pch.h"

#include "UILifeStateSelectorWidget.h"

namespace IED
{
	namespace UI
	{

		static constexpr auto s_data = stl::make_array(

			std::make_pair(ActorState::ACTOR_LIFE_STATE::kAlive, UILifeStateSelectorWidgetStrings::Alive),
			std::make_pair(ActorState::ACTOR_LIFE_STATE::kDying, UILifeStateSelectorWidgetStrings::Dying),
			std::make_pair(ActorState::ACTOR_LIFE_STATE::kDead, UILifeStateSelectorWidgetStrings::Dead),
			std::make_pair(ActorState::ACTOR_LIFE_STATE::kUnconcious, UILifeStateSelectorWidgetStrings::Unconcious),
			std::make_pair(ActorState::ACTOR_LIFE_STATE::kReanimate, UILifeStateSelectorWidgetStrings::Reanimate),
			std::make_pair(ActorState::ACTOR_LIFE_STATE::kRecycle, UILifeStateSelectorWidgetStrings::Recycle),
			std::make_pair(ActorState::ACTOR_LIFE_STATE::kRestrained, UILifeStateSelectorWidgetStrings::Restrained),
			std::make_pair(ActorState::ACTOR_LIFE_STATE::kEssentialDown, UILifeStateSelectorWidgetStrings::EssentialDown),
			std::make_pair(ActorState::ACTOR_LIFE_STATE::kBleedout, UILifeStateSelectorWidgetStrings::Bleedout)

		);

		bool UILifeStateSelectorWidget::DrawLifeStateSelector(
			ActorState::ACTOR_LIFE_STATE& a_state)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Source, "ex_ls_sel"),
					life_state_to_desc(a_state),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : s_data)
				{
					ImGui::PushID(stl::underlying(e.first));

					const bool selected = (e.first == a_state);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							UIL::LS<UILifeStateSelectorWidgetStrings, 3>(e.second, "1"),
							selected))
					{
						a_state = e.first;
						result  = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UILifeStateSelectorWidget::life_state_to_desc(
			ActorState::ACTOR_LIFE_STATE a_state)
		{
			switch (a_state)
			{
			case ActorState::ACTOR_LIFE_STATE::kAlive:
				return UIL::LS(UILifeStateSelectorWidgetStrings::Alive);
			case ActorState::ACTOR_LIFE_STATE::kDying:
				return UIL::LS(UILifeStateSelectorWidgetStrings::Dying);
			case ActorState::ACTOR_LIFE_STATE::kDead:
				return UIL::LS(UILifeStateSelectorWidgetStrings::Dead);
			case ActorState::ACTOR_LIFE_STATE::kUnconcious:
				return UIL::LS(UILifeStateSelectorWidgetStrings::Unconcious);
			case ActorState::ACTOR_LIFE_STATE::kReanimate:
				return UIL::LS(UILifeStateSelectorWidgetStrings::Reanimate);
			case ActorState::ACTOR_LIFE_STATE::kRecycle:
				return UIL::LS(UILifeStateSelectorWidgetStrings::Recycle);
			case ActorState::ACTOR_LIFE_STATE::kRestrained:
				return UIL::LS(UILifeStateSelectorWidgetStrings::Restrained);
			case ActorState::ACTOR_LIFE_STATE::kEssentialDown:
				return UIL::LS(UILifeStateSelectorWidgetStrings::EssentialDown);
			case ActorState::ACTOR_LIFE_STATE::kBleedout:
				return UIL::LS(UILifeStateSelectorWidgetStrings::Bleedout);
			default:
				return nullptr;
			}
		}
	}

}
