#include "pch.h"

#include "I3DIObjectCamera.h"

#include "Common/VectorMath.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;
		using namespace VectorMath;

		I3DIObjectCamera::I3DIObjectCamera(NiCamera* a_camera) noexcept :
			I3DICamera(a_camera)
		{
			const auto world = XMQuaternionRotationMatrix(VectorMath::NiTransformGetRotationMatrix(a_camera->m_worldTransform));

			m_xfrm.qs = world;

		}

		void I3DIObjectCamera::CameraSetTranslation()
		{
		}

		void I3DIObjectCamera::CameraProcessMouseInput(const Handlers::MouseMoveEvent& a_evn)
		{
		}

		void I3DIObjectCamera::CameraUpdate(NiCamera* a_camera)
		{
			
		}

	}
}