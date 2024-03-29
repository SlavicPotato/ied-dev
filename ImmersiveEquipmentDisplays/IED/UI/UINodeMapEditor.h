#pragma once

#include "UICommon.h"

#include "Widgets/UITableRowInputWidget.h"

#include "IED/UI/Window/UIWindow.h"
#include "IED/UI/UIContext.h"

#include "UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINodeMapEditor :
			public UIContext,
			public UIWindow,
			public UITableRowInputWidget<stl::fixed_string>
		{
			static constexpr auto WINDOW_ID = "ied_nodemap";

		public:
			static constexpr auto CHILD_ID = ChildWindowID::kUINodeMapEditor;

			UINodeMapEditor(Controller& a_controller);

			void Draw() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

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
