#pragma once

#include "IED/ConfigCommon.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		class UIGlobalConfigTypeSelectorWidget : 
			public virtual UILocalizationInterface
		{
		public:

			UIGlobalConfigTypeSelectorWidget(
				Localization::ILocalization& a_localization);

			bool DrawTypeSelectorRadio(Data::GlobalConfigType& a_value);
		};
	}
}