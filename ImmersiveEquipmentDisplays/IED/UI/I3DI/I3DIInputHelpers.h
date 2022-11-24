#pragma once

namespace IED
{
	namespace UI
	{
		namespace I3DI
		{
			bool ShouldBlockInput() noexcept;
			bool IsMouseClicked(ImGuiMouseButton a_button = ImGuiMouseButton_Left) noexcept;
			bool IsMouseReleased(ImGuiMouseButton a_button = ImGuiMouseButton_Left) noexcept;
			bool IsMouseDown(ImGuiMouseButton a_button = ImGuiMouseButton_Left) noexcept;
			bool IsMouseInputValid() noexcept;
		}
	}
}