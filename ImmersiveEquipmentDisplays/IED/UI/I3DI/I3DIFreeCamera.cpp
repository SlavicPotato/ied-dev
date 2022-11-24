#include "pch.h"

#include "I3DIFreeCamera.h"

#include "I3DIInputHelpers.h"

#include "Common/VectorMath.h"

#include "Drivers/Input/Handlers.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;
		using namespace VectorMath;

		I3DIFreeCamera::I3DIFreeCamera(NiCamera* a_camera) noexcept :
			I3DICamera(a_camera)
		{
			const auto world = XMQuaternionRotationMatrix(NiTransformGetRotationMatrix(a_camera->m_worldTransform));
			const auto d     = XMQuaternionMultiply(world, XMQuaternionInverse(m_xfrm.qb));

			m_xfrm.qs = m_xfrm.qt = d;
		}

		void I3DIFreeCamera::CameraSetTranslation()
		{
			if (!m_fSettings.enableTranslation)
			{
				m_velocity.d = g_XMZero.v;
				return;
			}

			const auto& io = ImGui::GetIO();

			auto d = g_XMZero.v;

			if (io.KeysDown[0x0057])
			{
				d += g_XMIdentityR0.v;
			}

			if (io.KeysDown[0x0053])
			{
				d -= g_XMIdentityR0.v;
			}

			if (io.KeysDown[0x0044])
			{
				d += g_XMIdentityR2.v;
			}

			if (io.KeysDown[0x0041])
			{
				d -= g_XMIdentityR2.v;
			}

			if (io.MouseDown[ImGuiMouseButton_Left])
			{
				d += g_XMIdentityR1.v;
			}

			if (io.MouseDown[ImGuiMouseButton_Right])
			{
				d -= g_XMIdentityR1.v;
			}

			m_velocity.d = XMVector3Normalize(d);

			if (io.KeyShift)
			{
				constexpr XMVECTOR sbm = { 4, 4, 4, 0 };

				m_velocity.d *= sbm;
			}
		}

		void I3DIFreeCamera::CameraProcessMouseInput(const Handlers::MouseMoveEvent& a_evn)
		{
			const auto speed = m_fSettings.lookSpeed;

			const float x = static_cast<float>(-a_evn.inputX) * speed;
			const float y = static_cast<float>(-a_evn.inputY) * speed;

			const auto target = XMQuaternionMultiply(m_xfrm.qt, XMQuaternionRotationNormal(g_XMIdentityR1.v, x));
			m_xfrm.qt         = XMQuaternionMultiply(XMQuaternionRotationNormal(g_XMIdentityR2.v, y), target);
		}

		void I3DIFreeCamera::CameraUpdate(NiCamera* a_camera)
		{
			const auto step  = *Game::g_frameTimer;
			const auto stepv = XMVectorReplicate(step);

			CameraComputeRotation(step);

			// calculate velocity

			auto v = m_velocity.v;

			v -= v * XMVectorMin(stepv * m_fSettings.translateDamping, g_XMOne.v);

			const auto vdn = XMVector3Rotate(m_velocity.d, m_xfrm.q);
			v += vdn * stepv * m_fSettings.translateAccel;

			// apply velocity limit

			const auto vd = XMVector3Normalize(v);
			const auto vm = XMVector3Length(v);

			v = vd * XMVectorMin(vm, m_fSettings.translateMaxVelocity);

			m_velocity.v = v;

			// advance position

			m_xfrm.t += v * stepv * m_fSettings.translateSpeed;

			I3DICamera::CameraUpdate(a_camera);
		}

	}
}