#include "pch.h"

#include "D3DBoundingOrientedBox.h"

#include "D3DCommon.h"
#include "D3DPrimitiveBatch.h"

namespace IED
{
	using namespace DirectX;

	void XM_CALLCONV D3DBoundingOrientedBox::DrawBox(
		D3DPrimitiveBatch& a_batch,
		XMVECTOR           a_color) const
	{
		const auto center      = XMLoadFloat3(std::addressof(Center));
		const auto extents     = XMLoadFloat3(std::addressof(Extents));
		const auto orientation = XMLoadFloat4(std::addressof(Orientation));

		const auto p1 = GetPoint(DirectX::g_BoxOffset[0], center, extents, orientation);
		const auto p2 = GetPoint(DirectX::g_BoxOffset[1], center, extents, orientation);
		const auto p3 = GetPoint(DirectX::g_BoxOffset[2], center, extents, orientation);
		const auto p4 = GetPoint(DirectX::g_BoxOffset[3], center, extents, orientation);
		const auto p5 = GetPoint(DirectX::g_BoxOffset[4], center, extents, orientation);
		const auto p6 = GetPoint(DirectX::g_BoxOffset[5], center, extents, orientation);
		const auto p7 = GetPoint(DirectX::g_BoxOffset[6], center, extents, orientation);
		const auto p8 = GetPoint(DirectX::g_BoxOffset[7], center, extents, orientation);

		a_batch.AddLine(p1, p2, a_color);
		a_batch.AddLine(p2, p3, a_color);
		a_batch.AddLine(p3, p4, a_color);
		a_batch.AddLine(p4, p1, a_color);
		a_batch.AddLine(p1, p5, a_color);
		a_batch.AddLine(p2, p6, a_color);
		a_batch.AddLine(p3, p7, a_color);
		a_batch.AddLine(p4, p8, a_color);
		a_batch.AddLine(p5, p6, a_color);
		a_batch.AddLine(p6, p7, a_color);
		a_batch.AddLine(p7, p8, a_color);
		a_batch.AddLine(p8, p5, a_color);
	}

	XMVECTOR XM_CALLCONV D3DBoundingOrientedBox::GetPoint(XMVECTOR a_direction) const
	{
		const auto center      = XMLoadFloat3(std::addressof(Center));
		const auto extents     = XMLoadFloat3(std::addressof(Extents));
		const auto orientation = XMLoadFloat4(std::addressof(Orientation));

		return GetPoint(a_direction, center, extents, orientation);
	}

	XMVECTOR XM_CALLCONV D3DBoundingOrientedBox::GetPopupAnchorPoint(
		D3DCommon& a_scene,
		XMVECTOR   a_direction,
		float      a_offset,
		XMVECTOR&  a_origin) const
	{
		const auto center      = XMLoadFloat3(std::addressof(Center));
		const auto extents     = XMLoadFloat3(std::addressof(Extents));
		const auto orientation = XMLoadFloat4(std::addressof(Orientation));

		a_origin = GetPoint(a_direction, center, extents, orientation);

		return VectorMath::WorldToScreenSpace(
			a_scene.GetViewport(),
			GetPoint(a_direction, center, extents + XMVectorReplicate(a_offset), orientation),
			a_scene.GetViewMatrix(),
			a_scene.GetProjectionMatrix());
	}

}