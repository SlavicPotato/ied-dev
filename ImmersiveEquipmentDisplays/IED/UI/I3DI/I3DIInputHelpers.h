#pragma once

namespace IED
{
	namespace UI
	{
		namespace I3DI
		{

			inline bool IsMouseClicked() noexcept
			{
				auto& io = ImGui::GetIO();
				return !io.WantCaptureMouse && io.MouseClicked[0];
			}

			inline bool IsMouseReleased() noexcept
			{
				auto& io = ImGui::GetIO();
				return !io.WantCaptureMouse && io.MouseReleased[0];
			}
		}
	}
}