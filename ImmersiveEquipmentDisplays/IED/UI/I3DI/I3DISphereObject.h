#pragma once

#include "IED/ConfigNodePhysicsValues.h"

//#include "I3DIBoundObject.h"
#include "I3DIModelObject.h"

namespace IED
{
	namespace UI
	{
		struct I3DICommonData;

		class I3DISphereObject :
			public I3DIModelObject
		{
		public:
			I3DISphereObject(
				I3DICommonData& a_data,
				I3DIBoundObject *a_parent = nullptr) noexcept(false);

			virtual bool ObjectIntersects(
				I3DICommonData& a_data,
				const I3DIRay&  a_ray,
				float&          a_dist) override;

			virtual I3DIBoundObject* GetParentObject() const override;

		private:
			virtual void OnMouseMoveOver(I3DICommonData& a_data) override;
			virtual void OnMouseMoveOut(I3DICommonData& a_data) override;

			virtual void DrawObjectExtra(I3DICommonData& a_data) override;

			I3DIBoundObject* m_parent;
		};
	}
}
