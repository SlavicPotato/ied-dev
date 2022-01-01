#pragma once

#include "IED/ConfigCommon.h"

namespace IED
{
	namespace UI
	{
		class UIGlobalConfigTypeSelectorWidget
		{
		public:
			static bool DrawTypeSelectorRadio(Data::GlobalConfigType& a_value);
		};
	}
}