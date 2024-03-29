#pragma once

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
		
		struct MouseMoveEvent
		{
			std::int32_t inputX;
			std::int32_t inputY;
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

			constexpr void SetKey(std::uint32_t a_key) noexcept
			{
				m_key = a_key;
			}

			constexpr auto GetKey() const noexcept
			{
				return m_key;
			}

			constexpr void SetProcessPaused(bool a_switch) noexcept
			{
				m_processPaused = a_switch;
			}

			void SetLambda(func_t a_func)
			{
				m_func = std::move(a_func);
			}

			constexpr bool Enabled() const noexcept
			{
				return m_key != 0;
			}

		protected:
			bool CanProcess() const noexcept;
			void Trigger() const;

			std::uint32_t m_key{ 0 };
			bool          m_processPaused{ true };

			func_t m_func;
		};

		class ComboKeyPressHandler : public KeyPressHandler
		{
		public:
			using KeyPressHandler::KeyPressHandler;

			constexpr void SetComboKey(std::uint32_t a_key) noexcept
			{
				m_comboKey     = a_key;
				m_comboKeyDown = false;
			}

			constexpr void SetKeys(
				std::uint32_t a_comboKey,
				std::uint32_t a_key) noexcept
			{
				SetComboKey(a_comboKey);
				SetKey(a_key);
			}

			constexpr auto GetComboKey() const noexcept
			{
				return m_comboKey;
			}

		private:
			std::uint32_t m_comboKey{ 0 };
			bool          m_comboKeyDown{ false };

			virtual void Receive(const KeyEvent& a_evn) override;
		};
	}

}