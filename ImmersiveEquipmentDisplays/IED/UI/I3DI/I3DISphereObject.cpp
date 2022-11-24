#include "pch.h"

#include "I3DISphereObject.h"

#include "I3DICommonData.h"

namespace IED
{
	namespace UI
	{
		I3DISphereObject::I3DISphereObject(
			I3DICommonData&  a_data,
			I3DIBoundObject* a_parent) noexcept(false) :
			I3DIModelObject(
				a_data.scene.GetDevice().Get(),
				BoundingShape::kSphere,
				a_data.assets.GetModel(I3DIModelID::kSphere)),
			m_parent(a_parent)
		{
		}

		bool I3DISphereObject::ObjectIntersects(
			I3DICommonData& a_data,
			const I3DIRay&  a_ray,
			float&          a_dist)
		{
			return false;
		}

		I3DIBoundObject* I3DISphereObject::GetParentObject() const
		{
			return m_parent;
		}

		void I3DISphereObject::OnMouseMoveOver(I3DICommonData& a_data)
		{
		}

		void I3DISphereObject::OnMouseMoveOut(I3DICommonData& a_data)
		{
		}
		void I3DISphereObject::DrawObjectExtra(I3DICommonData& a_data)
		{
		}
	}
}