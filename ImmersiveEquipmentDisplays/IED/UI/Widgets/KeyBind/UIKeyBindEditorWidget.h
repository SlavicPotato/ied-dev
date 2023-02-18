#pragma once

#include "IED/KeyToggleStateEntryHolder.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIKeyBindEditorWidget 
		{
			enum class UIKeyBindEditorContextAction
			{
				None = 0,

				Delete = 1
			};

		public:
			bool DrawKeyBindEditorWidget(KB::KeyToggleStateEntryHolder& a_data);

		private:
			bool                         DrawList(KB::KeyToggleStateEntryHolder& a_data);
			UIKeyBindEditorContextAction DrawEntryTreeContextMenu(KB::KeyToggleStateEntryHolder::container_type::value_type& a_data);
			bool                         DrawEntryTree(KB::KeyToggleStateEntryHolder::container_type::value_type& a_data);
			bool                         DrawEntry(KB::KeyToggleStateEntryHolder::container_type::value_type& a_data);
		};
	}
}