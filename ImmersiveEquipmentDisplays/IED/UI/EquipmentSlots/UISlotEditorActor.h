#pragma once

#include "IED/UI/UICommon.h"

#include "IED/UI/Widgets/Form/UIFormInfoTooltipWidget.h"
#include "IED/UI/Widgets/Lists/UIActorListWidget.h"
#include "IED/UI/Widgets/UIProfileSelectorWidget.h"

#include "UISlotEditorCommon.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UISlotEditorActor :
			public UISlotEditorCommon<Game::FormID>,
			UIActorList<entrySlotData_t>,
			UIFormInfoTooltipWidget
		{
		public:
			UISlotEditorActor(Controller& a_controller);
			~UISlotEditorActor() noexcept override;

			virtual void EditorInitialize() override;
			virtual void EditorDraw() override;

			virtual void EditorOnOpen() override;
			virtual void EditorOnClose() override;
			virtual void EditorReset() override;
			virtual void EditorQueueUpdateCurrent() override;

		private:
			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const override;

			virtual SlotEditorCurrentData GetCurrentData() override;

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
			virtual void                                    OnEditorPanelSettingsChange() override;

			virtual void ListResetAllValues(
				Game::FormID a_handle) override;

			virtual entrySlotData_t GetData(
				Game::FormID a_formid) override;

			Data::configSlotHolder_t&
				GetOrCreateConfigSlotHolder(
					Game::FormID a_handle) const override;

			virtual void MergeProfile(
				const profileSelectorParamsSlot_t<Game::FormID>& a_data,
				const SlotProfile&                               a_profile) override;

			virtual void ApplyProfile(
				const profileSelectorParamsSlot_t<Game::FormID>& a_data,
				const SlotProfile&                               a_profile) override;

			virtual void OnBaseConfigChange(
				Game::FormID     a_handle,
				const void*      a_params,
				PostChangeAction a_action) override;

			virtual void OnFullConfigChange(
				Game::FormID                  a_handle,
				const SlotConfigUpdateParams& a_params) override;

			virtual void OnPriorityConfigChange(
				Game::FormID                          a_handle,
				const SlotPriorityConfigUpdateParams& a_params) override;

			virtual void OnPriorityConfigClear(
				Game::FormID                  a_handle,
				const SlotConfigUpdateParams& a_params) override;

			virtual void OnSingleSlotClear(
				Game::FormID                       a_handle,
				const SingleSlotConfigClearParams& a_params) override;

			virtual void OnFullConfigClear(
				Game::FormID                     a_handle,
				const FullSlotConfigClearParams& a_params) override;

			virtual void OnListChangeCurrentItem(
				const std::optional<UIActorList<entrySlotData_t>::listValue_t>& a_oldHandle,
				const std::optional<UIActorList<entrySlotData_t>::listValue_t>& a_newHandle) override;

			virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

			virtual void OnListOptionsChange() override;

			virtual UIPopupQueue& GetPopupQueue() override;
			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData() override;
			virtual void                         OnCollapsibleStatesUpdate() override;

			virtual void DrawMenuBarItemsExtra() override;

			virtual void ListDrawExtraActorInfo(
				const listValue_t& a_entry) override;

			virtual bool DrawExtraSlotInfo(
				Game::FormID                      a_handle,
				Data::ObjectSlot                  a_slot,
				const entrySlotData_t::data_type& a_entry,
				bool                              a_infoDrawn) override;

			virtual const ImVec4* HighlightEntry(Game::FormID a_handle) override;

			virtual entrySlotData_t GetCurrentData(
				Game::FormID a_handle) override;

			virtual void OnConditionListDrawn(
				const Data::equipmentOverrideConditionSet_t& a_condition) override;

			Controller& m_controller;
		};

	}
}