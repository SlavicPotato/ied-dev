#pragma once

namespace IED
{
	namespace Drivers
	{
		namespace Events
		{
			struct D3D11CreateEventPost
			{
				const DXGI_SWAP_CHAIN_DESC& m_pSwapChainDesc;
				ID3D11Device*               m_pDevice;
				ID3D11DeviceContext*        m_pImmediateContext;
				IDXGISwapChain4*            m_pSwapChain;
			};

			struct IDXGISwapChainPresent
			{
			};

			struct PrepareGameDataEvent
			{
			};
		}
	}
}