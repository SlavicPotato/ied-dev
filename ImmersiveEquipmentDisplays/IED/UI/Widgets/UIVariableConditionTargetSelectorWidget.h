#pragma once

#include "IED/ConfigVariableConditionTarget.h"

#include "IED/UI/UILocalizationInterface.h"

#include "UIVariableConditionTargetSelectorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		class UIVariableConditionTargetSelectorWidget :
			public virtual UILocalizationInterface
		{
		public:
			UIVariableConditionTargetSelectorWidget(
				Localization::ILocalization& a_localization);

			bool DrawVariableConditionTargetSelectorWidget(
				Data::VariableConditionTarget& a_type);

		protected:
			const char* variable_cond_target_to_desc(
				Data::VariableConditionTarget a_type) const;

		private:
			using data_type = std::array<
				std::pair<
					Data::VariableConditionTarget,
					UIVariableConditionTargetSelectorWidgetStrings>,
				5>;

			static data_type m_data;
		};
	}

}
