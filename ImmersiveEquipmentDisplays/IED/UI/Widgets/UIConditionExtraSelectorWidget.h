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
			UIConditionExtraSelectorWidget(
				Localization::ILocalization& a_localization);

			bool DrawExtraConditionSelector(
				Data::ExtraConditionType& a_type);

			const char* condition_type_to_desc(
				Data::ExtraConditionType a_type) const;

		private:
			inline static constexpr std::size_t NUM_CONDITIONS =
#if defined(IED_ENABLE_CONDITION_EN)
				34
#else
				33
#endif
				;

			using data_type = std::array<
				std::pair<
					Data::ExtraConditionType,
					UIConditionExtraSelectorWidgetStrings>,
				NUM_CONDITIONS>;

			static const data_type m_data;
		};
	}

}
