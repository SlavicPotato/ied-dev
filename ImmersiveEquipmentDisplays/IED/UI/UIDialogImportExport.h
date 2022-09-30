#pragma once

#include "UICommon.h"

#include "UITips.h"
#include "Widgets/UIExportFilterWidget.h"
#include "Widgets/UIFileSelectorWidget.h"

#include "IED/UI/Window/UIWindow.h"
#include "IED/UI/UIContext.h"

#include "IED/Controller/ImportFlags.h"

#include "IED/ConfigStore.h"

#include "Widgets/UIImportWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIDialogImportExport :
			public UIWindow,
			public UIContext,
			public UIFileSelector,
			public UIImportWidget,
			public virtual UITipsInterface
		{
			static constexpr auto WINDOW_ID = "ied_ie";

		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUIDialogImportExport;

			UIDialogImportExport(Controller& a_controller);

			void Draw() override;
			void OnOpen() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			virtual void OnDataImport(bool a_success) override;

			void DrawExportContextMenu();

			void DoExport(const fs::path& a_path);

			bool DoUpdate();

			Controller& m_controller;
		};
	}
}