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
				DirectX::XMVECTOR translateDamping{ 8.0f, 8.0f, 8.0f, 8.0f };
				DirectX::XMVECTOR translateAccel{ 0.01f, 0.01f, 0.01f, 0.01f };
				DirectX::XMVECTOR translateMaxVelocity{ 0.2f, 0.2f, 0.2f, 0.2f };
				DirectX::XMVECTOR translateSpeed{ 100000.0f, 100000.0f, 100000.0f, 100000.0f };

				float lookSpeed{ 0.0025f };
				bool  enableTranslation{ true };
			};

		public:
			I3DIFreeCamera(NiCamera* a_camera) noexcept;
			virtual ~I3DIFreeCamera() noexcept override = default;

			virtual void CameraSetTranslation() override;
			virtual void CameraProcessMouseInput(const Handlers::MouseMoveEvent& a_evn) override;
			virtual void CameraUpdate(NiCamera* a_camera) override;

			constexpr void EnableTranslation(bool a_switch) noexcept
			{
				m_fSettings.enableTranslation = a_switch;
			}

		private:
			VelocityData    m_velocity;
			FreeCamSettings m_fSettings;
		};
	}
}