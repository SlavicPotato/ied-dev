#pragma once

#include "IED/ConfigCommon.h"

#include "UIComparisonOperatorSelectorStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIComparisonOperatorSelector :
			public virtual UILocalizationInterface
		{
		public:
			UIComparisonOperatorSelector(Localization::ILocalization& a_localization);

			bool DrawComparisonOperatorSelector(
				Data::ComparisonOperator& a_comp);

			const char* comp_operator_to_desc(
				Data::ComparisonOperator a_comp) const;

		};
	}

}
