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

			bool IsMouseClicked(ImGuiMouseButton a_button) noexcept
			{
				const auto& io = ImGui::GetIO();
				return !io.WantCaptureMouse &&
				       io.MouseClicked[a_button] &&
				       !io.MouseDownOwned[a_button];
			}

			bool IsMouseReleased(ImGuiMouseButton a_button) noexcept
			{
				const auto& io = ImGui::GetIO();
				return !io.WantCaptureMouse &&
				       io.MouseReleased[a_button] &&
				       !io.MouseDownOwned[a_button];
			}

			bool IsMouseDown(ImGuiMouseButton a_button) noexcept
			{
				const auto& io = ImGui::GetIO();
				return !io.WantCaptureMouse &&
				       io.MouseDown[a_button] &&
				       !io.MouseDownOwned[a_button];
			}

			bool IsMouseInputValid() noexcept
			{
				const auto& io = ImGui::GetIO();
				return io.MousePos.x != -FLT_MAX && io.MousePos.y != -FLT_MAX;
			}
		}
	}
}