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

		bool Input::PlayerControls_InputEvent_ProcessEvents_Hook(
			const InputEvent** a_evns)
		{
			return m_Instance.ProcessEventsHookImpl(a_evns);
		}

		bool Input::ProcessEventsHookImpl(const InputEvent** a_evns)
		{
			bool blocked = m_inputBlocked.load(
				std::memory_order_relaxed);

			if (blocked)
			{
				// nasty, implement this properly
				if (auto pc = PlayerControls::GetSingleton())
				{
					pc->moveInput = { 0, 0 };
					pc->lookInput = { 0, 0 };
					pc->prevMove  = { 0, 0 };
					pc->prevLook  = { 0, 0 };
				}
			}

			if (a_evns)
			{
				ProcessPriorityEvents(a_evns);
			}

			if (m_nextIEPCall)
			{
				blocked |= m_nextIEPCall(a_evns);
			}

			return blocked;
		}

		void Input::ProcessPriorityEvents(const InputEvent** a_evns)
		{
			for (auto it = *a_evns; it; it = it->next)
			{
				switch (it->eventType)
				{
				case INPUT_EVENT_TYPE::kButton:
					{
						auto buttonEvent = static_cast<const ButtonEvent*>(it);

						auto          deviceType = buttonEvent->device;
						std::uint32_t keyCode;

						if (deviceType == INPUT_DEVICE::kMouse)
						{
							keyCode = InputMap::kMacro_MouseButtonOffset + buttonEvent->GetIDCode();
						}
						else if (deviceType == INPUT_DEVICE::kKeyboard)
						{
							keyCode = buttonEvent->GetIDCode();
						}
						else
						{
							continue;
						}

						if (!keyCode || keyCode >= InputMap::kMaxMacros)
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
						else if (buttonEvent->IsUpLF())
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
					auto buttonEvent = it->AsButtonEvent();
					if (!buttonEvent)
					{
						continue;
					}

					if (buttonEvent->device != INPUT_DEVICE::kKeyboard)
					{
						continue;
					}

					std::uint32_t keyCode = buttonEvent->GetIDCode();

					if (!keyCode || keyCode >= InputMap::kMaxMacros)
					{
						continue;
					}

					if (buttonEvent->IsDown())
					{
						DispatchKeyEvent(KeyEventState::KeyDown, keyCode);
					}
					else if (buttonEvent->IsUpLF())
					{
						DispatchKeyEvent(KeyEventState::KeyUp, keyCode);
					}
				}
			}

			return EventResult::kContinue;
		}

		template <bool _IsAE, class T>
		static constexpr bool ExtractHookCallAddr(
			std::uintptr_t a_dst,
			T&             a_out)
		{
#pragma pack(push, 1)

			struct payload_se_t
			{
				/*	
					mov rcx, r12
				*/
				std::uint8_t m[3];
				// call ..
				std::uint8_t escape;
				std::uint8_t modrm;
				std::int32_t displ;
				// ...
			};

			struct payload_ae_t
			{
				/*	
					push rax
					push rcx
					push r11
					sub rsp, 0x28
					mov rcx, r12
				*/
				std::uint8_t m[11];
				// call ..
				std::uint8_t escape;
				std::uint8_t modrm;
				std::int32_t displ;
				// ...
			};

#pragma pack(pop)

			using payload_t = std::conditional_t<
				_IsAE,
				payload_ae_t,
				payload_se_t>;

			std::uintptr_t traddr;

			if (!hook::get_dst5<0xE9>(a_dst, traddr))
			{
				return false;
			}

			const payload_t* pl{ nullptr };

			if (!hook::get_dst6<0x25>(traddr, pl))
			{
				return false;
			}

			if constexpr (_IsAE)
			{
				if (!Patching::validate_mem(
						std::uintptr_t(pl->m),
						{ 0x50,
				          0x51,
				          0x41,
				          0x53,
				          0x48,
				          0x83,
				          0xEC,
				          0x28,
				          0x4C,
				          0x89,
				          0xE1 }))
				{
					return false;
				}
			}
			else
			{
				if (!Patching::validate_mem(
						std::uintptr_t(pl->m),
						{ 0x4C,
				          0x89,
				          0xE1 }))
				{
					return false;
				}
			}

			return hook::get_dst6<0x15>(
				std::uintptr_t(std::addressof(pl->escape)),
				a_out);
		}

		void Input::InstallPriorityHook()
		{
			struct ProcessInputEvent : JITASM::JITASM
			{
				ProcessInputEvent(std::uintptr_t a_targetAddr) :
					JITASM(ISKSE::GetLocalTrampoline())
				{
					Xbyak::Label exitSkipLabel;
					Xbyak::Label exitContinueLabel;
					Xbyak::Label callLabel;

					Xbyak::Label skip;

					if (IAL::IsAE())
					{
						push(rax);
						push(rcx);
						push(r11);
						sub(rsp, 0x28);

						mov(rcx, r12);
						call(ptr[rip + callLabel]);
						mov(dl, al);

						add(rsp, 0x28);
						pop(r11);
						pop(rcx);
						pop(rax);

						test(dl, dl);
						jne(skip);

						cmp(rsi, rbx);
					}
					else
					{
						mov(rcx, r12);
						call(ptr[rip + callLabel]);
						test(al, al);
						jne(skip);

						cmp(rdi, rbx);
					}

					je(skip);

					jmp(ptr[rip + exitContinueLabel]);

					L(skip);
					jmp(ptr[rip + exitSkipLabel]);

					L(exitSkipLabel);
					dq(a_targetAddr + (IAL::IsAE() ? 0x66 : 0x6A));

					L(exitContinueLabel);
					dq(a_targetAddr + 0x5);

					L(callLabel);
					dq(std::uintptr_t(PlayerControls_InputEvent_ProcessEvents_Hook));
				}
			};

			if ((IAL::IsAE() ?
			         ExtractHookCallAddr<true>(
						 m_inputEventpProc_a.get(),
						 m_Instance.m_nextIEPCall) :
                     ExtractHookCallAddr<false>(
						 m_inputEventpProc_a.get(),
						 m_Instance.m_nextIEPCall)))
			{
				m_Instance.Debug(
					"%s: extracted previous hook call address @%p",
					__FUNCTION__,
					m_Instance.m_nextIEPCall);
			}
			else
			{
				if (IAL::IsAE())
				{
					ASSERT_STR(
						Patching::validate_mem(
							m_inputEventpProc_a.get(),
							{ 0x48, 0x3B, 0xF3, 0x74, 0x61 }),
						"Memory validation failed");
				}
				else
				{
					ASSERT_STR(
						Patching::validate_mem(
							m_inputEventpProc_a.get(),
							{ 0x48, 0x3B, 0xFB, 0x74, 0x65 }),
						"Memory validation failed");
				}
			}

			m_Instance.LogPatchBegin();
			{
				ProcessInputEvent code(m_inputEventpProc_a.get());

				ISKSE::GetBranchTrampoline().Write5Branch(m_inputEventpProc_a.get(), code.get());
			}
			m_Instance.LogPatchEnd();
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

		void Input::DispatchPriorityKeyEvent(const MouseMoveEvent* a_evn)
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