#pragma once

#include "IED/UI/UILocalizationInterface.h"

#include "IED/ConfigBipedObjectList.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

namespace IED
{
	namespace UI
	{
		enum class UIBipedObjectListContextAction
		{
			None,
			Add,
			Clear,
			Paste,
			Swap
		};

		struct UIBipedObjectListContextResult
		{
			UIBipedObjectListContextAction action{ UIBipedObjectListContextAction::None };
			BIPED_OBJECT                   object{ BIPED_OBJECT::kNone };
			SwapDirection                  dir;
		};

		class UIBipedObjectList
		{
		public:
			UIBipedObjectList() = default;

			bool DrawBipedObjectListTree(
				const char*                    a_strid,
				Data::configBipedObjectList_t& a_data,
				std::function<bool()>          a_extra = [] { return false; });

		private:
			UIBipedObjectListContextAction DrawHeaderContextMenu(Data::configBipedObjectList_t& a_data);
			UIBipedObjectListContextResult DrawEntryContextMenu();

			bool DrawTable(Data::configBipedObjectList_t& a_data);

			BIPED_OBJECT m_newObject{ BIPED_OBJECT::kNone };
		};
	}
}