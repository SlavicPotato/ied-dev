#pragma once

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		class UIKeyBindIDSelectorWidget
		{
		public:
			using filter_func_t = std::function<bool(std::uint8_t)>;

			static bool DrawKeyBindIDSelector(stl::fixed_string& a_out);
			static bool DrawKeyBindIDSelector(std::string& a_out);
		};
	}
}