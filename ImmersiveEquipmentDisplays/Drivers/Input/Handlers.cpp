#include "pch.h"

#include "Handlers.h"

namespace IED
{
	namespace Handlers
	{
		void ComboKeyPressHandler::Receive(const KeyEvent& a_evn)
		{
			if (a_evn.type == KeyEventType::KeyDown)
			{
				if (m_comboKey && a_evn.key == m_comboKey)
				{
					m_comboKeyDown = true;
				}

				if (a_evn.key == m_key && (!m_comboKey || m_comboKeyDown))
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
	}  // namespace Handlers

}  // namespace IED