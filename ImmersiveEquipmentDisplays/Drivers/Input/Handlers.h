#pragma once

#include "Events/Dispatcher.h"
#include "KeyEventType.h"

#include <ext/GameCommon.h>

namespace IED
{
	namespace Handlers
	{
		struct KeyEvent
		{
			KeyEventState state;
			std::uint32_t key;
		};

		class KeyPressHandler : public Events::EventSink<KeyEvent>
		{
		protected:
			using func_t = std::function<void()>;

		public:
			KeyPressHandler() = default;
			KeyPressHandler(func_t a_func) :
				m_func(std::move(a_func))
			{
			}

			inline constexpr void SetKey(std::uint32_t a_key) noexcept
			{
				m_key = a_key;
			}

			inline constexpr auto GetKey() const noexcept
			{
				return m_key;
			}

			inline constexpr void SetProcessPaused(bool a_switch) noexcept
			{
				m_processPaused = a_switch;
			}

			void SetLambda(func_t a_func)
			{
				m_func = std::move(a_func);
			}

			inline constexpr bool Enabled() const noexcept
			{
				return m_key != 0;
			}

		protected:
			inline constexpr bool CanProcess() const noexcept
			{
				if (m_processPaused)
				{
					return true;
				}

				if (Game::InPausedMenu())
				{
					return false;
				}

				return !Game::Main::GetSingleton()->freezeTime;
			}

			inline constexpr void Trigger() const
			{
				if (CanProcess())
				{
					m_func();
				}
			}

			std::uint32_t m_key{ 0 };
			bool          m_processPaused{ true };

			func_t m_func;
		};

		class ComboKeyPressHandler : public KeyPressHandler
		{
		public:
			using KeyPressHandler::KeyPressHandler;

			inline constexpr void SetComboKey(std::uint32_t a_key) noexcept
			{
				m_comboKey     = a_key;
				m_comboKeyDown = false;
			}

			inline constexpr void SetKeys(
				std::uint32_t a_comboKey,
				std::uint32_t a_key) noexcept
			{
				SetComboKey(a_comboKey);
				SetKey(a_key);
			}

			inline constexpr auto GetComboKey() const noexcept
			{
				return m_comboKey;
			}

		private:
			bool          m_comboKeyDown{ false };
			std::uint32_t m_comboKey{ 0 };

			virtual void Receive(const KeyEvent& a_evn) override;
		};
	}

}