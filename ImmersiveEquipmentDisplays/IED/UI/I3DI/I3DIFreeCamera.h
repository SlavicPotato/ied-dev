#pragma once

#include "I3DICamera.h"

namespace IED
{
	namespace Handlers
	{
		struct MouseMoveEvent;
	}

	namespace UI
	{
		class I3DIFreeCamera :
			public I3DICamera
		{
			struct VelocityData
			{
				DirectX::XMVECTOR d{ DirectX::g_XMZero.v };
				DirectX::XMVECTOR v{ DirectX::g_XMZero.v };
			};

			struct FreeCamSettings
			{
				DirectX::XMVECTOR translateDamping{ _mm_set_ps1(8.0f) };
				DirectX::XMVECTOR translateAccel{ _mm_set_ps1(0.01f) };
				DirectX::XMVECTOR translateMaxVelocity{ _mm_set_ps1(0.2f) };
				DirectX::XMVECTOR translateSpeed{ _mm_set_ps1(100000.0f) };

				float lookSpeed{ 0.0025f };
				bool  enableTranslation{ true };
			};

		public:
			I3DIFreeCamera(NiCamera* a_camera) noexcept;
			virtual ~I3DIFreeCamera() noexcept override = default;

			virtual void CameraSetTranslation() override;
			virtual void CameraProcessMouseInput(const Handlers::MouseMoveEvent& a_evn) override;
			virtual void CameraUpdate(NiCamera* a_camera) override;

			inline constexpr void EnableTranslation(bool a_switch) noexcept
			{
				m_fSettings.enableTranslation = a_switch;
			}

		private:
			VelocityData    m_velocity;
			FreeCamSettings m_fSettings;
		};
	}
}