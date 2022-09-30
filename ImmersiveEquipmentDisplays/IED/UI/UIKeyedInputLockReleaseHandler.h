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
			inline static constexpr auto ALPHA_FADE_RATE = 13.0f;

		public:
			UIKeyedInputLockReleaseHandler(
				Tasks::UIRenderTaskBase& a_owner,
				Controller&              a_controller);

			void Receive(const Handlers::KeyEvent& a_evn);

			void SetKeys(std::uint32_t a_key, std::uint32_t a_combo = 0);

			void Reset();
			void ResetTaskOptions() const;

			void Begin();
			void End();

			[[nodiscard]] inline constexpr bool Enabled() const noexcept
			{
				return m_key != 0;
			}

			[[nodiscard]] inline constexpr auto GetKey() const noexcept
			{
				return m_key;
			}

			[[nodiscard]] inline constexpr auto GetComboKey() const noexcept
			{
				return m_comboKey;
			}

			[[nodiscard]] inline constexpr auto IsHeld() const noexcept
			{
				return m_held;
			}

			inline constexpr void SetLockedAlpha(float a_alpha)
			{
				m_lockedAlpha = std::clamp(a_alpha, 0.0f, 1.0f);
			}

			[[nodiscard]] inline constexpr auto GetLockedAlpha() const noexcept
			{
				return m_lockedAlpha;
			}

		private:
			void EvaluateCurrentState();

			std::uint32_t m_key{ 0 };
			std::uint32_t m_comboKey{ 0 };

			bool m_held{ false };

			KeyEventState m_comboState{ KeyEventState::KeyUp };
			KeyEventState m_state{ KeyEventState::KeyUp };

			float m_lockedAlpha{ 0.33f };

			std::optional<float> m_currentAlpha;
			std::optional<float> m_originalAlpha;

			Tasks::UIRenderTaskBase& m_owner;
			Controller&              m_controller;
		};
	}
}