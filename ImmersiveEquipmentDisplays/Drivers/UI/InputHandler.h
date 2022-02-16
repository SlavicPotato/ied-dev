#pragma once

#include "Drivers/Input/DeadKey.h"
#include "Drivers/Input/Handlers.h"
#include "Drivers/Input/KeyState.h"

namespace IED
{
	namespace Drivers
	{
		class UIInputHandler :
			KeyState
		{
			static_assert(sizeof(WCHAR) == sizeof(ImWchar16));

			SKMP_ALIGN(16)
			class KeyEventTaskPress
			{
			public:
				KeyEventTaskPress() = delete;

				inline explicit KeyEventTaskPress(
					UINT  a_key,
					WCHAR a_char) :
					m_keys{ a_key, a_char },
					m_type(KeyEventType::Keyboard)
				{
				}

				inline explicit KeyEventTaskPress(
					KeyEventType a_type,
					UINT         a_key) :
					m_keys{ a_key, 0 },
					m_type(a_type)
				{
				}

				inline explicit KeyEventTaskPress(
					float a_delta) :
					m_wheel{ a_delta },
					m_type(KeyEventType::Wheel)
				{
				}

				void Run();

			private:
				KeyEventType m_type;

				union
				{
					struct
					{
						UINT  key;
						WCHAR chr;
					} m_keys;

					struct
					{
						float delta;
					} m_wheel;
				};
			};

			class KeyEventTaskRelease
			{
			public:
				KeyEventTaskRelease() = delete;

				inline explicit KeyEventTaskRelease(
					KeyEventType a_type,
					UINT         a_vkey) :
					m_type(a_type),
					m_key(a_vkey)
				{
				}

				void Run();

			private:
				UINT         m_key;
				KeyEventType m_type;
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

			void ProcessPressQueue();
			void ProcessReleaseQueue();

			void ResetInput();

		private:
			TaskQueueStatic<KeyEventTaskPress>   m_keyPressQueue;
			TaskQueueStatic<KeyEventTaskRelease> m_keyReleaseQueue;

			DeadKey m_dk;
		};
	}
}