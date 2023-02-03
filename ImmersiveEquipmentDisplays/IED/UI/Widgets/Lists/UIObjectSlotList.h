#pragma once

#include "IED/UI/UILocalizationInterface.h"

#include "IED/ConfigCommon.h"

#include "IED/UI/Widgets/UIWidgetsCommon.h"

namespace IED
{
	namespace UI
	{
		enum class UIObjectSlotListContextAction
		{
			None,
			Add,
			Clear,
			Paste,
			Swap
		};

		struct UIObjectSlotListContextResult
		{
			UIObjectSlotListContextAction action{ UIObjectSlotListContextAction::None };
			Data::ObjectSlot              object{ Data::ObjectSlot::kMax };
			SwapDirection                 dir;
		};

		class UIObjectSlotList
		{
		public:
			UIObjectSlotList() = default;

			bool DrawObjectSlotListTree(
				const char*                   a_strid,
				Data::configObjectSlotList_t& a_data,
				std::function<bool()>         a_extra = [] { return false; });

		private:
			UIObjectSlotListContextAction DrawHeaderContextMenu(Data::configObjectSlotList_t& a_data);
			UIObjectSlotListContextResult DrawEntryContextMenu();

			bool DrawTable(Data::configObjectSlotList_t& a_data);

			Data::ObjectSlot m_newObject{ Data::ObjectSlot::kMax };
		};
	}
}