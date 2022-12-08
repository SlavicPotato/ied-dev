#pragma once

#include "IED/ConditionalVariableStorage.h"

#include "IED/UI/UILocalizationInterface.h"

#include "UIVariableTypeSelectorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		class UIVariableTypeSelectorWidget 
		{
		public:

			static bool DrawVariableTypeSelectorWidget(
				ConditionalVariableType& a_type);

			static const char* variable_type_to_desc(ConditionalVariableType a_type) ;

		};
	}

}
