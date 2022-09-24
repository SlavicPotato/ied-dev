#pragma once

#include "IED/UI/Widgets/UIFormTypeSelectorWidget.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	struct formInfoResult_t;
	struct ObjectEntryBase;

	namespace UI
	{
		class UIFormInfoTooltipWidget :
			public virtual UIFormTypeSelectorWidget,
			public virtual UILocalizationInterface
		{
		public:
			UIFormInfoTooltipWidget(Localization::ILocalization& a_localization);

			void DrawFormInfoTooltip(
				const formInfoResult_t*  a_info,
				const ObjectEntryBase& a_entry);

			void DrawObjectEntryHeaderInfo(
				const formInfoResult_t*  a_info,
				const ObjectEntryBase& a_entry);
		};
	}
}