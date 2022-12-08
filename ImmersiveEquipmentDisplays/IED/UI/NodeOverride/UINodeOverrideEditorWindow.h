#pragma once

#include "UINodeOverrideTabPanel.h"

#include "IED/UI/UIContext.h"
#include "IED/UI/Window/UIWindow.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;
	class UIProfileEditorNodeOverride;

	namespace UI
	{
		class UINodeOverrideEditorWindow :
			public UIContext,
			public UIWindow
		{
			inline static constexpr auto WINDOW_ID = "ied_noe";

		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUINodeOverrideEditorWindow;

			UINodeOverrideEditorWindow(
				Controller& a_controller);

			void Initialize() override;
			void Reset() override;
			void Draw() override;
			void OnOpen() override;
			void OnClose() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			void DrawMenuBar();
			void DrawFileMenu();
			void DrawToolsMenu();

			UINodeOverrideTabPanel m_tabPanel;

			Controller& m_controller;
		};

	}
}
