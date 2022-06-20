#pragma once

#include "IED/D3D/D3DCommon.h"
#include "IED/D3D/D3DPrimitiveBatch.h"

#include "I3DIAssetCache.h"

namespace IED
{
	namespace UI
	{
		struct I3DICommonData
		{
			struct Ray
			{
				Ray() noexcept :
					origin(DirectX::g_XMZero),
					dir(DirectX::g_XMZero)
				{
				}

				DirectX::XMVECTOR origin;
				DirectX::XMVECTOR dir;
			};

			I3DICommonData(
				ID3D11Device*               a_device,
				ID3D11DeviceContext*        a_context,
				const DXGI_SWAP_CHAIN_DESC& a_desc) noexcept(false) :
				scene(a_device, a_context, a_desc),
				batchNoDepth(a_device, a_context),
				assets(a_device, a_context)
			{
			}

			D3DCommon         scene;
			D3DPrimitiveBatch batchNoDepth;
			I3DIAssetCache    assets;

			Ray ray;

			void UpdateRay();
		};

	}
}