#pragma once

#include "IED/ConfigVariableConditionSource.h"

#include "IED/UI/UILocalizationInterface.h"

#include "UIVariableConditionSourceSelectorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		class UIVariableConditionSourceSelectorWidget :
			public virtual UILocalizationInterface
		{
		public:
			UIVariableConditionSourceSelectorWidget(
				Localization::ILocalization& a_localization);

			bool DrawVariableConditionSourceSelectorWidget(
				Data::VariableConditionSource& a_type);

			const char* variable_cond_source_to_desc(
				Data::VariableConditionSource a_type) const;

		};
	}

}
