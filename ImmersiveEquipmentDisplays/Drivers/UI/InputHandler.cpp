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

			switch (m_type)
			{
			case KeyEventType::Keyboard:

				if (m_keys.key < std::size(io.KeysDown))
				{
					io.KeysDown[m_keys.key] = true;
				}

				io.AddInputCharacterUTF16(m_keys.chr);

				break;
			case KeyEventType::Mouse:

				if (m_keys.key < std::size(io.MouseDown))
				{
					io.MouseDown[m_keys.key] = true;
				}

				break;
			case KeyEventType::Wheel:

				io.MouseWheel += m_wheel.delta;

				break;
			}
		}

		void UIInputHandler::KeyEventTaskRelease::Run()
		{
			auto& io = ImGui::GetIO();

			switch (m_type)
			{
			case KeyEventType::Keyboard:

				if (m_key < std::size(io.KeysDown))
				{
					io.KeysDown[m_key] = false;
				}

				break;
			case KeyEventType::Mouse:

				if (m_key < std::size(io.MouseDown))
				{
					io.MouseDown[m_key] = false;
				}

				break;
			}
		}

		void UIInputHandler::ProcessEvent(const Handlers::KeyEvent& a_evn)
		{
			switch (a_evn.key)
			{
			case InputMap::kMacro_MouseButtonOffset:

				if (a_evn.state == KeyEventState::KeyDown)
				{
					GetKeyPressQueue().AddTask(KeyEventType::Mouse, 0);
				}
				else
				{
					GetKeyReleaseQueue().AddTask(KeyEventType::Mouse, 0);
				}
				break;
			case InputMap::kMacro_MouseButtonOffset + 1:
				if (a_evn.state == KeyEventState::KeyDown)
				{
					GetKeyPressQueue().AddTask(KeyEventType::Mouse, 1);
				}
				else
				{
					GetKeyReleaseQueue().AddTask(KeyEventType::Mouse, 1);
				}
				break;
			case InputMap::kMacro_MouseButtonOffset + 2:
				if (a_evn.state == KeyEventState::KeyDown)
				{
					GetKeyPressQueue().AddTask(KeyEventType::Mouse, 2);
				}
				else
				{
					GetKeyReleaseQueue().AddTask(KeyEventType::Mouse, 2);
				}
				break;
			case InputMap::kMacro_MouseWheelOffset:
				GetKeyPressQueue().AddTask(1.0f);
				break;
			case InputMap::kMacro_MouseWheelOffset + 1:
				GetKeyPressQueue().AddTask(-1.0f);
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
					vkCode = MapVirtualKeyExW(a_evn.key, MAPVK_VSC_TO_VK, layout);
					break;
				}

				if (!vkCode || vkCode >= std::size(GetKeyStateData()))
				{
					return;
				}

				SetKeyState(a_evn.state, a_evn.key, vkCode);

				if (Game::IsPaused())
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

				if (a_evn.state == KeyEventState::KeyDown)
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
							m_dk    = cbuf[0];
							cbuf[0] = 0;
						}
					}
					else if (n == 1)
					{
						if (m_dk)
						{
							const WCHAR srcStr[3]{ cbuf[0], m_dk.comb(), 0 };
							WCHAR       dstStr[3]{ 0 };

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
					GetKeyReleaseQueue().AddTask(KeyEventType::Keyboard, vkCode);
				}

				break;
			}
		}

		void UIInputHandler::ProcessPressQueue()
		{
			GetKeyPressQueue().ProcessTasks();
		}

		void UIInputHandler::ProcessReleaseQueue()
		{
			GetKeyReleaseQueue().ProcessTasks();
		}

		void UIInputHandler::ResetInput()
		{
			ResetKeyState();
			m_dk.clear();

			m_keyPressQueue.ClearTasks();
			m_keyReleaseQueue.ClearTasks();

			auto& io = ImGui::GetIO();

			std::memset(io.KeysDown, 0x0, sizeof(io.KeysDown));
			std::memset(io.MouseDown, 0x0, sizeof(io.MouseDown));

			io.MouseWheel = 0.0f;
			io.KeyCtrl    = false;
			io.KeyShift   = false;
			io.KeyAlt     = false;
		}
	}
}
