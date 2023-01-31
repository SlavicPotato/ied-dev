#pragma once

#include "IED/ConfigCommon.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		class UIActorValueSelectorWidget
		{
		public:
			static bool DrawActorValueSelector(
				RE::ActorValue& a_av);

			static const char* actor_value_to_desc(
				RE::ActorValue a_av);
		};
	}

}
