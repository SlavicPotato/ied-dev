#include "pch.h"

#include "I3DIFreeCamera.h"

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
				m_velocity.d = g_XMZero;
				return;
			}

			auto& io = ImGui::GetIO();

			float fwd, lr, ud;

			if (io.KeysDown[0x0057])
			{
				fwd = 1;
			}
			else if (io.KeysDown[0x0053])
			{
				fwd = -1;
			}
			else
			{
				fwd = 0;
			}

			if (io.KeysDown[0x0044])
			{
				lr = 1;
			}
			else if (io.KeysDown[0x0041])
			{
				lr = -1;
			}
			else
			{
				lr = 0;
			}

			if (io.MouseDown[0])
			{
				ud = 1;
			}
			else if (io.MouseDown[1])
			{
				ud = -1;
			}
			else
			{
				ud = 0;
			}

			m_velocity.d = XMVectorSet(fwd, ud, lr, 0);
		}

		void I3DIFreeCamera::CameraProcessMouseInput(const Handlers::MouseMoveEvent& a_evn)
		{
			const auto speed = m_fSettings.lookSpeed;

			const float x = static_cast<float>(-a_evn.inputX) * speed;
			const float y = static_cast<float>(-a_evn.inputY) * speed;

			auto target = m_xfrm.qt;

			target    = XMQuaternionMultiply(target, XMQuaternionRotationNormal(g_XMIdentityR1.v, x));
			m_xfrm.qt = XMQuaternionMultiply(XMQuaternionRotationNormal(g_XMIdentityR2.v, y), target);
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