#include "pch.h"

#include "I3DISphereObject.h"

#include "I3DICommonData.h"

namespace IED
{
	namespace UI
	{
		I3DISphereObject::I3DISphereObject(
			I3DICommonData& a_data) noexcept(false) :
			I3DIModelObject(
				a_data.scene.GetDevice().Get(),
				BoundingShape::kSphere,
				a_data.assets.GetModel(I3DIModelID::kSphere))
		{
		}

		bool I3DISphereObject::ObjectIntersects(
			I3DICommonData& a_data,
			const I3DIRay&  a_ray,
			float&          a_dist)
		{
			return false;
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