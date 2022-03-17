#pragma once

#include "UIContextBase.h"
#include "UILocalizationInterface.h"

#include "UICommon.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIIntroBanner :
			public UIContextBase,
			UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID = "ied_intro";

		public:
			UIIntroBanner(Controller& a_controller, float a_voffset);

			virtual ~UIIntroBanner() noexcept = default;

			virtual void Draw() override;

			float m_voffset;

			UICommon::float_anim_t<float, 0.0f, 1.0f, 1.75f> m_animbg;
		};
	}
}