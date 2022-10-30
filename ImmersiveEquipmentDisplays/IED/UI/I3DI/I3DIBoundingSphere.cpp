#include "pch.h"

#include "I3DIBoundingSphere.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		void XM_CALLCONV I3DIBoundingSphere::Draw(
			D3DPrimitiveBatch& a_batch,
			XMVECTOR           a_color) const
		{
			m_bound.DrawSphere(a_batch, a_color);
		}

		bool XM_CALLCONV I3DIBoundingSphere::Intersects(
			XMVECTOR a_origin,
			XMVECTOR a_direction,
			float&   a_distance) const
		{
			return m_bound.Intersects(a_origin, a_direction, a_distance);
		}

		float XM_CALLCONV I3DIBoundingSphere::GetCenterDistance(
			XMVECTOR a_origin) const
		{
			return XMVectorGetX(XMVector3Length(I3DIBoundingSphere::GetBoundingShapeCenter() - a_origin));
		}

		float XM_CALLCONV I3DIBoundingSphere::GetCenterDistanceSq(
			XMVECTOR a_origin) const
		{
			return XMVectorGetX(XMVector3LengthSq(I3DIBoundingSphere::GetBoundingShapeCenter() - a_origin));
		}

		XMVECTOR XM_CALLCONV I3DIBoundingSphere::GetBoundingShapeCenter() const
		{
			return XMLoadFloat3(std::addressof(m_bound.Center));
		}
	}
}