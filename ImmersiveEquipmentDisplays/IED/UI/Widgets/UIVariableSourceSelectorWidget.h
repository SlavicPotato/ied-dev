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

		protected:
			const char* variable_source_to_desc(
				Data::VariableSource a_type) const;

		private:
			using data_type = std::array<
				std::pair<
					Data::VariableSource,
					UIVariableSourceSelectorWidgetStrings>,
				4>;

			static data_type m_data;
		};
	}

}
