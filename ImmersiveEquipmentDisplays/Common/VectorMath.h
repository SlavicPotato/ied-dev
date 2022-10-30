#pragma once

namespace VectorMath
{
	extern const DirectX::XMMATRIX g_identity;

	/*inline extern const __declspec(selectany) DirectX::XMVECTOR g_negFltMax{
		-FLT_MAX,
		-FLT_MAX,
		-FLT_MAX,
		-FLT_MAX,
	};*/

	DirectX::XMMATRIX XM_CALLCONV NiTransformToMatrix4x4(
		const NiTransform& a_in);

	NiTransform XM_CALLCONV Matrix4x4ToNiTransform(
		DirectX::XMMATRIX a_in);

	NiTransform XM_CALLCONV CreateNiTransformTransposed(
		DirectX::XMVECTOR a_t,
		DirectX::XMVECTOR a_q,
		DirectX::XMVECTOR a_s);

	NiTransform XM_CALLCONV CreateNiTransform(
		DirectX::XMVECTOR a_t,
		DirectX::XMVECTOR a_q,
		DirectX::XMVECTOR a_s);

	DirectX::XMMATRIX XM_CALLCONV NiTransformGetRotationMatrix(
		const NiTransform& a_in);

	DirectX::XMMATRIX XM_CALLCONV NiTransformGetScalingMatrix(
		const NiTransform& a_in);

	DirectX::XMMATRIX XM_CALLCONV NiTransformGetTranslationMatrix(
		const NiTransform& a_in);

	DirectX::XMVECTOR XM_CALLCONV NiTransformGetPosition(
		const NiTransform& a_in);

	void GetCameraPV(
		NiCamera*          a_camera,
		DirectX::XMMATRIX& a_view,
		DirectX::XMMATRIX& a_proj,
		DirectX::XMVECTOR& a_pos);

	void RayCastScreenPt(
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

	DirectX::XMVECTOR XM_CALLCONV XMQuaternionSlerpCubic(
		DirectX::XMVECTOR a_from,
		DirectX::XMVECTOR a_to,
		float             a_factor);

	DirectX::XMVECTOR XM_CALLCONV XMVectorLerpCubic(
		DirectX::XMVECTOR a_from,
		DirectX::XMVECTOR a_to,
		float             a_factor);

}
