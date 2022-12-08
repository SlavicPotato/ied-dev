#pragma once

#include "IED/ConfigVariableSource.h"

#include "IED/UI/UILocalizationInterface.h"

#include "UIVariableSourceSelectorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		class UIVariableSourceSelectorWidget
		{
		public:

			static bool DrawVariableSourceSelectorWidget(
				Data::VariableSource& a_type);

			static const char* variable_source_to_desc(
				Data::VariableSource a_type) ;
		};
	}

}
