#pragma once

namespace IED
{
	namespace Handlers
	{
		struct MouseMoveEvent;
	}

	namespace UI
	{
		class I3DIMain;

		class I3DICamera
		{
			friend class I3DIMain;

			struct TransformData
			{
				TransformData(NiCamera* a_camera);

				DirectX::XMVECTOR t;                               // position
				DirectX::XMVECTOR s;                               // scale
				DirectX::XMVECTOR q{ DirectX::g_XMIdentityR3.v };  // final rotation
				DirectX::XMVECTOR qs;                              // current pitch/yaw rotation
				DirectX::XMVECTOR qt;                              // target pitch/yaw rotation
				DirectX::XMVECTOR qb;                              // base rotation
				DirectX::XMMATRIX mb;                              // base rotation
			};

			struct Settings
			{
				float rotationInterpSpeed{ 40.0f };
			};

		public:
			I3DICamera(NiCamera* a_camera) noexcept;
			virtual ~I3DICamera() noexcept = default;

			virtual void CameraSetTranslation() = 0;
			virtual void CameraProcessMouseInput(const Handlers::MouseMoveEvent& a_evn){};
			virtual void CameraUpdate(NiCamera* a_camera);

		protected:
			void CameraComputeRotation(float a_step);

			TransformData m_xfrm;
			Settings      m_settings;
		};
	}
}