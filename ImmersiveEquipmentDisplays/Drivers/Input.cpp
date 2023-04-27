#include "pch.h"

#include "Input.h"

#include <ext/IHook.h>
#include <ext/JITASM.h>

namespace IED
{
	namespace Drivers
	{
		using namespace ::Events;

		Input Input::m_Instance;

		void Input::BSTEventSource_InputEvent_SendEvent_Hook(
			BSTEventSource<InputEvent*>* a_dispatcher,
			InputEvent* const*           a_evns)
		{
			m_Instance.ProcessPriorityEventsImpl(a_dispatcher, a_evns);
		}

		void Input::ProcessPriorityEventsImpl(
			BSTEventSource<InputEvent*>* a_dispatcher,
			InputEvent* const*           a_evns)
		{
			if (a_evns)
			{
				ProcessPriorityEvents(a_evns);
			}

			if (m_inputBlocked.load(std::memory_order_relaxed))
			{
				constexpr InputEvent* const dummy[] = { nullptr };

				m_inputEventProc_o(a_dispatcher, dummy);
			}
			else
			{
				m_inputEventProc_o(a_dispatcher, a_evns);
			}
		}

		namespace detail
		{
			static constexpr std::uint32_t get_keycode(const ButtonEvent* a_event) noexcept
			{
				const auto    deviceType = a_event->device;
				std::uint32_t keyCode;

				switch (deviceType)
				{
				case INPUT_DEVICE::kMouse:
					keyCode = InputMap::kMacro_MouseButtonOffset + a_event->GetIDCode();
					break;
				case INPUT_DEVICE::kGamepad:
					keyCode = InputMap::kMacro_GamepadOffset + a_event->GetIDCode();
					break;
				case INPUT_DEVICE::kKeyboard:
					keyCode = a_event->GetIDCode();
					break;
				default:
					return 0;
				}

				return keyCode < InputMap::kMaxMacros ? keyCode : 0;
			}
		}

		void Input::ProcessPriorityEvents(const InputEvent* const* a_evns)
		{
			for (auto it = *a_evns; it; it = it->next)
			{
				switch (it->eventType)
				{
				case INPUT_EVENT_TYPE::kButton:
					{
						const auto buttonEvent = static_cast<const ButtonEvent*>(it);
						const auto keyCode     = detail::get_keycode(buttonEvent);

						if (!keyCode)
						{
							continue;
						}

						//_DMESSAGE("%X | %f | %f", keyCode, buttonEvent->value, buttonEvent->heldDownSecs);

						if (buttonEvent->IsDown())
						{
							DispatchPriorityKeyEvent(
								KeyEventState::KeyDown,
								keyCode);
						}
						else if (buttonEvent->IsUp())
						{
							DispatchPriorityKeyEvent(
								KeyEventState::KeyUp,
								keyCode);
						}
					}
					break;
				case INPUT_EVENT_TYPE::kMouseMove:
					{
						auto mouseMoveEvent = static_cast<const MouseMoveEvent*>(it);

						DispatchPriorityKeyEvent(mouseMoveEvent);
					}
					break;
				}
			}
		}

		auto Input::ReceiveEvent(
			InputEvent* const*           a_evns,
			BSTEventSource<InputEvent*>* a_dispatcher)
			-> EventResult
		{
			if (a_evns)
			{
				for (auto it = *a_evns; it; it = it->next)
				{
					const auto buttonEvent = it->AsButtonEvent();
					if (!buttonEvent)
					{
						continue;
					}

					const auto keyCode = detail::get_keycode(buttonEvent);
					if (!keyCode)
					{
						continue;
					}

					if (buttonEvent->IsDown())
					{
						DispatchKeyEvent(KeyEventState::KeyDown, keyCode);
					}
					else if (buttonEvent->IsUp())
					{
						DispatchKeyEvent(KeyEventState::KeyUp, keyCode);
					}
				}
			}

			return EventResult::kContinue;
		}

		void Input::InstallPriorityHook()
		{
			const bool result = hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_Instance.m_inputEventProc_a.get(),
				std::uintptr_t(BSTEventSource_InputEvent_SendEvent_Hook),
				m_Instance.m_inputEventProc_o);

			ASSERT(result == true);
		}

		bool Input::SinkToInputDispatcher()
		{
			if (auto evd = InputEventDispatcher::GetSingleton())
			{
				evd->AddEventSink(std::addressof(m_Instance));
				return true;
			}
			else
			{
				return false;
			}
		}

		void Input::RegisterForPriorityKeyEvents(
			EventSink<Handlers::KeyEvent>* const a_handler)
		{
			m_Instance.m_prioHandlers.AddSink(a_handler);
		}

		void Input::RegisterForPriorityKeyEvents(
			EventSink<Handlers::KeyEvent>& a_handler)
		{
			m_Instance.m_prioHandlers.AddSink(std::addressof(a_handler));
		}

		void Input::RegisterForPriorityMouseMoveEvents(
			EventSink<Handlers::MouseMoveEvent>* const a_handler)
		{
			m_Instance.m_prioMMHandlers.AddSink(a_handler);
		}

		void Input::RegisterForKeyEvents(
			EventSink<Handlers::KeyEvent>* const a_handler)
		{
			m_Instance.m_handlers.AddSink(a_handler);
		}

		void Input::RegisterForKeyEvents(
			EventSink<Handlers::KeyEvent>& a_handler)
		{
			m_Instance.m_handlers.AddSink(std::addressof(a_handler));
		}

		void Input::DispatchPriorityKeyEvent(
			KeyEventState a_event,
			std::uint32_t a_keyCode)
		{
			const Handlers::KeyEvent evn{
				a_event,
				a_keyCode
			};

			m_prioHandlers.SendEvent(evn);
		}

		void Input::DispatchPriorityKeyEvent(
			const MouseMoveEvent* a_evn)
		{
			const Handlers::MouseMoveEvent evn{
				a_evn->mouseInputX,
				a_evn->mouseInputY
			};

			m_prioMMHandlers.SendEvent(evn);
		}

		void Input::DispatchKeyEvent(
			KeyEventState a_event,
			std::uint32_t a_keyCode)
		{
			const Handlers::KeyEvent evn{
				a_event,
				a_keyCode
			};

			m_handlers.SendEvent(evn);
		}
	}
}