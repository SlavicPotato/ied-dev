#pragma once

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	struct formInfoResult_t;
	struct objectEntryBase_t;

	namespace UI
	{
		class UIFormInfoTooltipWidget :
			public virtual UILocalizationInterface
		{
		public:
			UIFormInfoTooltipWidget(Localization::ILocalization& a_localization);

			void DrawFormInfoTooltip(
				const formInfoResult_t&  a_info,
				const objectEntryBase_t& a_entry);

			void DrawObjectEntryHeaderInfo(
				const formInfoResult_t*  a_info,
				const objectEntryBase_t& a_entry);
		};
	}
}