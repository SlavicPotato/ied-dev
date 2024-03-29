#include "pch.h"

#include "UIContext.h"

namespace IED
{
	namespace UI
	{
		void UIContext::SetOpenState(bool a_state, bool a_notify)
		{
			if (m_openState != a_state)
			{
				if (OnWantOpenStateChange(a_state))
				{
					m_openState    = a_state;
					m_stateChanged = true;
				}

				//OnOpenStateChanged(a_state);
			}
		}

		void UIContext::ToggleOpenState(bool a_notify)
		{
			const bool newState = !m_openState;

			if (OnWantOpenStateChange(newState))
			{
				m_openState    = newState;
				m_stateChanged = true;
			}

			//OnOpenStateChanged(m_openState);
		}

		void UIContext::DrawWrapper()
		{
			if (!m_stateChanged)
			{
				if (!IsContextOpen())
				{
					return;
				}
			}
			else
			{
				m_stateChanged = false;

				if (IsContextOpen())
				{
					OnOpen();
					SendOpenStateEvent();
				}
			}

			Draw();

			if (!IsContextOpen())
			{
				OnClose();
				SendOpenStateEvent();
			}
		}

		void UIContext::SendOpenStateEvent()
		{
			const UIContextStateChangeEvent evn{
				*this,
				m_openState
			};

			SendEvent(evn);
		}
	}
}