#pragma once

namespace IED
{
	namespace UI
	{

		struct I3DIRay
		{
			DirectX::XMVECTOR origin{ DirectX::g_XMZero.v };
			DirectX::XMVECTOR dir{ DirectX::g_XMZero.v };
		};

	}
}