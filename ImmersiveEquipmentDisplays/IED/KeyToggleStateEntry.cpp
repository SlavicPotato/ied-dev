#include "pch.h"

#include "KeyToggleStateEntry.h"

namespace IED
{
	namespace KB
	{
		bool KeyToggleStateEntry::ProcessEvent(
			const Handlers::KeyEvent& a_evn) noexcept
		{
			if (a_evn.state == KeyEventState::KeyDown)
			{
				if (comboKey && a_evn.key == comboKey)
				{
					comboKeyDown = true;
				}

				if (key && a_evn.key == key && (!comboKey || comboKeyDown))
				{					
					state = ++state > numStates ? 0 : state;
					return true;
				}
			}
			else
			{
				if (comboKey && a_evn.key == comboKey)
				{
					comboKeyDown = false;
				}
			}

			return false;
		}
	}
}