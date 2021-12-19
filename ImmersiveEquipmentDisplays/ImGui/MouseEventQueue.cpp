#include "pch.h"

#include "MouseEventQueue.h"

namespace IED
{
	void ImGuiMouseEventQueue::ProcessEvents()
	{
		IScopedLock _(m_lock);

		auto& io = ImGui::GetIO();

		std::uint8_t mouse_button_changed_mask(0x0);

		while (!m_queue.empty())
		{
			auto evt = m_queue.front();
			m_queue.pop();

			if (evt.m_type == ImGuiIoEventType::MouseButton)
			{
				auto button = evt.m_mouseButton.button;

				if (io.MouseDown[button] != evt.m_mouseButton.down)
				{
					std::uint8_t mask = (1ui8 << button);

					if (mouse_button_changed_mask & mask)
					{
						break;
					}

					mouse_button_changed_mask |= mask;

					io.MouseDown[button] = evt.m_mouseButton.down;
				}
			}
			else if (evt.m_type == ImGuiIoEventType::MouseWheel)
			{
				io.MouseWheel += evt.m_mouseWheel.value;
			}
		}
	}
}  // namespace IED