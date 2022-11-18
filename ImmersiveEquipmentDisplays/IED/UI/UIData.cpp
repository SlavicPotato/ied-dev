#include "pch.h"

#include "UIData.h"

namespace IED
{
	namespace UI
	{
		namespace UIData
		{
			const controlDescData_t g_comboControlMap(
				{

					{ 0, "None" },
					{ DIK_LSHIFT, "Left shift" },
					{ DIK_RSHIFT, "Right shift" },
					{ DIK_LCONTROL, "Left control" },
					{ DIK_RCONTROL, "Right control" },
					{ DIK_LALT, "Left alt" },
					{ DIK_RALT, "Right alt" }

				});

			const controlDescData_t g_controlMap(
				{

					{ 0, "None" },
					{ DIK_INSERT, "Insert" },
					{ DIK_DELETE, "Delete" },
					{ DIK_HOME, "Home" },
					{ DIK_END, "End" },
					{ DIK_PGUP, "Page up" },
					{ DIK_PGDN, "Page down" },
					{ DIK_BACKSPACE, "Backspace" },
					{ DIK_RETURN, "Enter" },
					{ DIK_PAUSE, "Pause" },
					{ DIK_CAPSLOCK, "Caps lock" },
					{ DIK_LEFT, "Left" },
					{ DIK_RIGHT, "Right" },
					{ DIK_UP, "Up" },
					{ DIK_DOWN, "Down" },
					{ DIK_TAB, "Tab" },
					{ DIK_F1, "F1" },
					{ DIK_F2, "F2" },
					{ DIK_F3, "F3" },
					{ DIK_F4, "F4" },
					{ DIK_F5, "F5" },
					{ DIK_F6, "F6" },
					{ DIK_F7, "F7" },
					{ DIK_F8, "F8" },
					{ DIK_F9, "F9" },
					{ DIK_F10, "F10" },
					{ DIK_F11, "F11" },
					{ DIK_F12, "F12" },
					{ DIK_F13, "F13" },
					{ DIK_F14, "F14" },
					{ DIK_F15, "F15" },
					{ DIK_NUMPAD0, "Num 0" },
					{ DIK_NUMPAD1, "Num 1" },
					{ DIK_NUMPAD2, "Num 2" },
					{ DIK_NUMPAD3, "Num 3" },
					{ DIK_NUMPAD4, "Num 4" },
					{ DIK_NUMPAD5, "Num 5" },
					{ DIK_NUMPAD6, "Num 6" },
					{ DIK_NUMPAD7, "Num 7" },
					{ DIK_NUMPAD8, "Num 8" },
					{ DIK_NUMPAD9, "Num 9" },
					{ DIK_NUMPADSLASH, "Num /" },
					{ DIK_NUMPADSTAR, "Num *" },
					{ DIK_NUMPADMINUS, "Num -" },
					{ DIK_NUMPADPLUS, "Num +" },
					{ DIK_NUMPADENTER, "Num Enter" },
					{ DIK_NUMPADCOMMA, "Num ," },
					{ DIK_PERIOD, "." },
					{ DIK_COMMA, "," },
					{ DIK_MINUS, "-" },
					{ DIK_BACKSLASH, "\\" },
					{ DIK_COLON, ":" },
					{ DIK_SEMICOLON, ";" },
					{ DIK_SLASH, "/" },
					{ DIK_0, "0" },
					{ DIK_1, "1" },
					{ DIK_2, "2" },
					{ DIK_3, "3" },
					{ DIK_4, "4" },
					{ DIK_5, "5" },
					{ DIK_6, "6" },
					{ DIK_7, "7" },
					{ DIK_8, "8" },
					{ DIK_9, "9" },
					{ DIK_A, "A" },
					{ DIK_B, "B" },
					{ DIK_C, "C" },
					{ DIK_D, "D" },
					{ DIK_E, "E" },
					{ DIK_F, "F" },
					{ DIK_G, "G" },
					{ DIK_H, "H" },
					{ DIK_I, "I" },
					{ DIK_J, "J" },
					{ DIK_K, "K" },
					{ DIK_L, "L" },
					{ DIK_M, "M" },
					{ DIK_N, "N" },
					{ DIK_O, "O" },
					{ DIK_P, "P" },
					{ DIK_Q, "Q" },
					{ DIK_R, "R" },
					{ DIK_S, "S" },
					{ DIK_T, "T" },
					{ DIK_U, "U" },
					{ DIK_V, "V" },
					{ DIK_W, "W" },
					{ DIK_X, "X" },
					{ DIK_Y, "Y" },
					{ DIK_Z, "Z" },
					{ DIK_LSHIFT, "Left shift" },
					{ DIK_RSHIFT, "Right shift" },
					{ DIK_LCONTROL, "Left control" },
					{ DIK_RCONTROL, "Right control" },
					{ DIK_LALT, "Left alt" },
					{ DIK_RALT, "Right alt" },
					{ InputMap::kMacro_MouseButtonOffset + 0, "Mouse left" },
					{ InputMap::kMacro_MouseButtonOffset + 1, "Mouse right" },
					{ InputMap::kMacro_MouseButtonOffset + 2, "Mouse middle" },
					{ InputMap::kMacro_MouseButtonOffset + 3, "Mouse 4" },
					{ InputMap::kMacro_MouseButtonOffset + 4, "Mouse 5" },
					{ InputMap::kMacro_MouseButtonOffset + 5, "Mouse 6" },
					{ InputMap::kMacro_MouseButtonOffset + 6, "Mouse 7" },
					{ InputMap::kMacro_MouseButtonOffset + 7, "Mouse 8" },

				});

			bool& UICollapsibleStates::get(ImGuiID a_key, bool a_default)
			{
				return m_data.emplace(a_key, a_default).first->second;
			}

			void UICollapsibleStates::insert(ImGuiID a_key, bool a_value)
			{
				m_data.emplace(a_key, a_value);
			}

			bool& UICollapsibleStates::operator[](ImGuiID a_key)
			{
				return get(a_key);
			}

			bool UICollapsibleStates::erase(ImGuiID a_key) noexcept
			{
				return m_data.erase(a_key) > 0;
			}

			void UICollapsibleStates::clear() noexcept
			{
				m_data.clear();
			}

		}
	}
}