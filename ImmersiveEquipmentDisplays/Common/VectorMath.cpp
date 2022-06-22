#include "VectorMath.h"

#include <d3d11.h>

#include <skse64/NiObjects.h>
#include <skse64/NiTypes.h>

namespace VectorMath
{
	using namespace DirectX;

	XMMATRIX XM_CALLCONV NiTransformTo4x4Matrix(
		const NiTransform& a_in)
	{
		XMMATRIX rot = {
			a_in.rot.GetColMM(0),
			a_in.rot.GetColMM(1),
			a_in.rot.GetColMM(2),
			g_XMIdentityR3
		};

		auto scale = XMMatrixScalingFromVector(
			XMVectorReplicate(a_in.scale));

		auto trans = XMMatrixTranslation(
			a_in.pos.x,
			a_in.pos.y,
			a_in.pos.z);

		return (rot * scale) * trans;
	}

	XMMATRIX XM_CALLCONV NiTransformGetRotation(const NiTransform& a_in)
	{
		return {
			a_in.rot.GetColMM(0),
			a_in.rot.GetColMM(1),
			a_in.rot.GetColMM(2),
			g_XMIdentityR3
		};
	}

	XMMATRIX XM_CALLCONV NiTransformGetScale(const NiTransform& a_in)
	{
		return XMMatrixScalingFromVector(
			XMVectorReplicate(a_in.scale));
	}

	XMMATRIX XM_CALLCONV NiTransformGetTranslation(const NiTransform& a_in)
	{
		return XMMatrixTranslation(
			a_in.pos.x,
			a_in.pos.y,
			a_in.pos.z);
	}

	void XM_CALLCONV GetCameraPV(
		NiCamera* a_camera,
		XMMATRIX& a_view,
		XMMATRIX& a_proj)
	{
		const auto worldDir = a_camera->m_worldTransform.rot.GetColMM(0);
		const auto worldUp  = a_camera->m_worldTransform.rot.GetColMM(1);
		const auto worldPos = a_camera->m_worldTransform.pos.GetMM();

		a_view = XMMatrixLookAtRH(
			worldPos,
			worldPos + worldDir,
			worldUp);

		const auto& frustum = a_camera->m_frustum;

		if (frustum.m_bOrtho)
		{
			a_proj = XMMatrixOrthographicOffCenterRH(
				frustum.m_fLeft,
				frustum.m_fRight,
				frustum.m_fBottom,
				frustum.m_fTop,
				frustum.m_fNear,
				frustum.m_fFar);
		}
		else
		{
			a_proj = XMMatrixPerspectiveOffCenterRH(
				frustum.m_fLeft * frustum.m_fNear,
				frustum.m_fRight * frustum.m_fNear,
				frustum.m_fBottom * frustum.m_fNear,
				frustum.m_fTop * frustum.m_fNear,
				frustum.m_fNear,
				frustum.m_fFar);
		}
	}

	static const XMMATRIX s_identity{
		g_XMIdentityR0.v,
		g_XMIdentityR1.v,
		g_XMIdentityR2.v,
		g_XMIdentityR3.v
	};

	void XM_CALLCONV RayCastScreenPt(
		const CD3D11_VIEWPORT& a_viewport,
		CXMMATRIX              a_view,
		CXMMATRIX              a_proj,
		float                  a_x,
		float                  a_y,
		XMVECTOR&              a_rayOrigin,
		XMVECTOR&              a_rayDir)
	{
		auto v = XMVector3Unproject(
			XMVectorSet(a_x, a_y, 1.0f, 0.0f),
			a_viewport.TopLeftX,
			a_viewport.TopLeftY,
			a_viewport.Width,
			a_viewport.Height,
			a_viewport.MinDepth,
			a_viewport.MaxDepth,
			a_proj,
			a_view,
			s_identity);

		a_rayOrigin = XMVector3Unproject(
			XMVectorSet(a_x, a_y, 0.0f, 0.0f),
			a_viewport.TopLeftX,
			a_viewport.TopLeftY,
			a_viewport.Width,
			a_viewport.Height,
			a_viewport.MinDepth,
			a_viewport.MaxDepth,
			a_proj,
			a_view,
			s_identity);

		a_rayDir = XMVector3Normalize(v - a_rayOrigin);
	}

	XMVECTOR XM_CALLCONV ScreenSpaceToWorld(
		const CD3D11_VIEWPORT& a_viewport,
		XMVECTOR               a_pos,
		CXMMATRIX              a_view,
		CXMMATRIX              a_proj)
	{
		return XMVector3Unproject(
			a_pos,
			a_viewport.TopLeftX,
			a_viewport.TopLeftY,
			a_viewport.Width,
			a_viewport.Height,
			a_viewport.MinDepth,
			a_viewport.MaxDepth,
			a_proj,
			a_view,
			s_identity);
	}

	XMVECTOR XM_CALLCONV WorldToScreenSpace(
		const CD3D11_VIEWPORT& a_viewport,
		CXMMATRIX              a_view,
		CXMMATRIX              a_proj,
		CXMMATRIX              a_world)
	{
		return XMVector3Project(
			g_XMZero,
			a_viewport.TopLeftX,
			a_viewport.TopLeftY,
			a_viewport.Width,
			a_viewport.Height,
			a_viewport.MinDepth,
			a_viewport.MaxDepth,
			a_proj,
			a_view,
			a_world);
	}

	XMVECTOR XM_CALLCONV WorldToScreenSpace(
		const CD3D11_VIEWPORT& a_viewport,
		XMVECTOR               a_pos,
		CXMMATRIX              a_view,
		CXMMATRIX              a_proj,
		CXMMATRIX              a_world)
	{
		return XMVector3Project(
			a_pos,
			a_viewport.TopLeftX,
			a_viewport.TopLeftY,
			a_viewport.Width,
			a_viewport.Height,
			a_viewport.MinDepth,
			a_viewport.MaxDepth,
			a_proj,
			a_view,
			a_world);
	}

	XMVECTOR XM_CALLCONV WorldToScreenSpace(
		const CD3D11_VIEWPORT& a_viewport,
		XMVECTOR               a_pos,
		CXMMATRIX              a_view,
		CXMMATRIX              a_proj)
	{
		return XMVector3Project(
			a_pos,
			a_viewport.TopLeftX,
			a_viewport.TopLeftY,
			a_viewport.Width,
			a_viewport.Height,
			a_viewport.MinDepth,
			a_viewport.MaxDepth,
			a_proj,
			a_view,
			s_identity);
	}

	XMFLOAT2 XM_CALLCONV WorldToScreenSpacePt2(
		const CD3D11_VIEWPORT& a_viewport,
		CXMMATRIX              a_view,
		CXMMATRIX              a_proj,
		CXMMATRIX              a_world)
	{
		auto p = WorldToScreenSpace(
			a_viewport,
			a_view,
			a_proj,
			a_world);

		return {
			XMVectorGetX(p),
			XMVectorGetY(p)
		};
	}

	XMFLOAT2 XM_CALLCONV WorldToScreenSpacePt2(
		const CD3D11_VIEWPORT& a_viewport,
		XMVECTOR               a_pos,
		CXMMATRIX              a_view,
		CXMMATRIX              a_proj,
		CXMMATRIX              a_world)
	{
		auto p = WorldToScreenSpace(
			a_viewport,
			a_pos,
			a_view,
			a_proj,
			a_world);

		return {
			XMVectorGetX(p),
			XMVectorGetY(p)
		};
	}
}