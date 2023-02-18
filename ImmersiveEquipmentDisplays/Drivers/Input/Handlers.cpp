#include "pch.h"

#include "Handlers.h"

namespace IED
{
	namespace Handlers
	{
		void ComboKeyPressHandler::Receive(const KeyEvent& a_evn)
		{
			if (a_evn.state == KeyEventState::KeyDown)
			{
				if (m_comboKey && a_evn.key == m_comboKey)
				{
					m_comboKeyDown = true;
				}

				if (m_key && a_evn.key == m_key && (!m_comboKey || m_comboKeyDown))
				{
					Trigger();
				}
			}
			else
			{
				if (m_comboKey && a_evn.key == m_comboKey)
				{
					m_comboKeyDown = false;
				}
			}
		}

		bool KeyPressHandler::CanProcess() const noexcept
		{
			return m_processPaused || !Game::IsPaused();
		}

		void KeyPressHandler::Trigger() const
		{
			if (CanProcess())
			{
				m_func();
			}
		}
	}
}