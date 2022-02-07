#include "pch.h"

#include "InputHandler.h"

#include <ext/GameCommon.h>

namespace IED
{
	namespace Drivers
	{
		void UIInputHandler::KeyEventTaskPress::Run()
		{
			auto& io = ImGui::GetIO();

			if (m_vkey < std::size(io.KeysDown))
			{
				io.KeysDown[m_vkey] = true;
			}

			io.AddInputCharacterUTF16(m_char);
		}

		void UIInputHandler::KeyEventTaskRelease::Run()
		{
			auto& io = ImGui::GetIO();

			if (m_vkey < std::size(io.KeysDown))
			{
				io.KeysDown[m_vkey] = false;
			}
		}

		void UIInputHandler::ProcessEvent(const Handlers::KeyEvent& a_evn)
		{
			switch (a_evn.key)
			{
			case InputMap::kMacro_MouseButtonOffset:
				if (a_evn.type == KeyEventType::KeyDown)
				{
					GetMousePressEventQueue().AddMouseButtonEvent(0, true);
				}
				else
				{
					GetMouseReleaseEventQueue().AddMouseButtonEvent(0, false);
				}
				break;
			case InputMap::kMacro_MouseButtonOffset + 1:
				if (a_evn.type == KeyEventType::KeyDown)
				{
					GetMousePressEventQueue().AddMouseButtonEvent(1, true);
				}
				else
				{
					GetMouseReleaseEventQueue().AddMouseButtonEvent(1, false);
				}
				break;
			case InputMap::kMacro_MouseButtonOffset + 2:
				if (a_evn.type == KeyEventType::KeyDown)
				{
					GetMousePressEventQueue().AddMouseButtonEvent(2, true);
				}
				else
				{
					GetMouseReleaseEventQueue().AddMouseButtonEvent(2, false);
				}
				break;
			case InputMap::kMacro_MouseWheelOffset:
				GetMousePressEventQueue().AddMouseWheelEvent(1.0f);
				break;
			case InputMap::kMacro_MouseWheelOffset + 1:
				GetMousePressEventQueue().AddMouseWheelEvent(-1.0f);
				break;
			default:
				if (a_evn.key >= InputMap::kMacro_NumKeyboardKeys)
				{
					return;
				}

				auto layout = GetKeyboardLayout(0);

				UINT vkCode;

				switch (a_evn.key)
				{
				case DIK_LEFT:
					vkCode = VK_LEFT;
					break;
				case DIK_RIGHT:
					vkCode = VK_RIGHT;
					break;
				case DIK_UP:
					vkCode = VK_UP;
					break;
				case DIK_DOWN:
					vkCode = VK_DOWN;
					break;
				case DIK_DELETE:
					vkCode = VK_DELETE;
					break;
				case DIK_LMENU:
				case DIK_RMENU:
					vkCode = VK_MENU;
					break;
				case DIK_LCONTROL:
				case DIK_RCONTROL:
					vkCode = VK_CONTROL;
					break;
				case DIK_LSHIFT:
				case DIK_RSHIFT:
					vkCode = VK_SHIFT;
					break;
				default:
					vkCode = MapVirtualKeyExW(a_evn.key, MAPVK_VSC_TO_VK, nullptr);
					break;
				}

				if (!vkCode || vkCode >= std::size(GetKeyStateData()))
				{
					return;
				}

				SetKeyState(a_evn.type, a_evn.key, vkCode);

				if (Game::InPausedMenu())
				{
					ClearKeyState(VK_CAPITAL);
					ClearKeyState(VK_SCROLL);
					ClearKeyState(VK_NUMLOCK);
				}
				else
				{
					UpdateKeyState(VK_CAPITAL);
					UpdateKeyState(VK_SCROLL);
					UpdateKeyState(VK_NUMLOCK);
				}

				if (a_evn.type == KeyEventType::KeyDown)
				{
					WCHAR cbuf[3]{ 0 };

					int n = ToUnicodeEx(
						vkCode,
						a_evn.key,
						GetKeyStateData(),
						cbuf,
						2,
						0,
						layout);

					if (n == -1 || n == 2)
					{
						if (m_dk)
						{
							GetKeyPressQueue().AddTask(vkCode, m_dk.code());
							m_dk.clear();
						}
						else
						{
							m_dk = cbuf[0];
							cbuf[0] = 0;
						}
					}
					else if (n == 1)
					{
						if (m_dk)
						{
							const WCHAR srcStr[3]{ cbuf[0], m_dk.comb(), 0 };
							WCHAR dstStr[3]{ 0 };

							if (auto l = FoldStringW(
									MAP_PRECOMPOSED,
									srcStr,
									std::size(srcStr),
									dstStr,
									std::size(dstStr));
							    l == 2)
							{
								cbuf[0] = dstStr[0];
							}
							else
							{
								GetKeyPressQueue().AddTask(vkCode, m_dk.code());
							}

							m_dk.clear();
						}
					}
					else
					{
						cbuf[0] = 0;
						m_dk.clear();
					}

					GetKeyPressQueue().AddTask(vkCode, cbuf[0]);
				}
				else
				{
					GetKeyReleaseQueue().AddTask(vkCode);
				}

				break;
			}
		}

		void UIInputHandler::ProcessPressQueues()
		{
			GetKeyPressQueue().ProcessTasks();
			GetMousePressEventQueue().ProcessEvents();
		}

		void UIInputHandler::ProcessReleaseQueues()
		{
			GetKeyReleaseQueue().ProcessTasks();
			GetMouseReleaseEventQueue().ProcessEvents();
		}

		void UIInputHandler::ResetInput()
		{
			ResetKeyState();
			m_dk.clear();

			m_keyPressQueue.ClearTasks();
			m_keyReleaseQueue.ClearTasks();

			m_mousePressQueue.Clear();
			m_mouseReleaseQueue.Clear();
		}
	}
}
