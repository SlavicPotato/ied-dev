#pragma once

#include "UICommon.h"

#include "Widgets/UITableRowInputWidget.h"

#include "IED/UI/Window/UIWindow.h"
#include "IED/UI/Window/UIWindowBase.h"

#include "UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINodeMapEditor :
			public UIWindow,
			public UIChildWindowBase,
			public UITableRowInputWidget<stl::fixed_string>,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID = "ied_nodemap";

		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUINodeMapEditor;

			UINodeMapEditor(Controller& a_controller);

			void Draw() override;

		private:
			void DrawMenuBar();
			void DrawNodeTable();

			void QueueNewNodePopup();
			void QueueDeleteNodePopup(const stl::fixed_string& a_node);

			void OnChange();

			Controller& m_controller;
		};
	}
}
