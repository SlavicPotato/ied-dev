#pragma once

#include "IED/ConfigCommon.h"

#include "UIConditionExtraSelectorWidgetStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIConditionExtraSelectorWidget
		{
		public:
			static bool DrawExtraConditionSelector(
				Data::ExtraConditionType& a_type);

			static const char* condition_type_to_desc(
				Data::ExtraConditionType a_type);

		};
	}

}
