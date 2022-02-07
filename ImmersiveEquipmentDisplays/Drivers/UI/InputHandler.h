#pragma once

#include "Drivers/Input/DeadKey.h"
#include "Drivers/Input/Handlers.h"
#include "Drivers/Input/KeyState.h"

#include "ImGui/MouseEventQueue.h"

namespace IED
{
	namespace Drivers
	{
		class UIInputHandler :
			KeyState
		{
			static_assert(sizeof(WCHAR) == sizeof(ImWchar16));

			class KeyEventTaskPress
			{
			public:

				KeyEventTaskPress() = delete;

				inline explicit KeyEventTaskPress(
					UINT a_vkey,
					WCHAR a_char) :
					m_vkey(a_vkey),
					m_char(a_char)
				{
				}

				void Run();

			private:
				UINT m_vkey;
				WCHAR m_char;
			};

			class KeyEventTaskRelease
			{
			public:

				KeyEventTaskRelease() = delete;

				inline explicit KeyEventTaskRelease(
					UINT a_vkey) :
					m_vkey(a_vkey)
				{
				}

				void Run();

			private:
				UINT m_vkey;
			};

		protected:
			void ProcessEvent(const Handlers::KeyEvent& a_evn);

			[[nodiscard]] inline constexpr auto& GetKeyPressQueue() noexcept
			{
				return m_keyPressQueue;
			}

			[[nodiscard]] inline constexpr auto& GetKeyReleaseQueue() noexcept
			{
				return m_keyReleaseQueue;
			}

			[[nodiscard]] inline constexpr auto& GetMousePressEventQueue() noexcept
			{
				return m_mousePressQueue;
			}

			[[nodiscard]] inline constexpr auto& GetMouseReleaseEventQueue() noexcept
			{
				return m_mouseReleaseQueue;
			}

			void ProcessPressQueues();
			void ProcessReleaseQueues();

			void ResetInput();

		private:
			TaskQueueStatic<KeyEventTaskPress> m_keyPressQueue;
			TaskQueueStatic<KeyEventTaskRelease> m_keyReleaseQueue;
			ImGuiMouseEventQueue m_mousePressQueue;
			ImGuiMouseEventQueue m_mouseReleaseQueue;

			DeadKey m_dk;
		};
	}
}