#pragma once

#include "IED/ConfigVariableConditionSource.h"

#include "IED/UI/UILocalizationInterface.h"

#include "UIVariableConditionSourceSelectorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		class UIVariableConditionSourceSelectorWidget
		{
		public:

			static bool DrawVariableConditionSourceSelectorWidget(
				Data::VariableConditionSource& a_type);

			static const char* variable_cond_source_to_desc(
				Data::VariableConditionSource a_type);

		};
	}

}
