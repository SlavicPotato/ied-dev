#pragma once

#include "UICommon.h"

#include "UITips.h"
#include "Widgets/UIExportFilterWidget.h"
#include "Widgets/UIFileSelectorWidget.h"

#include "IED/UI/Window/UIWindow.h"

#include "IED/Controller/ImportFlags.h"

#include "IED/ConfigOverride.h"

#include "Widgets/UIImportWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIDialogImportExport :
			public UIWindow,
			public UIFileSelector,
			public UIImportWidget,
			public virtual UITipsInterface
		{
			static constexpr auto WINDOW_ID = "ied_ie";

		public:
			UIDialogImportExport(Controller& a_controller);

			void Draw();
			void OnOpen();

		private:
			virtual void OnDataImport(bool a_success) override;

			void DrawExportContextMenu();

			void DoExport(const fs::path& a_path);

			bool DoUpdate();

			Controller& m_controller;
		};
	}
}