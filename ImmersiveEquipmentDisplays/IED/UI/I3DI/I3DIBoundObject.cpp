#include "pch.h"

#include "I3DIBoundObject.h"

#include "I3DIBoundingOrientedBox.h"
#include "I3DIBoundingSphere.h"
#include "I3DIRay.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		I3DIBoundObject::I3DIBoundObject(BoundingShape a_type) noexcept(false) :
			m_type(a_type)
		{
			switch (a_type)
			{
			case BoundingShape::kOrientedBox:
				m_bound = std::make_unique<I3DIBoundingOrientedBox>();
				break;
			case BoundingShape::kSphere:
				m_bound = std::make_unique<I3DIBoundingSphere>();
				break;
			default:
				throw std::runtime_error(__FUNCTION__ ": invalid bounding shape");
			}
		}

		bool I3DIBoundObject::ObjectIntersects(
			I3DICommonData& a_data,
			const I3DIRay&  a_ray,
			float&          a_dist)
		{
			return Intersects(a_ray.origin, a_ray.dir, a_dist);
		}

		void XM_CALLCONV I3DIBoundObject::DrawBoundingShape(
			D3DPrimitiveBatch& a_batch,
			XMVECTOR           a_color) const
		{
			m_bound->Draw(a_batch, a_color);
		}

		bool XM_CALLCONV I3DIBoundObject::Intersects(
			XMVECTOR a_origin,
			XMVECTOR a_direction,
			float&   a_distance) const
		{
			return m_bound->Intersects(a_origin, a_direction, a_distance);
		}

		float XM_CALLCONV I3DIBoundObject::GetCenterDistance(
			XMVECTOR a_origin) const
		{
			return m_bound->GetCenterDistance(a_origin);
		}

		float XM_CALLCONV I3DIBoundObject::GetCenterDistanceSq(
			XMVECTOR a_origin) const
		{
			return m_bound->GetCenterDistanceSq(a_origin);
		}

		XMVECTOR XM_CALLCONV I3DIBoundObject::GetBoundingShapeCenter() const
		{
			return m_bound->GetBoundingShapeCenter();
		}
	}
}