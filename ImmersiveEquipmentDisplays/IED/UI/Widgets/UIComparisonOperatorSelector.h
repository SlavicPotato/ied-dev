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
				Data::ExtraComparisonOperator& a_comp);

		protected:
			const char* comp_operator_to_desc(Data::ExtraComparisonOperator a_comp);

		private:
			using data_type = std::array<
				std::pair<
					Data::ExtraComparisonOperator,
					UIComparisonOperatorSelectorStrings>,
				5>;

			static data_type m_data;
		};
	}

}
