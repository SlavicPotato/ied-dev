#include "pch.h"

#include "I3DICamera.h"

#include "Common/VectorMath.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;
		using namespace VectorMath;

		I3DICamera::TransformData::TransformData(NiCamera* a_camera) :
			mb(NiTransformGetRotationMatrix(a_camera->m_localTransform)),
			t(NiTransformGetPosition(a_camera->m_worldTransform)),
			s(XMVectorReplicate(a_camera->m_worldTransform.scale))
		{
			qb = XMQuaternionRotationMatrix(mb);
			qs = qt = qb;
		}

		I3DICamera::I3DICamera(NiCamera* a_camera) noexcept :
			m_xfrm(a_camera)
		{
		}

		void I3DICamera::CameraUpdate(NiCamera* a_camera)
		{
			a_camera->m_worldTransform = CreateNiTransformTransposed(m_xfrm.t, m_xfrm.q, m_xfrm.s);
		}

		void I3DICamera::CameraComputeRotation(float a_step)
		{
			const auto factor = std::min(a_step * m_settings.rotationInterpSpeed, 1.0f);
			const auto qn     = XMQuaternionSlerpCubic(m_xfrm.qs, m_xfrm.qt, factor);

			m_xfrm.qs = qn;
			m_xfrm.q  = XMQuaternionMultiply(qn, m_xfrm.qb);
		}

	}
}