#include "pch.h"

#include "I3DIActorContext.h"

#include "I3DIInputHelpers.h"

#include "Drivers/UI.h"

namespace IED
{
	namespace UI
	{
		namespace I3DI
		{
			bool ShouldBlockInput() noexcept
			{
				return Drivers::UI::IsImInputBlocked() ||
				       ImGui::GetIO().WantCaptureMouse;
			}

			bool IsMouseClicked() noexcept
			{
				const auto& io = ImGui::GetIO();
				return !io.WantCaptureMouse &&
				       io.MouseClicked[ImGuiMouseButton_Left] &&
				       !io.MouseDownOwned[ImGuiMouseButton_Left];
			}

			bool IsMouseReleased() noexcept
			{
				const auto& io = ImGui::GetIO();
				return !io.WantCaptureMouse &&
				       io.MouseReleased[ImGuiMouseButton_Left] &&
				       !io.MouseDownOwned[ImGuiMouseButton_Left];
			}

			bool IsMouseDown() noexcept
			{
				const auto& io = ImGui::GetIO();
				return !io.WantCaptureMouse &&
				       io.MouseDown[ImGuiMouseButton_Left] &&
				       !io.MouseDownOwned[ImGuiMouseButton_Left];
			}

			bool IsMouseInputValid() noexcept
			{
				const auto& io = ImGui::GetIO();
				return io.MousePos.x != -FLT_MAX && io.MousePos.y != -FLT_MAX;
			}
		}
	}
}