#pragma once

#include "IED/ConfigCommon.h"

#include "UIComparisonOperatorSelectorStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIComparisonOperatorSelector
		{
		public:
			static bool DrawComparisonOperatorSelector(
				Data::ComparisonOperator& a_comp);

			static const char* comp_operator_to_desc(
				Data::ComparisonOperator a_comp);

		};
	}

}
