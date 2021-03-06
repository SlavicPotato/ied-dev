#include "pch.h"

#include "Render.h"

#include <skse64/NiRenderer.h>

namespace IED
{
	namespace Drivers
	{
		using namespace ::Events;

		Render Render::m_Instance;

		bool Render::Initialize(bool a_prepHook)
		{
			if (!hook::check_dst5<0xE8>(m_Instance.m_createD3D11_a.get()) ||
			    !hook::check_dst5<0xE8>(m_Instance.m_unkPresent_a.get()))
			{
				m_Instance.Error("Unable to hook, one or more invalid targets");
				return false;
			}

			auto& skse = ISKSE::GetSingleton();

			ASSERT(hook::call5(
				skse.GetBranchTrampoline(),
				m_Instance.m_createD3D11_a.get(),
				std::uintptr_t(CreateD3D11_Hook),
				m_Instance.m_createD3D11_o));

			ASSERT(hook::call5(
				skse.GetBranchTrampoline(),
				m_Instance.m_unkPresent_a.get(),
				std::uintptr_t(Present_Pre_Hook),
				m_Instance.m_unkPresent_o));

#if defined(IED_ENABLE_I3DI)
			if (a_prepHook)
			{
				ASSERT(hook::call5(
					skse.GetBranchTrampoline(),
					m_Instance.m_prepData_a.get(),
					std::uintptr_t(PrepareData_Hook),
					m_Instance.m_prepData_o));
			}
#endif

			return true;
		}

		void Render::CreateD3D11_Hook()
		{
			m_Instance.m_createD3D11_o();
			m_Instance.InitializeD3D();
		}

		void Render::InitializeD3D()
		{
			auto renderManager = BSRenderManager::GetSingleton();

			ASSERT(renderManager != nullptr);

			auto swapChain = renderManager->swapChain;
			auto device    = renderManager->forwarder;
			auto context   = renderManager->context;

			ASSERT(swapChain != nullptr);
			ASSERT(device != nullptr);
			ASSERT(context != nullptr);

			DXGI_SWAP_CHAIN_DESC sd{};
			if (FAILED(swapChain->GetDesc(std::addressof(sd))))
			{
				constexpr auto error_msg = "IDXGISwapChain::GetDesc failed";

				Error(error_msg);
				WinApi::MessageBoxError(PLUGIN_NAME, error_msg);

				return;
			}

			m_swapChainDesc = sd;

			m_device  = device;
			m_context = context;

			m_bufferSize.x = static_cast<float>(sd.BufferDesc.Width);
			m_bufferSize.y = static_cast<float>(sd.BufferDesc.Height);
			m_bufferSize.z = m_bufferSize.x / m_bufferSize.y;

			Debug("%s: Sending event", __FUNCTION__);

			Events::D3D11CreateEventPost evd_post{
				std::addressof(sd),
				renderManager->forwarder,
				renderManager->context,
				swapChain
			};

			GetEventDispatcher<Events::D3D11CreateEventPost>().SendEvent(evd_post);
		}

		void Render::Present_Pre_Hook(std::uint32_t a_p1)
		{
			m_Instance.m_unkPresent_o(a_p1);

			Events::IDXGISwapChainPresent evn;

			m_Instance.GetEventDispatcher<Events::IDXGISwapChainPresent>().SendEvent(evn);
		}

		void Render::PrepareData_Hook(Game::ProcessLists* a_pl, float a_frameTimerSlow)
		{
			m_Instance.m_prepData_o(a_pl, a_frameTimerSlow);

			Events::PrepareGameDataEvent evn;

			m_Instance.GetEventDispatcher<Events::PrepareGameDataEvent>().SendEvent(evn);
		}
	}
}