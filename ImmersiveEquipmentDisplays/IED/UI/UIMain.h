#pragma once

#include "UIDialogImportExport.h"
#include "UIFormBrowser.h"
#include "UIFormInfoCache.h"
#include "UILog.h"
#include "UIMainCommon.h"
#include "UIMainStrings.h"
#include "UINodeMapEditor.h"
#include "UISettings.h"
#include "UIStats.h"

#include "EquipmentSlots/Profile/UIProfileEditorSlot.h"
#include "EquipmentSlots/UISlotTabPanel.h"

#include "Custom/Profile/UIProfileEditorCustom.h"
#include "Custom/UICustomTabPanel.h"

#include "NodeOverride/Profile/UIProfileEditorNodeOverride.h"
#include "NodeOverride/UINodeOverrideEditorWindow.h"

#include "FormFilters/UIProfileEditorFormFilters.h"

#include "Widgets/UIExportFilterWidget.h"

#include "IED/ConfigOverrideDefault.h"
#include "IED/SettingHolder.h"

#include "Window/UIWindow.h"

#include "UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIMain :
			public UIWindow,
			UIExportFilterWidget,
			public virtual UILocalizationInterface
		{
			static constexpr auto TITLE_NAME = "IED";
			static constexpr auto WINDOW_ID = "IED_main";

		public:
			UIMain(Controller& a_controller);

			void Initialize();
			void Reset();
			void Draw();

			void OnOpen();
			void OnClose();

			inline constexpr auto& GetPopupQueue() noexcept
			{
				return m_popupQueue;
			}

			inline constexpr auto& GetFormBrowser() noexcept
			{
				return m_formBrowser;
			}

			inline constexpr auto& GetFormLookupCache() noexcept
			{
				return m_formLookupCache;
			}

			const Data::SettingHolder::UserInterface& GetUISettings() noexcept;

		private:
			void DrawMenuBar();
			void DrawFileMenu();
			void DrawViewMenu();
			void DrawToolsMenu();
			void DrawActionsMenu();

			void OpenEditorPanel(UIEditorPanel a_panel);

			void SetTitle(Localization::StringID a_strid);

			UIFormBrowser m_formBrowser;
			UISettings m_settings;
			UIDialogImportExport m_importExport;
			UIProfileEditorSlot m_slotProfileEditor;
			UIProfileEditorCustom m_customProfileEditor;
			UIProfileEditorNodeOverride m_nodeOverrideProfileEditor;
			UINodeMapEditor m_nodeMapEditor;
			UINodeOverrideEditorWindow m_nodeOverrideEditor;
			UIProfileEditorFormFilters m_formFiltersProfileEditor;
			UILog m_log;
			UIStats m_stats;

			UISlotEditorTabPanel m_slotTabPanel;
			UICustomEditorTabPanel m_customTabPanel;

			UIEditorPanel m_currentEditorPanel{ UIEditorPanel::Slot };

			UIFormInfoCache m_formLookupCache;

			UIPopupQueue m_popupQueue;

			char m_currentTitle[128]{ 0 };

			Controller& m_controller;
		};

	}
}