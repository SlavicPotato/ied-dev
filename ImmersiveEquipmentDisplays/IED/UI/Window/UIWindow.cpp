#include "pch.h"

#include "UIWindow.h"

namespace IED
{
	namespace UI
	{
		void UIWindow::SetWindowDimensions(
			float a_offsetX,
			float a_sizeX,
			float a_sizeY,
			bool  a_centered)
		{
			if (!m_sizeData.initialized)
			{
				auto& io = ImGui::GetIO();

				m_sizeData.sizeMin = {
					std::min(300.0f, io.DisplaySize.x - 40.0f),
					std::min(200.0f, io.DisplaySize.y - 40.0f)
				};

				m_sizeData.sizeMax = {
					io.DisplaySize.x,
					std::max(io.DisplaySize.y - 40.0f, m_sizeData.sizeMin.y)
				};

				if (a_centered)
				{
					m_sizeData.pos   = { io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f };
					m_sizeData.pivot = { 0.5f, 0.5f };
				}
				else
				{
					m_sizeData.pos = {
						std::min(20.0f + a_offsetX, io.DisplaySize.x - 40.0f),
						20.0f
					};
				}

				m_sizeData.size = {
					a_sizeX < 0.0f ? 450.0f : a_sizeX,
					a_sizeY < 0.0f ? io.DisplaySize.y : a_sizeY
				};

				m_sizeData.initialized = true;
			}

			if (a_centered)
			{
				ImGui::SetNextWindowPos(
					m_sizeData.pos,
					ImGuiCond_FirstUseEver,
					m_sizeData.pivot);
			}
			else
			{
				ImGui::SetNextWindowPos(m_sizeData.pos, ImGuiCond_FirstUseEver);
			}

			ImGui::SetNextWindowSize(m_sizeData.size, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSizeConstraints(m_sizeData.sizeMin, m_sizeData.sizeMax);
		}

		bool UIWindow::CanClip() const
		{
			auto window = ImGui::GetCurrentWindow();
			IM_ASSERT(window != nullptr);
			return window->SkipItems;
		}

	}
}