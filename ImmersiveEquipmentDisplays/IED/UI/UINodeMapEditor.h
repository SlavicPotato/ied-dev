#pragma once

#include "UICommon.h"

#include "Widgets/UITableRowInputWidget.h"

#include "IED/UI/Window/UIWindow.h"

#include "UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINodeMapEditor :
			public UIWindow,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID = "IED_nodemap";
		public:
			UINodeMapEditor(Controller& a_controller);

			void Draw();

		private:
			void DrawMenuBar();
			void DrawNodeTable();

			void QueueNewNodePopup();
			void QueueDeleteNodePopup(const stl::fixed_string& a_node);

			void OnChange();

			UITableRowInputWidget<stl::fixed_string> m_tableRowInput;

			Controller& m_controller;
		};
	}
}
