#pragma once

#include "UIPackageTypeSelectorWidgetStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIPackageTypeSelectorWidget :
			public virtual UILocalizationInterface
		{
		public:
			UIPackageTypeSelectorWidget(Localization::ILocalization& a_localization);

			bool DrawPackageTypeSelector(
				PACKAGE_PROCEDURE_TYPE& a_type);

		protected:
			const char* procedure_type_to_desc(
				PACKAGE_PROCEDURE_TYPE a_type) const;

		private:
			using data_type = std::array<
				std::pair<
					PACKAGE_PROCEDURE_TYPE,
					UIPackageTypeSelectorWidgetStrings>,
				40>;

			static data_type m_data;
		};
	}

}
