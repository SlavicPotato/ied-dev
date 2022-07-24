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
		class UIRenderTaskBase
		{
			friend class IED::Drivers::UI;

		public:
			virtual ~UIRenderTaskBase() noexcept = default;

			inline constexpr void SetControlLock(bool a_switch) noexcept
			{
				m_options.lock = a_switch;
			}

			inline constexpr void SetFreezeTime(bool a_switch) noexcept
			{
				m_options.freeze = a_switch;
			}

			inline constexpr void SetWantCursor(bool a_switch) noexcept
			{
				m_options.wantCursor = a_switch;
			}

			inline constexpr void EnableRestrictions(bool a_switch) noexcept
			{
				m_options.enableChecks = a_switch;
			}

			inline constexpr bool GetFreeze() const noexcept
			{
				return m_options.freeze;
			}

			inline constexpr bool GetWantCursor() const noexcept
			{
				return m_options.wantCursor;
			}

			inline constexpr void SetEnabledInMenu(bool a_switch) noexcept
			{
				m_options.enableInMenu = a_switch;
			}

			inline constexpr bool GetEnabledInMenu() const noexcept
			{
				return m_options.enableInMenu;
			}

			long long GetRunTime() const noexcept;

			inline constexpr bool IsRunning() const noexcept
			{
				return m_state.running;
			}

			virtual bool RunEnableChecks() const;

		protected:
			virtual bool Run() = 0;
			virtual void PrepareGameData(){};
			virtual void Render(){};
			virtual void OnMouseMove(const Handlers::MouseMoveEvent &a_evn){};

			struct renderTaskOptions_t
			{
				bool lock{ true };
				bool freeze{ false };
				bool enableChecks{ false };
				bool enableInMenu{ false };
				bool wantCursor{ true };
			};

			struct renderTaskState_t
			{
				long long startTime{ 0 };

				bool running{ false };
				bool holdsLock{ false };
				bool holdsFreeze{ false };
				bool holdsWantCursor{ false };
			};

			renderTaskOptions_t m_options;
			renderTaskState_t   m_state;

			bool m_stopMe{ false };

		private:
			virtual void OnTaskStart(){};
			virtual void OnTaskStop(){};
		};

	}
}