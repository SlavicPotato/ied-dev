#pragma once

namespace IED
{
	namespace UI
	{
		class UIWindowBase
		{
		public:
			UIWindowBase() = default;

			UIWindowBase(const UIWindowBase&) = delete;
			UIWindowBase(UIWindowBase&&)      = delete;
			UIWindowBase& operator=(const UIWindowBase&) = delete;
			UIWindowBase& operator=(UIWindowBase&&) = delete;

			inline constexpr void SetOpenState(bool a_state) noexcept
			{
				m_openState = a_state;
			}

			inline constexpr void ToggleOpenState() noexcept
			{
				m_openState = !m_openState;
			}

			[[nodiscard]] inline constexpr bool* GetOpenState() noexcept
			{
				return std::addressof(m_openState);
			}

			[[nodiscard]] inline constexpr bool IsWindowOpen() const noexcept
			{
				return m_openState;
			}

		private:
			bool m_openState{ false };
		};

	}
}