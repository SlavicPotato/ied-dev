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
				return !io.WantCaptureMouse && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

				/*auto& io = ImGui::GetIO();
				return !io.WantCaptureMouse && io.MouseClicked[0];*/
			}

			inline bool IsMouseReleased() noexcept
			{
				auto& io = ImGui::GetIO();
				return !io.WantCaptureMouse && io.MouseReleased[ImGuiMouseButton_Left];
			}

			inline bool IsMouseDown() noexcept
			{
				auto& io = ImGui::GetIO();
				return !io.WantCaptureMouse && io.MouseDown[ImGuiMouseButton_Left];
			}

			inline bool IsMouseInputValid() noexcept
			{
				auto& io = ImGui::GetIO();
				return io.MousePos.x != -FLT_MAX && io.MousePos.y != -FLT_MAX;
			}
		}
	}
}