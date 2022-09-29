#pragma once

#include "UINodeOverrideTabPanel.h"

#include "IED/UI/Window/UIWindow.h"
#include "IED/UI/UIContext.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;
	class UIProfileEditorNodeOverride;

	namespace UI
	{
		class UINodeOverrideEditorWindow :
			public UIWindow,
			public UIContext,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID = "ied_noe";

		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUINodeOverrideEditorWindow;

			UINodeOverrideEditorWindow(
				Controller&   a_controller,
				UIContext& a_profileEditor);

			void Initialize() override;
			void Reset() override;
			void Draw() override;
			void OnOpen() override;
			void OnClose() override;

		private:
			void DrawMenuBar();
			void DrawFileMenu();
			void DrawToolsMenu();

			UINodeOverrideTabPanel m_tabPanel;

			UIContext& m_profileEditor;

			Controller& m_controller;
		};

	}
}
