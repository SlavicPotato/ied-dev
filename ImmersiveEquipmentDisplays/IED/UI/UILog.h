#pragma once

#include <ext/ILogging.h>

#include "Window/UIWindow.h"
#include "Widgets/Filters/UIGenericFilter.h"

#include "UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UILog :
			public UIWindow,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID = "IED_log";

		public:
			UILog(Controller& a_controller);

			void Draw();
			void Initialize();

		private:

			void DrawHeader();
			void DrawLogText();
			void DrawLevelCheckbox(const char *a_label, LogLevel a_level);

			void AutoScroll();

			UIGenericFilter m_filter;
			std::int8_t m_initialScroll{ 2 };
			bool m_doScrollBottom{ false };

			std::size_t m_lastSize{ 0 };

			Controller& m_controller;
		};

	} 
}