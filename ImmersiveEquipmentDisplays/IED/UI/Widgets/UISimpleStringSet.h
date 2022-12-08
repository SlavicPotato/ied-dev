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
			public virtual UIDescriptionPopupWidget
		{
		public:
			UISimpleStringSetWidget() = default;

			bool DrawStringSetTree(
				const char*                   a_id,
				Localization::StringID        a_title,
				Data::configFixedStringSet_t& a_data,
				ImGuiTreeNodeFlags            a_treeFlags = 
				ImGuiTreeNodeFlags_SpanAvailWidth);

		private:
			SimpleStringSetContextResult DrawContextMenu(
				Data::configFixedStringSet_t& a_data);

			bool DrawStringSetTree(
				Data::configFixedStringSet_t& a_data);
		};
	}
}