#pragma once

#include "Drivers/Input/Handlers.h"

namespace IED
{
	namespace Tasks
	{
		class UIRenderTaskBase;
	};

	class Controller;

	namespace UI
	{
		class UIKeyedInputLockReleaseHandler
		{
			static constexpr float ALPHA_FADE_RATE = 13.0f;

		public:
			UIKeyedInputLockReleaseHandler(
				Tasks::UIRenderTaskBase& a_owner);

			void ILRHReceive(const Handlers::KeyEvent& a_evn);

			void ILRHSetKeys(std::uint32_t a_key, std::uint32_t a_combo = 0);

			void ILRHReset();
			void ILRHResetTaskOptions();

			void ILRHBegin();
			void ILRHEnd();

			[[nodiscard]] constexpr bool ILRHEnabled() const noexcept
			{
				return m_key != 0;
			}

			[[nodiscard]] constexpr auto ILRHGetKey() const noexcept
			{
				return m_key;
			}

			[[nodiscard]] constexpr auto ILRHGetComboKey() const noexcept
			{
				return m_comboKey;
			}

			[[nodiscard]] constexpr auto ILRHIsHeld() const noexcept
			{
				return m_held;
			}

			constexpr void ILRHSetLockedAlpha(float a_alpha)
			{
				m_lockedAlpha = std::clamp(a_alpha, 0.0f, 1.0f);
			}

			[[nodiscard]] constexpr auto ILRHGetLockedAlpha() const noexcept
			{
				return m_lockedAlpha;
			}

			void ILRHSetUnfreezeTime(bool a_switch);

		private:
			virtual bool ILRHGetCurrentControlLockSetting() = 0;
			virtual bool ILRHGetCurrentFreezeTimeSetting()  = 0;

			void SetHeldTaskOptions();
			void EvaluateCurrentState();

			std::uint32_t m_key{ 0 };
			std::uint32_t m_comboKey{ 0 };

			float m_lockedAlpha{ 0.33f };

			bool m_held{ false };
			bool m_unfreezeTime{ false };

			KeyEventState m_comboState{ KeyEventState::KeyUp };
			KeyEventState m_state{ KeyEventState::KeyUp };

			std::optional<float> m_currentAlpha;
			std::optional<float> m_originalAlpha;

			Tasks::UIRenderTaskBase& m_owner;
		};
	}
}