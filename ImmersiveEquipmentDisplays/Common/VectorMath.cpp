#include "pch.h"

#include "VectorMath.h"

#pragma warning(disable: 4073)
#pragma init_seg(lib)

namespace VectorMath
{
	using namespace DirectX;

	const __declspec(selectany) XMMATRIX g_identity{
		g_XMIdentityR0.v,
		g_XMIdentityR1.v,
		g_XMIdentityR2.v,
		g_XMIdentityR3.v
	};

	XMMATRIX XM_CALLCONV NiTransformToMatrix4x4(
		const NiTransform& a_in)
	{
		const auto mr = NiTransformGetRotationMatrix(a_in);
		const auto ms = NiTransformGetScalingMatrix(a_in);
		const auto mt = NiTransformGetTranslationMatrix(a_in);

		return mr * ms * mt;
	}

	NiTransform XM_CALLCONV Matrix4x4ToNiTransform(
		XMMATRIX a_in)
	{
		XMVECTOR t, q, s;

		if (!XMMatrixDecompose(
				std::addressof(s),
				std::addressof(q),
				std::addressof(t),
				a_in))
		{
			return {};
		}
		else
		{
			return CreateNiTransformTransposed(t, q, s);
		}
	}

	NiTransform XM_CALLCONV CreateNiTransformTransposed(
		XMVECTOR a_t,
		XMVECTOR a_q,
		XMVECTOR a_s)
	{
		NiTransform result(NiTransform::noinit_arg_t{});

		const auto m = XMMatrixTranspose(XMMatrixRotationQuaternion(a_q));

		static_assert(offsetof(NiTransform, rot) == 0x0);
		static_assert(offsetof(NiTransform, pos) == 0x24);
		static_assert(offsetof(NiTransform, scale) == 0x30);
		static_assert(sizeof(NiTransform) == 0x34);

		// w overflows in to the next member so writes need to be ordered

		_mm_storeu_ps(result.rot.data[0], m.r[0]);
		_mm_storeu_ps(result.rot.data[1], m.r[1]);
		_mm_storeu_ps(result.rot.data[2], m.r[2]);
		_mm_storeu_ps(result.pos, a_t);
		result.scale = XMVectorGetX(a_s);

		return result;
	}

	NiTransform XM_CALLCONV CreateNiTransform(
		XMVECTOR a_t,
		XMVECTOR a_q,
		XMVECTOR a_s)
	{
		const auto m = XMMatrixRotationQuaternion(a_q);

		NiTransform result(NiTransform::noinit_arg_t{});

		static_assert(offsetof(NiTransform, rot) == 0x0);
		static_assert(offsetof(NiTransform, pos) == 0x24);
		static_assert(offsetof(NiTransform, scale) == 0x30);
		static_assert(sizeof(NiTransform) == 0x34);

		// w overflows in to the next member so writes need to be ordered

		_mm_storeu_ps(result.rot.data[0], m.r[0]);
		_mm_storeu_ps(result.rot.data[1], m.r[1]);
		_mm_storeu_ps(result.rot.data[2], m.r[2]);
		_mm_storeu_ps(result.pos, a_t);
		result.scale = XMVectorGetX(a_s);

		return result;
	}

	XMMATRIX XM_CALLCONV NiTransformGetRotationMatrix(const NiTransform& a_in)
	{
		return {
			a_in.rot.GetColMM(0),
			a_in.rot.GetColMM(1),
			a_in.rot.GetColMM(2),
			g_XMIdentityR3.v
		};
	}

	XMMATRIX XM_CALLCONV NiTransformGetScalingMatrix(const NiTransform& a_in)
	{
		return XMMatrixScalingFromVector(
			XMVectorReplicate(a_in.scale));
	}

	XMMATRIX XM_CALLCONV NiTransformGetTranslationMatrix(const NiTransform& a_in)
	{
		return XMMatrixTranslationFromVector(a_in.pos.GetMM());
	}

	XMVECTOR XM_CALLCONV NiTransformGetPosition(const NiTransform& a_in)
	{
		return a_in.pos.GetMM();
	}

	void GetCameraPV(
		NiCamera* a_camera,
		XMMATRIX& a_view,
		XMMATRIX& a_proj,
		XMVECTOR& a_pos)
	{
		const auto& transform = a_camera->m_worldTransform;
		const auto& frustum   = a_camera->m_frustum;

		const auto worldDir = transform.rot.GetColMM(0);
		const auto worldUp  = transform.rot.GetColMM(1);
		const auto worldPos = transform.pos.GetMM();

		a_pos = worldPos;

		a_view = XMMatrixLookToRH(
			worldPos,
			worldDir,
			worldUp);

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

	void RayCastScreenPt(
		const CD3D11_VIEWPORT& a_viewport,
		CXMMATRIX              a_view,
		CXMMATRIX              a_proj,
		float                  a_x,
		float                  a_y,
		XMVECTOR&              a_rayOrigin,
		XMVECTOR&              a_rayDir)
	{
		const auto v = XMVector3Unproject(
			XMVectorSet(a_x, a_y, 1.0f, 0.0f),
			a_viewport.TopLeftX,
			a_viewport.TopLeftY,
			a_viewport.Width,
			a_viewport.Height,
			a_viewport.MinDepth,
			a_viewport.MaxDepth,
			a_proj,
			a_view,
			g_identity);

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
			g_identity);

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
			g_identity);
	}

	XMVECTOR XM_CALLCONV WorldToScreenSpace(
		const CD3D11_VIEWPORT& a_viewport,
		CXMMATRIX              a_view,
		CXMMATRIX              a_proj,
		CXMMATRIX              a_world)
	{
		return XMVector3Project(
			g_XMZero.v,
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
			g_identity);
	}

	XMFLOAT2 XM_CALLCONV WorldToScreenSpacePt2(
		const CD3D11_VIEWPORT& a_viewport,
		CXMMATRIX              a_view,
		CXMMATRIX              a_proj,
		CXMMATRIX              a_world)
	{
		const auto p = WorldToScreenSpace(
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
		const auto p = WorldToScreenSpace(
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

	XMVECTOR XM_CALLCONV XMQuaternionSlerpCubic(
		XMVECTOR a_from,
		XMVECTOR a_to,
		float    a_factor)
	{
		return XMQuaternionSlerp(
			a_from,
			a_to,
			a_factor * a_factor * (3.0f - 2.0f * a_factor));
	}

	XMVECTOR XM_CALLCONV XMVectorLerpCubic(
		XMVECTOR a_from,
		XMVECTOR a_to,
		float    a_factor)
	{
		return XMVectorLerp(
			a_from,
			a_to,
			a_factor * a_factor * (3.0f - 2.0f * a_factor));
	}

	XMVECTOR XM_CALLCONV XMVectorConvertToRadians(XMVECTOR a_degrees)
	{
		return a_degrees * g_pidiv180;
	}
}