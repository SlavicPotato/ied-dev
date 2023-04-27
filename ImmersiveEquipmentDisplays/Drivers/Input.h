#pragma once

#include "Input/Handlers.h"

namespace IED
{
	namespace Drivers
	{
		class Input :
			ILog,
			public BSTEventSink<InputEvent*>
		{
		public:
			static void RegisterForPriorityKeyEvents(
				::Events::EventSink<Handlers::KeyEvent>* const a_handler);

			static void RegisterForPriorityKeyEvents(
				::Events::EventSink<Handlers::KeyEvent>& a_handler);

			static void RegisterForPriorityMouseMoveEvents(
				::Events::EventSink<Handlers::MouseMoveEvent>* const a_handler);

			static void RegisterForKeyEvents(
				::Events::EventSink<Handlers::KeyEvent>* const a_handler);

			static void RegisterForKeyEvents(
				::Events::EventSink<Handlers::KeyEvent>& a_handler);

			static void InstallPriorityHook();
			static bool SinkToInputDispatcher();

			inline static void SetInputBlocked(bool a_enabled) noexcept
			{
				m_Instance.m_inputBlocked.store(a_enabled, std::memory_order_relaxed);
			}

			FN_NAMEPROC("Input");

		private:
			Input() = default;

			virtual EventResult ReceiveEvent(
				InputEvent* const*           a_evns,
				BSTEventSource<InputEvent*>* a_dispatcher) override;

			static void BSTEventSource_InputEvent_SendEvent_Hook(
				BSTEventSource<InputEvent*>* a_dispatcher,
				const InputEvent**           a_evns);

			void ProcessPriorityEventsImpl(
				BSTEventSource<InputEvent*>* a_dispatcher,
				const InputEvent**           a_evns);

			void ProcessPriorityEvents(const InputEvent** a_evns);

			void DispatchPriorityKeyEvent(
				KeyEventState a_event,
				std::uint32_t a_keyCode);

			void DispatchPriorityKeyEvent(
				const MouseMoveEvent* a_evn);

			void DispatchKeyEvent(
				KeyEventState a_event,
				std::uint32_t a_keyCode);

			::Events::EventDispatcher<Handlers::KeyEvent>       m_prioHandlers;
			::Events::EventDispatcher<Handlers::MouseMoveEvent> m_prioMMHandlers;
			::Events::EventDispatcher<Handlers::KeyEvent>       m_handlers;

			std::atomic_bool m_inputBlocked{ false };

			decltype(&BSTEventSource_InputEvent_SendEvent_Hook) m_inputEventProc_o{ nullptr };

			static inline const auto m_inputEventProc_a = IAL::Address<std::uintptr_t>(67315, 68617, 0x7B, 0x7B);

			static Input m_Instance;
		};
	}
}