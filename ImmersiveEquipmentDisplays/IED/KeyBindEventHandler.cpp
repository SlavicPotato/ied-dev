#include "pch.h"

#include "KeyBindEventHandler.h"

namespace IED
{
	namespace KB
	{
		void KeyBindEventHandler::Receive(const Handlers::KeyEvent& a_evn)
		{
			if (Game::IsPaused())
			{
				return;
			}

			bool changed = false;

			m_data->visit([&](auto& a_e) noexcept {
				if (a_e.second.ProcessEvent(a_evn))
				{
					changed = true;
				}
			});

			if (changed)
			{
				OnKBStateChanged();
			}
		}

	}
}