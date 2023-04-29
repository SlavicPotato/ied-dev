#pragma once

#include "UIContext.h"
#include "UILocalizationInterface.h"

#include "Window/UIWindow.h"

#include "UICommon.h"

namespace IED
{
	class Controller;

	namespace Tasks
	{
		class UIRenderTaskBase;
	};

	namespace UI
	{
		class UIIntroBanner :
			public UIContext,
			public UIWindow
		{
			static constexpr auto WINDOW_ID = "ied_intro";

		public:
			UIIntroBanner(
				Tasks::UIRenderTaskBase& a_owner,
				Controller&              a_controller,
				float                    a_voffset);

			virtual ~UIIntroBanner() noexcept = default;

			virtual void Draw() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(-1);
			}

			float m_voffset;

			UICommon::float_anim_stepper<float, 0.0f, 1.0f, 1.75f> m_animbg;

			Tasks::UIRenderTaskBase& m_owner;
			Controller&              m_controller;
		};
	}
}