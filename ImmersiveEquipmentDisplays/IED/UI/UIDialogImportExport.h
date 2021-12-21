#pragma once

#include "UICommon.h"

#include "UITips.h"
#include "Widgets/UIExportFilterWidget.h"
#include "Widgets/UIFileSelectorWidget.h"

#include "IED/UI/Window/UIWindow.h"

#include "IED/Controller/ImportFlags.h"

#include "IED/ConfigOverride.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIDialogImportExport :
			public UIWindow,
			public UIFileSelector,
			UITipsInterface,
			UIExportFilterWidget
		{
			static constexpr auto WINDOW_ID = "ied_ie";

		public:
			UIDialogImportExport(Controller& a_controller);

			void Draw();
			void OnOpen();

		private:
			void DrawExportContextMenu();

			void DoImport(Data::configStore_t&& a_data, stl::flag<ImportFlags> a_flags);
			void DoExport(const fs::path& a_path);

			bool DoUpdate(bool a_select);

			bool CheckFileName(const std::string& a_path) const;

			std::regex m_rFileCheck;

			Controller& m_controller;
		};
	}  // namespace UI
}  // namespace IED