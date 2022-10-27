#include "pch.h"

#include "I3DIPopupWindow.h"

#include "I3DICommonData.h"

#include "Common/VectorMath.h"

namespace IED
{
	namespace UI
	{
		void I3DIPopupWindow::Draw(
			const char*     a_id,
			I3DICommonData& a_data,
			func_type       a_func)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.2f });
			ImGui::PushStyleColor(ImGuiCol_Border, { 0.5f, 0.5f, 0.5f, 0.5f });
			ImGui::PushStyleColor(ImGuiCol_BorderShadow, { 0.0f, 0.0f, 0.0f, 0.0f });
			/*ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.43f, 0.43f, 0.43f, 0.39f });
			ImGui::PushStyleColor(ImGuiCol_PopupBg, { 0.11f, 0.11f, 0.14f, 0.92f });
			ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.00f, 0.00f, 0.00f, 0.00f });*/
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.0f);

			if (ImGui::Begin(
					a_id,
					nullptr,
					WINDOW_FLAGS))
			{
				ApplyPosition(a_data);

				a_func();
			}

			ImGui::End();

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			/*ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();*/
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}

		void I3DIPopupWindow::ApplyPosition(I3DICommonData& a_data)
		{
			auto window = ImGui::GetCurrentWindow();

			const auto outer  = ImGui::GetPopupAllowedExtentRect(window);
			const auto refPos = m_pos - window->Size * m_pivot;

			const auto pos = ImGui::FindBestWindowPosForPopupEx(
				refPos,
				window->Size,
				std::addressof(window->AutoPosLastDirection),
				outer,
				ImRect(refPos, refPos),
				ImGuiPopupPositionPolicy_Default);

			ImGui::SetWindowPos(window, pos, 0);

			if (m_worldOrigin)
			{
				const auto pt = pos + window->Size * ImVec2{ 0.5f, 1.0f };

				const auto wp = VectorMath::ScreenSpaceToWorld(
					a_data.scene.GetViewport(),
					DirectX::XMVectorSet(pt.x, pt.y, 0.0f, 0.0f),
					a_data.scene.GetViewMatrix(),
					a_data.scene.GetProjectionMatrix());

				a_data.batchNoDepth.AddLine(*m_worldOrigin, wp, DirectX::g_XMOneHalf.v);
			}
		}

	}
}