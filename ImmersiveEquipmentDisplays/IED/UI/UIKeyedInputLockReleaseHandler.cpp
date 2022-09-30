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
			Tasks::UIRenderTaskBase& a_owner,
			Controller&              a_controller) :
			m_owner(a_owner),
			m_controller(a_controller)
		{
		}

		void UIKeyedInputLockReleaseHandler::Receive(
			const Handlers::KeyEvent& a_evn)
		{
			if (!Enabled())
			{
				return;
			}

			if (m_comboKey && m_comboKey == a_evn.key)
			{
				m_comboState = a_evn.state;
			}

			if (m_key && m_key == a_evn.key)
			{
				m_state = a_evn.state;
			}

			auto held = (!m_comboKey || m_comboState == KeyEventState::KeyDown) &&
			            m_state == KeyEventState::KeyDown;

			if (held != m_held)
			{
				m_held = held;
				EvaluateCurrentState();
			}
		}

		void UIKeyedInputLockReleaseHandler::SetKeys(
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

		void UIKeyedInputLockReleaseHandler::Reset()
		{
			m_comboState = KeyEventState::KeyUp;
			m_state      = KeyEventState::KeyUp;
			m_held       = false;
			m_currentAlpha.reset();

			ResetTaskOptions();
		}

		void UIKeyedInputLockReleaseHandler::ResetTaskOptions() const
		{
			m_owner.SetBlockCursor(false);
			m_owner.SetBlockImGuiInput(false);
			m_owner.SetControlLock(m_controller.GetConfigStore().settings.data.ui.enableControlLock);
		}

		void UIKeyedInputLockReleaseHandler::Begin()
		{
			auto& style = ImGui::GetStyle();

			const auto held = IsHeld();

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

			if (std::fabs(targetAlpha - *m_currentAlpha) > 0.001f)
			{
				style.Alpha = *m_currentAlpha = std::lerp(
					*m_currentAlpha,
					targetAlpha,
					ImGui::GetIO().DeltaTime * ALPHA_FADE_RATE);
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

		void UIKeyedInputLockReleaseHandler::End()
		{
			if (m_originalAlpha)
			{
				ImGui::GetStyle().Alpha = *m_originalAlpha;

				m_originalAlpha.reset();
			}
		}

		void UIKeyedInputLockReleaseHandler::EvaluateCurrentState()
		{
			if (m_held)
			{
				m_owner.SetBlockCursor(true);
				m_owner.SetBlockImGuiInput(true);
				m_owner.SetControlLock(false);
			}
			else
			{
				ResetTaskOptions();
			}

			Drivers::UI::EvaluateTaskState();
		}
	}
}