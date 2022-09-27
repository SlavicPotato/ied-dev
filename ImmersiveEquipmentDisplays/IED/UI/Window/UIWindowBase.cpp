#include "pch.h"

#include "UIWindowBase.h"

namespace IED
{
	namespace UI
	{
		void UIChildWindowBase::SetOpenState(bool a_state, bool a_notify)
		{
			if (m_openState != a_state)
			{
				m_openState = a_state;
				m_stateChanged = true;
			}
		}

		void UIChildWindowBase::ToggleOpenState(bool a_notify)
		{
			m_openState = !m_openState;
			m_stateChanged = true;
		}

		void UIChildWindowBase::DrawWrapper()
		{
			if (!m_stateChanged)
			{
				if (!IsWindowOpen())
				{
					return;
				}
			}
			else
			{
				m_stateChanged = false;

				if (IsWindowOpen())
				{
					OnOpen();
				}
			}

			Draw();

			if (!IsWindowOpen())
			{
				OnClose();
			}
		}
	}
}