#include "pch.h"

#include "KeyState.h"

namespace IED
{
	void KeyState::UpdateKeyState(UINT a_key)
	{
		m_data[a_key] =
			((m_data[a_key] & 0xFE) |
		     static_cast<BYTE>(::GetKeyState(a_key) & 0x1));
	}

	void KeyState::ResetKeyState()
	{
		std::memset(m_data, 0, sizeof(m_data));
	}

	void KeyState::SetKeyState(KeyEventState a_ev, std::uint32_t a_sc, UINT a_vk)
	{
		SetKeyState(a_ev, a_vk);
		SetLRKeyState(a_ev, a_sc, a_vk);
	}

	void KeyState::SetKeyState(KeyEventState a_ev, UINT a_key)
	{
		if (a_ev == KeyEventState::KeyUp)
		{
			m_data[a_key] &= ~KS_MS_BIT;
		}
		else
		{
			m_data[a_key] |= KS_MS_BIT;
			m_data[a_key] ^= KS_LS_BIT;
		}
	}

	void KeyState::ClearKeyState(UINT a_key)
	{
		m_data[a_key] = 0;
	}

	void KeyState::SetLRKeyState(KeyEventState a_ev, std::uint32_t a_sc, UINT a_vk)
	{
		switch (a_vk)
		{
		case VK_MENU:

			if (a_sc == DIK_LMENU)
			{
				SetKeyState(a_ev, VK_LMENU);
			}
			else
			{
				SetKeyState(a_ev, VK_RMENU);
				SetKeyState(a_ev, VK_CONTROL);
				SetKeyState(a_ev, VK_LCONTROL);
			}

			break;

		case VK_CONTROL:

			if (a_sc == DIK_LCONTROL)
			{
				SetKeyState(a_ev, VK_LCONTROL);
			}
			else
			{
				SetKeyState(a_ev, VK_RCONTROL);
			}

			break;

		case VK_SHIFT:

			if (a_sc == DIK_LSHIFT)
			{
				SetKeyState(a_ev, VK_LSHIFT);
			}
			else
			{
				SetKeyState(a_ev, VK_RSHIFT);
			}

			break;
		}
	}

}