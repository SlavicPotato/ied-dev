#pragma once

#include "IED/ConfigCommon.h"

#include "IED/UI/UICommon.h"

#include "UIDescriptionPopup.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		enum class SimpleStringListContextResult
		{
			None  = 0,
			Paste = 1,
			Clear = 2,
			Add   = 3
		};

		class UISimpleStringListWidget :
			public virtual UIDescriptionPopupWidget,
			public virtual UILocalizationInterface
		{
		public:
			UISimpleStringListWidget(
				Localization::ILocalization& a_localization);

			bool DrawStringListTree(
				const char*                    a_id,
				Localization::StringID         a_title,
				Data::configFixedStringList_t& a_data,
				ImGuiTreeNodeFlags             a_treeFlags = ImGuiTreeNodeFlags_SpanAvailWidth);

		private:
			SimpleStringListContextResult DrawContextMenu(
				Data::configFixedStringList_t& a_data);

			bool DrawStringListTree(
				Data::configFixedStringList_t& a_data);
		};
	}
}