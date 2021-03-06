#pragma once

#include "UIAboutModal.h"
#include "UIContextBase.h"
#include "UIDialogImportExport.h"
#include "UIFormBrowser.h"
#include "UIFormInfoCache.h"
#include "UILocalizationInterface.h"
#include "UILog.h"
#include "UIMainCommon.h"
#include "UIMainStrings.h"
#include "UINodeMapEditor.h"
#include "UISettings.h"
#include "UISkeletonExplorer.h"
#include "UIStats.h"

#include "EquipmentSlots/Profile/UIProfileEditorSlot.h"
#include "EquipmentSlots/UISlotTabPanel.h"

#include "Custom/Profile/UIProfileEditorCustom.h"
#include "Custom/UICustomTabPanel.h"

#include "NodeOverride/Profile/UIProfileEditorNodeOverride.h"
#include "NodeOverride/UINodeOverrideEditorWindow.h"

#include "FormFilters/UIProfileEditorFormFilters.h"

#include "Window/UIWindow.h"

#include "Widgets/UIExportFilterWidget.h"

#include "IED/ConfigSerializationFlags.h"
#include "IED/SettingHolder.h"

#if defined(IED_ENABLE_I3DI)
#	include "I3DI/I3DIMain.h"
#endif

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIMain :
			public UIContextBase,
			public UIWindow,
			UIExportFilterWidget,
			UIAboutModal,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto TITLE_NAME = PLUGIN_NAME_FULL;
			inline static constexpr auto WINDOW_ID  = "ied_main";

		public:
			UIMain(Controller& a_controller);

			virtual ~UIMain() noexcept = default;

			virtual void Initialize() override;
			virtual void Reset() override;
			virtual void Draw() override;
			virtual void PrepareGameData() override;
			virtual void Render() override;

			virtual void OnOpen() override;
			virtual void OnClose() override;

			virtual void OnMouseMove(const Handlers::MouseMoveEvent& a_evn) override;

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
			void DrawHelpMenu();

			void DrawDefaultConfigSubmenu();

			void OpenEditorPanel(UIEditorPanel a_panel);

			void SetTitle(Localization::StringID a_strid);

			UIFormBrowser               m_formBrowser;
			UISettings                  m_settings;
			UIDialogImportExport        m_importExport;
			UIProfileEditorSlot         m_slotProfileEditor;
			UIProfileEditorCustom       m_customProfileEditor;
			UIProfileEditorNodeOverride m_nodeOverrideProfileEditor;
			UINodeMapEditor             m_nodeMapEditor;
			UINodeOverrideEditorWindow  m_nodeOverrideEditor;
			UIProfileEditorFormFilters  m_formFiltersProfileEditor;
			UILog                       m_log;
			UIStats                     m_stats;
			UISkeletonExplorer          m_skeletonExplorer;

#if defined(IED_ENABLE_I3DI)
			I3DIMain m_i3di;
#endif

			UISlotEditorTabPanel   m_slotTabPanel;
			UICustomEditorTabPanel m_customTabPanel;

			UIEditorPanel m_currentEditorPanel{ UIEditorPanel::Slot };

			UIFormInfoCache m_formLookupCache;

			UIPopupQueue m_popupQueue;

			char m_currentTitle[128]{ 0 };

			Controller& m_controller;
		};

	}
}