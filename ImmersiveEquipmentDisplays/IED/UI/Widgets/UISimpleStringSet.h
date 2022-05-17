#pragma once

#include "IED/ConfigCommon.h"

#include "IED/UI/UICommon.h"

#include "UIDescriptionPopup.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		enum class SimpleStringSetContextResult
		{
			None  = 0,
			Paste = 1,
			Clear = 2,
			Add   = 3
		};

		class UISimpleStringSetWidget :
			public virtual UIDescriptionPopupWidget,
			public virtual UILocalizationInterface
		{
		public:
			UISimpleStringSetWidget(
				Localization::ILocalization& a_localization);

			bool DrawStringSetTree(
				const char*                   a_id,
				Localization::StringID        a_title,
				Data::configFixedStringSet_t& a_data);

		private:
			SimpleStringSetContextResult DrawContextMenu(
				Data::configFixedStringSet_t& a_data);

			bool DrawStringSetList(
				Data::configFixedStringSet_t& a_data);
		};
	}
}