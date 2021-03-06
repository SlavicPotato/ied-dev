#pragma once

#include "IED/UI/UICommon.h"

#include "IED/UI/Widgets/Form/UIFormInfoTooltipWidget.h"
#include "IED/UI/Widgets/Lists/UIActorListWidget.h"
#include "IED/UI/Widgets/UIProfileSelectorWidget.h"

#include "UICustomEditorCommon.h"

#include "IED/Controller/ObjectManagerData.h"
#include "IED/GlobalProfileManager.h"

namespace IED
{
	class Controller;
	struct objectEntryCustom_t;

	namespace UI
	{
		class UICustomEditorActor :
			public UICustomEditorCommon<Game::FormID>,
			UIActorList<entryCustomData_t>,
			UIFormInfoTooltipWidget
		{
		public:
			UICustomEditorActor(Controller& a_controller);

			virtual void Initialize() override;

			virtual void Draw() override;
			virtual void OnOpen() override;
			virtual void OnClose() override;
			virtual void Reset() override;
			virtual void QueueUpdateCurrent() override;

		private:
			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const override;

			virtual Data::configCustomHolder_t& GetOrCreateConfigSlotHolder(
				Game::FormID a_handle) const override;

			virtual entryCustomData_t GetData(
				Game::FormID a_formid) override;

			virtual CustomEditorCurrentData GetCurrentData() override;

			virtual UIPopupQueue& GetPopupQueue() override;
			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void                         OnCollapsibleStatesUpdate();

			virtual void OnListOptionsChange() override;

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
			virtual void                                    OnEditorPanelSettingsChange() override;

			virtual void ListResetAllValues(
				Game::FormID a_handle) override;

			virtual void OnListChangeCurrentItem(
				const stl::optional<UIActorList<entryCustomData_t>::listValue_t>& a_oldHandle,
				const stl::optional<UIActorList<entryCustomData_t>::listValue_t>& a_newHandle) override;

			virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

			virtual void ApplyProfile(
				const profileSelectorParamsCustom_t<Game::FormID>& a_data,
				const CustomProfile&                               a_profile) override;

			virtual void MergeProfile(
				const profileSelectorParamsCustom_t<Game::FormID>& a_data,
				const CustomProfile&                               a_profile) override;

			virtual void OnBaseConfigChange(
				Game::FormID     a_handle,
				const void*      a_params,
				PostChangeAction a_action) override;

			virtual void OnFullConfigChange(
				Game::FormID                    a_handle,
				const CustomConfigUpdateParams& a_params) override;

			virtual bool OnCreateNew(
				Game::FormID                 a_handle,
				const CustomConfigNewParams& a_params) override;

			virtual void OnErase(
				Game::FormID                   a_handle,
				const CustomConfigEraseParams& a_params) override;

			virtual bool OnRename(
				Game::FormID                    a_handle,
				const CustomConfigRenameParams& a_params) override;

			virtual void DrawMenuBarItemsExtra() override;

			virtual bool DrawExtraItemInfo(
				Game::FormID                     a_handle,
				const stl::fixed_string&         a_name,
				const Data::configCustomEntry_t& a_entry,
				bool                             a_infoDrawn) override;

			const objectEntryCustom_t* GetLoadedObject(
				Game::FormID                     a_handle,
				const stl::fixed_string&         a_name,
				const Data::configCustomEntry_t& a_entry);

			virtual const ImVec4* HighlightEntry(Game::FormID a_handle) override;

			Controller& m_controller;
		};

	}
}