#pragma once

#include "IED/UI/UIContext.h"
#include "IED/UI/Window/UIWindow.h"

#include "IED/UI/Widgets/KeyBind/UIKeyBindEditorWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIKeyBindEditorWindow :
			public UIContext,
			public UIWindow,
			UIKeyBindEditorWidget
		{
			static constexpr auto WINDOW_ID = "ied_kbe";

		public:
			static constexpr auto CHILD_ID = ChildWindowID::kKeyBindEditor;

			UIKeyBindEditorWindow(
				Controller& a_controller);

			void Draw() override;
			void OnClose() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			void DrawEditor();
			bool DrawEditorImpl();

			void DrawMenuBar();
			void DrawFileMenu();
			void DrawActionMenu();
			void DrawAddPopup();

			std::string m_tmpID;

			Controller& m_controller;
		};

	}
}
