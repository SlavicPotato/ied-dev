#pragma once

#include "../UIData.h"

namespace IED
{
	namespace UI
	{
		class UIControlKeySelectorWidget
		{
		public:
			bool DrawKeySelector(
				const char*                      a_label,
				const UIData::controlDescData_t& a_data,
				std::uint32_t&                   a_key,
				bool                             a_allowNone = false);
		};
	}
}
