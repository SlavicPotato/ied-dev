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
		class I3DIMain;

		class I3DIObjectCamera :
			public I3DICamera
		{
			friend class I3DIMain;

			struct ObjectCamSettings
			{
			};

			struct ObjectCamData
			{
				
			};

		public:
			I3DIObjectCamera(NiCamera* a_camera) noexcept;
			virtual ~I3DIObjectCamera() noexcept override = default;

			virtual void CameraSetTranslation() override;
			virtual void CameraProcessMouseInput(const Handlers::MouseMoveEvent& a_evn) override;

			void CameraUpdate(NiCamera* a_camera) override;

		private:
			ObjectCamSettings m_fSettings;
			ObjectCamData     m_data;

		};
	}
}