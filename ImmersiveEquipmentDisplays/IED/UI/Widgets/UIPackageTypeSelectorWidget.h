#pragma once

#include "UIPackageTypeSelectorWidgetStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIPackageTypeSelectorWidget
		{
		public:

			static bool DrawPackageTypeSelector(
				PACKAGE_PROCEDURE_TYPE& a_type);

			static const char* procedure_type_to_desc(
				PACKAGE_PROCEDURE_TYPE a_type) ;

		};
	}

}
