#pragma once

#include <DirectXMath.h>

struct CD3D11_VIEWPORT;

class NiCamera;
class NiTransform;

namespace VectorMath
{
	DirectX::XMMATRIX XM_CALLCONV NiTransformTo4x4Matrix(
		const NiTransform& a_in);

	DirectX::XMMATRIX XM_CALLCONV NiTransformGetRotation(
		const NiTransform& a_in);

	DirectX::XMMATRIX XM_CALLCONV NiTransformGetScale(
		const NiTransform& a_in);

	DirectX::XMMATRIX XM_CALLCONV NiTransformGetTranslation(
		const NiTransform& a_in);

	void XM_CALLCONV GetCameraPV(
		NiCamera*          a_camera,
		DirectX::XMMATRIX& a_view,
		DirectX::XMMATRIX& a_proj);

	void XM_CALLCONV RayCastScreenPt(
		const CD3D11_VIEWPORT& a_viewport,
		DirectX::CXMMATRIX     a_view,
		DirectX::CXMMATRIX     a_proj,
		float                  a_x,
		float                  a_y,
		DirectX::XMVECTOR&     a_rayOrigin,
		DirectX::XMVECTOR&     a_rayDir);

	DirectX::XMVECTOR XM_CALLCONV ScreenSpaceToWorld(
		const CD3D11_VIEWPORT& a_viewport,
		DirectX::XMVECTOR      a_pos,
		DirectX::CXMMATRIX     a_view,
		DirectX::CXMMATRIX     a_proj);

	DirectX::XMVECTOR XM_CALLCONV WorldToScreenSpace(
		const CD3D11_VIEWPORT& a_viewport,
		DirectX::CXMMATRIX     a_view,
		DirectX::CXMMATRIX     a_proj,
		DirectX::CXMMATRIX     a_world);

	DirectX::XMVECTOR XM_CALLCONV WorldToScreenSpace(
		const CD3D11_VIEWPORT& a_viewport,
		DirectX::XMVECTOR      a_pos,
		DirectX::CXMMATRIX     a_view,
		DirectX::CXMMATRIX     a_proj,
		DirectX::CXMMATRIX     a_world);
	
	DirectX::XMVECTOR XM_CALLCONV WorldToScreenSpace(
		const CD3D11_VIEWPORT& a_viewport,
		DirectX::XMVECTOR      a_pos,
		DirectX::CXMMATRIX     a_view,
		DirectX::CXMMATRIX     a_proj);

	DirectX::XMFLOAT2 XM_CALLCONV WorldToScreenSpacePt2(
		const CD3D11_VIEWPORT& a_viewport,
		DirectX::CXMMATRIX     a_view,
		DirectX::CXMMATRIX     a_proj,
		DirectX::CXMMATRIX     a_world);

	DirectX::XMFLOAT2 XM_CALLCONV WorldToScreenSpacePt2(
		const CD3D11_VIEWPORT& a_viewport,
		DirectX::XMVECTOR      a_pos,
		DirectX::CXMMATRIX     a_view,
		DirectX::CXMMATRIX     a_proj,
		DirectX::CXMMATRIX     a_world);

}
