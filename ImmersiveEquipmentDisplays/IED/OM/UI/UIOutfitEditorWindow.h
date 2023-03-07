#pragma once

#include "UIOutfitEditorTabPanel.h"

#include "IED/UI/UIContext.h"
#include "IED/UI/Window/UIWindow.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		namespace OM
		{
			class UIOutfitEditorWindow :
				public UIContext,
				public UIWindow
			{
				static constexpr auto WINDOW_ID = "ied_otft";

			public:
				static constexpr auto CHILD_ID = ChildWindowID::kOutfitEditor;

				UIOutfitEditorWindow(
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

				UIOutfitEditorTabPanel m_tabPanel;

				Controller& m_controller;
			};
		}
	}
}