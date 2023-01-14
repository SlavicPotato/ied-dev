#pragma once

#include "UILifeStateSelectorWidgetStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		class UILifeStateSelectorWidget
		{
		public:
			static bool DrawLifeStateSelector(ActorState::ACTOR_LIFE_STATE& a_state);

			static const char* life_state_to_desc(ActorState::ACTOR_LIFE_STATE a_state);
		};

	}

}
