#pragma once

namespace IED
{
	namespace Drivers
	{
		class UI;
	}
}

namespace IED
{
	namespace Tasks
	{
		class UIRenderTaskBase
		{
			friend class IED::Drivers::UI;

		public:
			inline constexpr void UISetLock(bool a_switch) noexcept
			{
				m_options.lock = a_switch;
			}

			inline constexpr void UISetFreeze(bool a_switch) noexcept
			{
				m_options.freeze = a_switch;
			}

			inline constexpr void UIEnableRestrictions(bool a_switch) noexcept
			{
				m_options.enableChecks = a_switch;
			}

			inline constexpr bool UIGetFreeze() const
			{
				return m_options.freeze;
			}

			inline constexpr void UISetEnabledInMenu(bool a_switch) noexcept
			{
				m_options.enableInMenu = a_switch;
			}

			inline constexpr bool UIGetEnabledInMenu() const noexcept
			{
				return m_options.enableInMenu;
			}

		protected:
			virtual bool UIRunTask() = 0;
			virtual bool UIRunEnableChecks() const;

		private:
			struct renderTaskOptions_t
			{
				bool lock{ true };
				bool freeze{ false };
				bool enableChecks{ false };
				bool enableInMenu{ false };
			};

			struct renderTaskState_t
			{
				bool holdsLock{ false };
				bool holdsFreeze{ false };
			};

			renderTaskOptions_t m_options;
			renderTaskState_t   m_state;
		};
	}
}