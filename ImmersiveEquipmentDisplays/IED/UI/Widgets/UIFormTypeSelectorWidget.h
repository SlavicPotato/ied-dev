#pragma once

#include "UIFormTypeSelectorWidgetStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIFormTypeSelectorWidget :
			public virtual UILocalizationInterface
		{
			using filter_func_t = std::function<bool(std::uint8_t)>;

		public:
			UIFormTypeSelectorWidget(Localization::ILocalization& a_localization);

			bool DrawFormTypeSelector(
				stl::optional<std::uint8_t>& a_type,
				filter_func_t                a_filter);

			const char* form_type_to_desc(std::uint8_t a_type);
		};

	}

}
