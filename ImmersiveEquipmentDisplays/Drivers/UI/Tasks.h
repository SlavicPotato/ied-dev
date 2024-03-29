#pragma once

namespace IED
{
	namespace Drivers
	{
		class UI;
	}
}

#include "Drivers/Input/Handlers.h"

namespace IED
{
	namespace Tasks
	{
		class UIRenderTaskBase :
			public stl::intrusive_ref_counted
		{
			friend class IED::Drivers::UI;

		public:
			SKMP_REDEFINE_NEW_PREF();

			virtual ~UIRenderTaskBase() noexcept = default;

			constexpr void SetControlLock(bool a_switch) noexcept
			{
				m_options.lockControls = a_switch;
			}

			constexpr void SetFreezeTime(bool a_switch) noexcept
			{
				m_options.freeze = a_switch;
			}

			constexpr void SetWantCursor(bool a_switch) noexcept
			{
				m_options.wantCursor = a_switch;
			}

			constexpr void EnableRestrictions(bool a_switch) noexcept
			{
				m_options.enableChecks = a_switch;
			}

			constexpr bool GetFreeze() const noexcept
			{
				return m_options.freeze;
			}

			constexpr bool GetWantCursor() const noexcept
			{
				return m_options.wantCursor;
			}

			constexpr void SetEnabledInMenu(bool a_switch) noexcept
			{
				m_options.enableInMenu = a_switch;
			}

			constexpr bool GetEnabledInMenu() const noexcept
			{
				return m_options.enableInMenu;
			}

			constexpr void SetBlockCursor(bool a_switch) noexcept
			{
				m_options.blockCursor = a_switch;
			}

			constexpr bool GetBlockCursor() const noexcept
			{
				return m_options.blockCursor;
			}

			constexpr void SetBlockImGuiInput(bool a_switch) noexcept
			{
				m_options.blockImGuiInput = a_switch;
			}

			constexpr bool GetBlockInput() const noexcept
			{
				return m_options.blockImGuiInput;
			}

			long long GetRunTime() const noexcept;

			constexpr bool IsRunning() const noexcept
			{
				return m_state.running;
			}

			virtual bool RunEnableChecks() const;

			template <class T>
			[[nodiscard]] constexpr const T& As() const
			{
				return static_cast<const T&>(*this);
			}

			template <class T>
			[[nodiscard]] constexpr T& As()
			{
				return static_cast<T&>(*this);
			}

			template <class T>
			[[nodiscard]] constexpr const T* To() const
			{
				return dynamic_cast<const T*>(this);
			}

			template <class T>
			[[nodiscard]] constexpr T* To()
			{
				return dynamic_cast<T*>(this);
			}

			inline void StopMe() noexcept
			{
				m_stopMe.store(true, std::memory_order_relaxed);
			}

		protected:
			virtual bool Run() = 0;
			virtual void PrepareGameData(){};
			virtual void Render(){};
			virtual void OnMouseMove(const Handlers::MouseMoveEvent& a_evn){};
			virtual void OnKeyEvent(const Handlers::KeyEvent& a_evn){};

			struct TaskOptions
			{
				bool lockControls{ true };
				bool freeze{ false };
				bool enableChecks{ false };
				bool enableInMenu{ false };
				bool wantCursor{ true };
				bool blockCursor{ false };
				bool blockImGuiInput{ false };
			};

			struct TaskState
			{
				long long startTime{ 0 };

				bool running{ false };
				bool holdsControlLock{ false };
				bool holdsFreeze{ false };
				bool holdsWantCursor{ false };
				bool holdsBlockCursor{ false };
				bool holdsBlockImGuiInput{ false };
			};

			TaskOptions m_options;
			TaskState   m_state;

			std::atomic_bool m_stopMe{ false };

		private:
			virtual void OnTaskStart(){};
			virtual void OnTaskStop(){};
		};

	}
}