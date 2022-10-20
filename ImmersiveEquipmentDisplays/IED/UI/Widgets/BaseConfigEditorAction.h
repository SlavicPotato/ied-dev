#pragma once

namespace IED
{
	namespace UI
	{

		enum class BaseConfigEditorAction : std::uint8_t
		{
			None,
			Insert,
			InsertGroup,
			Delete,
			Swap,
			Edit,
			Reset,
			Rename,
			ClearKeyword,
			Copy,
			Paste,
			PasteOver,
			Create
		};

	}
}