#pragma once

#include "UIFormTypeSelectorWidgetStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIFormTypeSelectorWidget
		{
			using filter_func_t = std::function<bool(std::uint8_t)>;

		public:
			static bool DrawFormTypeSelector(
				stl::optional<std::uint8_t>& a_type,
				filter_func_t                a_filter);

			static const char* form_type_to_desc(std::uint8_t a_type);
		};

	}

}
