#include "pch.h"

#include "Render.h"

namespace IED
{
	namespace Drivers
	{
		using namespace ::Events;

		Render Render::m_Instance;

		bool Render::Initialize(bool a_prepHook)
		{
			return m_Instance.InitializeImpl(a_prepHook);
		}

		bool Render::InitializeImpl(bool a_prepHook)
		{
			if (!hook::check_dst5<0xE8>(m_createD3D11_a.get()) ||
			    !hook::check_dst5<0xE8>(m_unkPresent_a.get()))
			{
				Error("Unable to hook, one or more invalid targets");
				return false;
			}

			auto& skse = ISKSE::GetSingleton();

			ASSERT(hook::call5(
				skse.GetBranchTrampoline(),
				m_createD3D11_a.get(),
				std::uintptr_t(CreateD3D11_Hook),
				m_createD3D11_o));

			ASSERT(hook::call5(
				skse.GetBranchTrampoline(),
				m_unkPresent_a.get(),
				std::uintptr_t(Present_Pre_Hook),
				m_unkPresent_o));

			return true;
		}

		void Render::CreateD3D11_Hook()
		{
			m_Instance.m_createD3D11_o();
			m_Instance.InitializeD3D();
		}

		void Render::InitializeD3D()
		{
			const auto renderManager = BSRenderManager::GetSingleton();

			ASSERT(renderManager != nullptr);

			const auto swapChain = renderManager->swapChain;
			const auto device    = renderManager->forwarder;
			const auto context   = renderManager->context;

			ASSERT(swapChain != nullptr);
			ASSERT(device != nullptr);
			ASSERT(context != nullptr);

			if (FAILED(swapChain->GetDesc(std::addressof(m_swapChainDesc))))
			{
				WinApi::MessageBoxErrorLog(PLUGIN_NAME_FULL, "IDXGISwapChain::GetDesc failed");
				return;
			}

			m_device  = device;
			m_context = context;

			m_bufferSize.x = static_cast<float>(m_swapChainDesc.BufferDesc.Width);
			m_bufferSize.y = static_cast<float>(m_swapChainDesc.BufferDesc.Height);
			m_bufferSize.z = m_bufferSize.x / m_bufferSize.y;

			Debug("%s: Sending event", __FUNCTION__);

			const Events::D3D11CreateEventPost evd_post{
				m_swapChainDesc,
				renderManager->forwarder,
				renderManager->context,
				swapChain
			};

			GetEventDispatcher<Events::D3D11CreateEventPost>().SendEvent(evd_post);
		}

		void Render::Present_Pre_Hook(std::uint32_t a_p1)
		{
			m_Instance.m_unkPresent_o(a_p1);

			const Events::IDXGISwapChainPresent evn{};

			m_Instance.GetEventDispatcher<Events::IDXGISwapChainPresent>().SendEvent(evn);
		}
	}
}