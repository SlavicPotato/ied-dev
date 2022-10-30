#include "pch.h"

#include "I3DIBoundingOrientedBox.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		void XM_CALLCONV I3DIBoundingOrientedBox::Draw(
			D3DPrimitiveBatch& a_batch,
			XMVECTOR           a_color) const
		{
			m_bound.DrawBox(a_batch, a_color);
		}

		bool XM_CALLCONV I3DIBoundingOrientedBox::Intersects(
			XMVECTOR a_origin,
			XMVECTOR a_direction,
			float&   a_distance) const
		{
			return m_bound.Intersects(a_origin, a_direction, a_distance);
		}

		float XM_CALLCONV I3DIBoundingOrientedBox::GetCenterDistance(
			XMVECTOR a_origin) const
		{
			return XMVectorGetX(XMVector3Length(I3DIBoundingOrientedBox::GetBoundingShapeCenter() - a_origin));
		}

		float XM_CALLCONV I3DIBoundingOrientedBox::GetCenterDistanceSq(
			XMVECTOR a_origin) const
		{
			return XMVectorGetX(XMVector3LengthSq(I3DIBoundingOrientedBox::GetBoundingShapeCenter() - a_origin));
		}

		XMVECTOR XM_CALLCONV I3DIBoundingOrientedBox::GetBoundingShapeCenter() const
		{
			return XMLoadFloat3(std::addressof(m_bound.Center));
		}
	}
}