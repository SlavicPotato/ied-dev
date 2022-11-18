#pragma once

namespace IED
{
	namespace UI
	{
		namespace I3DI
		{
			bool ShouldBlockInput() noexcept;
			bool IsMouseClicked() noexcept;
			bool IsMouseReleased() noexcept;
			bool IsMouseDown() noexcept;
			bool IsMouseInputValid() noexcept;
		}
	}
}