#include "pch.h"

#include "UIKeyedInputLockReleaseHandler.h"

#include "IED/Controller/Controller.h"

#include "Drivers/UI.h"
#include "Drivers/UI/Tasks.h"

namespace IED
{
	namespace UI
	{
		UIKeyedInputLockReleaseHandler::UIKeyedInputLockReleaseHandler(
			Tasks::UIRenderTaskBase& a_owner) :
			m_owner(a_owner)
		{
		}

		inline static bool should_force_key_up(std::uint32_t a_key) noexcept
		{
			if (!a_key)
			{
				return true;
			}

			if (a_key < InputMap::kMacro_MouseButtonOffset)
			{
				return ImGui::GetIO().WantCaptureKeyboard;
			}
			else if (a_key < InputMap::kMacro_MouseButtonOffset + InputMap::kMacro_NumMouseButtons)
			{
				return ImGui::GetIO().WantCaptureMouse;
			}
			else
			{
				return true;
			}
		}

		inline static void update_key_state(
			const Handlers::KeyEvent& a_evn,
			std::uint32_t             a_key,
			KeyEventState&            a_out) noexcept
		{
			if (!should_force_key_up(a_key))
			{
				if (a_key == a_evn.key)
				{
					a_out = a_evn.state;
				}
			}
			else
			{
				a_out = KeyEventState::KeyUp;
			}
		}

		void UIKeyedInputLockReleaseHandler::ILRHReceive(
			const Handlers::KeyEvent& a_evn)
		{
			if (!ILRHEnabled())
			{
				return;
			}

			update_key_state(a_evn, m_comboKey, m_comboState);
			update_key_state(a_evn, m_key, m_state);

			const auto held =
				(!m_comboKey || m_comboState == KeyEventState::KeyDown) &&
				m_state == KeyEventState::KeyDown;

			if (held != m_held)
			{
				m_held = held;
				EvaluateCurrentState();
			}
		}

		void UIKeyedInputLockReleaseHandler::ILRHSetKeys(
			std::uint32_t a_key,
			std::uint32_t a_combo)
		{
			m_key      = a_key;
			m_comboKey = a_combo;

			m_comboState = KeyEventState::KeyUp;
			m_state      = KeyEventState::KeyUp;

			if (m_held)
			{
				m_held = false;
				EvaluateCurrentState();
			}
		}

		void UIKeyedInputLockReleaseHandler::ILRHReset()
		{
			m_comboState = KeyEventState::KeyUp;
			m_state      = KeyEventState::KeyUp;
			m_held       = false;
			m_currentAlpha.reset();

			ILRHResetTaskOptions();
		}

		void UIKeyedInputLockReleaseHandler::ILRHResetTaskOptions()
		{
			m_owner.SetBlockCursor(false);
			m_owner.SetBlockImGuiInput(false);
			m_owner.SetControlLock(ILRHGetCurrentControlLockSetting());
			m_owner.SetFreezeTime(ILRHGetCurrentFreezeTimeSetting());
		}

		void UIKeyedInputLockReleaseHandler::ILRHBegin()
		{
			auto& style = ImGui::GetStyle();

			const auto held = ILRHIsHeld();

			if (held)
			{
				if (!m_currentAlpha)
				{
					m_currentAlpha = style.Alpha;
				}
			}

			if (!m_currentAlpha)
			{
				return;
			}

			const auto targetAlpha =
				held ?
					std::min(m_lockedAlpha, style.Alpha) :
					style.Alpha;

			m_originalAlpha = style.Alpha;

			const float absDeltaA = std::fabs(targetAlpha - *m_currentAlpha);

			if (absDeltaA > 0.001f)
			{
				style.Alpha = *m_currentAlpha =
					std::clamp(
						std::lerp(
							*m_currentAlpha,
							targetAlpha,
							ImGui::GetIO().DeltaTime * ALPHA_FADE_RATE),
						std::min(*m_currentAlpha, targetAlpha),
						std::max(*m_currentAlpha, targetAlpha));
			}
			else if (held)
			{
				style.Alpha = targetAlpha;
			}
			else
			{
				m_currentAlpha.reset();
			}
		}

		void UIKeyedInputLockReleaseHandler::ILRHEnd()
		{
			if (m_originalAlpha)
			{
				ImGui::GetStyle().Alpha = *m_originalAlpha;

				m_originalAlpha.reset();
			}
		}

		void UIKeyedInputLockReleaseHandler::ILRHSetUnfreezeTime(bool a_switch)
		{
			if (a_switch != m_unfreezeTime)
			{
				m_unfreezeTime = a_switch;

				if (m_held)
				{
					EvaluateCurrentState();
				}
			}
		}

		void UIKeyedInputLockReleaseHandler::SetHeldTaskOptions()
		{
			m_owner.SetBlockCursor(true);
			m_owner.SetBlockImGuiInput(true);
			m_owner.SetControlLock(false);
			if (m_unfreezeTime)
			{
				m_owner.SetFreezeTime(false);
			}
			else
			{
				m_owner.SetFreezeTime(ILRHGetCurrentFreezeTimeSetting());
			}
		}

		void UIKeyedInputLockReleaseHandler::EvaluateCurrentState()
		{
			if (m_held)
			{
				SetHeldTaskOptions();
			}
			else
			{
				ILRHResetTaskOptions();
			}

			Drivers::UI::EvaluateTaskState();
		}
	}
}