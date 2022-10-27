#pragma once

namespace IED
{
	namespace UI
	{

		struct I3DIRay
		{
			I3DIRay() noexcept :
				origin(DirectX::g_XMZero.v),
				dir(DirectX::g_XMZero.v)
			{
			}

			DirectX::XMVECTOR origin;
			DirectX::XMVECTOR dir;
		};

	}
}