#pragma once

#include "IED/ConfigData.h"

#include "IED/UI/UILocalizationInterface.h"

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