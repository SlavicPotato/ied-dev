#pragma once

#include "IED/UI/UICommon.h"
#include "UISlotEditorCommon.h"

#include "IED/Data.h"

#include "IED/UI/Widgets/UIGlobalConfigTypeSelectorWidget.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "Widgets/UISlotEditorWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UISlotEditorGlobal :
			public UISlotEditorCommon<UIGlobalEditorDummyHandle>,
			UIGlobalConfigTypeSelectorWidget
		{
		public:
			UISlotEditorGlobal(Controller& a_controller);

			virtual void EditorInitialize() override;
			virtual void EditorDraw() override;
			virtual void EditorOnOpen() override;
			virtual void EditorOnClose() override;
			virtual void EditorReset() override;
			virtual void EditorQueueUpdateCurrent() override;

		private:
			Data::configSlotHolder_t& GetOrCreateConfigSlotHolder(UIGlobalEditorDummyHandle) const override;

			virtual bool ShowConfigClassIndicator() const override;
			virtual bool PermitDeletion() const override;

			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual SlotEditorCurrentData GetCurrentData() override;

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;

			virtual void OnEditorPanelSettingsChange() override;

			virtual void MergeProfile(
				const profileSelectorParamsSlot_t<UIGlobalEditorDummyHandle>& a_data,
				const SlotProfile&                                            a_profile) override;

			/*virtual void ApplyProfile(
				const profileSelectorParamsSlot_t<int>& a_data,
				const SlotProfile& a_profile) override;*/

			virtual void OnBaseConfigChange(
				UIGlobalEditorDummyHandle a_handle,
				const void*               a_params,
				PostChangeAction          a_action) override;

			virtual void
				OnFullConfigChange(
					UIGlobalEditorDummyHandle     a_handle,
					const SlotConfigUpdateParams& a_params) override;

			virtual void OnPriorityConfigChange(
				UIGlobalEditorDummyHandle             a_handle,
				const SlotPriorityConfigUpdateParams& a_params) override;

			virtual void OnPriorityConfigClear(
				UIGlobalEditorDummyHandle     a_handle,
				const SlotConfigUpdateParams& a_params) override;

			virtual void OnSingleSlotClear(
				UIGlobalEditorDummyHandle          a_handle,
				const SingleSlotConfigClearParams& a_params) override;

			virtual void OnFullConfigClear(
				UIGlobalEditorDummyHandle        a_handle,
				const FullSlotConfigClearParams& a_params) override;

			virtual void OnSexChanged(
				Data::ConfigSex a_newSex) override;

			virtual UIPopupQueue& GetPopupQueue() override;
			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData() override;
			virtual void                         OnCollapsibleStatesUpdate() override;

			void UpdateData(entrySlotData_t& a_data);

			virtual entrySlotData_t GetCurrentData(
				UIGlobalEditorDummyHandle a_handle) override;

			entrySlotData_t m_data;

			Controller& m_controller;
		};
	}
}