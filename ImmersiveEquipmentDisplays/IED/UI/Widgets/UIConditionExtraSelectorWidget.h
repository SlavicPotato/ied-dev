#pragma once

#include "IED/ConfigCommon.h"

#include "UIConditionExtraSelectorWidgetStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIConditionExtraSelectorWidget :
			public virtual UILocalizationInterface
		{
		public:
			UIConditionExtraSelectorWidget(Localization::ILocalization& a_localization);

			bool DrawExtraConditionSelector(
				Data::ExtraConditionType& a_type);

			const char* condition_type_to_desc(Data::ExtraConditionType a_type) const;

		private:
			using data_type = std::array<
				std::pair<
					Data::ExtraConditionType,
					UIConditionExtraSelectorWidgetStrings>,
				14>;

			static data_type m_data;
		};
	}

}
