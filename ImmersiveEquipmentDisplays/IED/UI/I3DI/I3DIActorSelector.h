#pragma once

#include "I3DIObjectSelector.h"

#include "IED/D3D/D3DPrimitiveBatch.h"

namespace IED
{
	class Controller;
	class D3DCommon;

	namespace UI
	{
		class I3DIActorSelector :
			public I3DIObjectSelector<
				Game::FormID,
				std::pair<
					std::pair<
						DirectX::XMVECTOR,
						DirectX::XMVECTOR>,
					DirectX ::XMMATRIX>>
		{
		public:
			I3DIActorSelector(
				ID3D11Device*        a_device,
				ID3D11DeviceContext* a_context) noexcept(false);

			void Update(Controller& a_controller);

			void XM_CALLCONV DrawBox(
				const Entry&      a_entry,
				DirectX::XMVECTOR a_color);

			DirectX::XMVECTOR XM_CALLCONV GetPopupAnchorPoint(
				D3DCommon&         a_scene,
				const Entry&       a_entry,
				DirectX::XMVECTOR& a_origin);

			void Render(D3DCommon& a_scene);

			D3DPrimitiveBatch m_batch;
		};
	}
}