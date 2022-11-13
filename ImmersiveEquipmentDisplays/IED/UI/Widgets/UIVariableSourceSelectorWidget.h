#pragma once

#include "IED/ConfigVariableSource.h"

#include "IED/UI/UILocalizationInterface.h"

#include "UIVariableSourceSelectorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		class UIVariableSourceSelectorWidget :
			public virtual UILocalizationInterface
		{
		public:
			UIVariableSourceSelectorWidget(
				Localization::ILocalization& a_localization);

			bool DrawVariableSourceSelectorWidget(
				Data::VariableSource& a_type);

			const char* variable_source_to_desc(
				Data::VariableSource a_type) const;
		};
	}

}
