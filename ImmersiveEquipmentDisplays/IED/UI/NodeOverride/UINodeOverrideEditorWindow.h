#pragma once

#include "UINodeOverrideTabPanel.h"

#include "IED/UI/Window/UIWindow.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;
	class UIProfileEditorNodeOverride;

	namespace UI
	{
		class UINodeOverrideEditorWindow :
			public UIWindow,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID = "ied_noe";

		public:
			UINodeOverrideEditorWindow(
				Controller&   a_controller,
				UIWindowBase& a_profileEditor);

			void Initialize();
			void Reset();
			void Draw();
			void OnOpen();
			void OnClose();

		private:
			void DrawMenuBar();
			void DrawFileMenu();
			void DrawToolsMenu();

			UINodeOverrideTabPanel m_tabPanel;

			UIWindowBase& m_profileEditor;

			Controller& m_controller;
		};

	}
}
