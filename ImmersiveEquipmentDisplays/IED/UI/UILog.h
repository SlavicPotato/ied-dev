#pragma once

#include <ext/ILogging.h>

#include "Widgets/Filters/UIGenericFilter.h"

#include "Window/UIWindow.h"
#include "UIContext.h"

#include "UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UILog :
			public UIWindow,
			public UIContext,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID = "ied_log";

		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUILog;

			UILog(Controller& a_controller);

			void Initialize() override;
			void Draw() override;

		private:
			void DrawHeader();
			void DrawLogText();
#if defined(SKMP_TIMESTAMP_LOGS)
			static void DrawTimeStampLine(const BackLog::Entry& a_entry);
#endif
			void DrawLevelCheckbox(const char* a_label, LogLevel a_level);

			void AutoScroll();

			UIGenericFilter m_filter;
			std::int8_t     m_initialScroll{ 2 };
			bool            m_doScrollBottom{ false };

			std::size_t m_lastSize{ 0 };

			Controller& m_controller;
		};

	}
}