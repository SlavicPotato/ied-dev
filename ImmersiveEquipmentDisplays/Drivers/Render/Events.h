#pragma once

namespace IED
{
	namespace Drivers
	{
		namespace Events
		{
			struct D3D11CreateEventPost
			{
				CONST DXGI_SWAP_CHAIN_DESC* const m_pSwapChainDesc;
				ID3D11Device* const m_pDevice;
				ID3D11DeviceContext* const m_pImmediateContext;
				IDXGISwapChain* const m_pSwapChain;
			};

			struct IDXGISwapChainPresent
			{
			};
		}
	}
}