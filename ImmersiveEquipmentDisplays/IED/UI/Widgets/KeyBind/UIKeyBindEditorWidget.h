#pragma once

#include "IED/ConfigKeybind.h"

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
			[[nodiscard]] void DrawKeyBindEditorWidget(Data::configKeybindEntryHolder_t& a_data);
			[[nodiscard]] void DrawKeyBindActionItems(Data::configKeybindEntryHolder_t& a_data);

		private:
			void                         DrawList(Data::configKeybindEntryHolder_t& a_data);
			UIKeyBindEditorContextAction DrawEntryTreeContextMenu(Data::configKeybindEntryHolder_t::container_type::value_type& a_data);
			void                         DrawEntryTree(Data::configKeybindEntryHolder_t::container_type::value_type& a_data);
			void                         DrawEntry(Data::configKeybindEntryHolder_t::container_type::value_type& a_data);

			virtual void OnKeybindErase(const Data::configKeybindEntryHolder_t::container_type::key_type& a_key);
			virtual void OnKeybindAdd(const Data::configKeybindEntryHolder_t::container_type::value_type& a_key);
			virtual void OnKeybindChange(const Data::configKeybindEntryHolder_t::container_type::value_type& a_data);

			virtual bool GetKeybindState(
				const Data::configKeybindEntryHolder_t::container_type::key_type& a_key,
				std::uint32_t&                                                    a_stateOut);

			std::string m_tmpID;
		};
	}
}