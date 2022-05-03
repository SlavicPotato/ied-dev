#pragma once

#include "IED/UI/UICommon.h"
#include "UISlotEditorCommon.h"

#include "IED/Data.h"

#include "IED/UI/Widgets/UIGlobalConfigTypeSelectorWidget.h"
#include "Widgets/UISlotEditorWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UISlotEditorGlobal :
			UIGlobalConfigTypeSelectorWidget,
			public UISlotEditorCommon<int>
		{
		public:
			UISlotEditorGlobal(Controller& a_controller);

			virtual void Initialize() override;
			virtual void Draw() override;
			virtual void OnOpen() override;
			virtual void OnClose() override;
			virtual void Reset() override;
			virtual void QueueUpdateCurrent() override;

		private:
			Data::configSlotHolder_t& GetOrCreateConfigSlotHolder(int) const override;

			virtual bool ShowConfigClassIndicator() const override;
			virtual bool PermitDeletion() const override;

			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual SlotEditorCurrentData GetCurrentData() override;

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;

			virtual void OnEditorPanelSettingsChange() override;

			virtual void MergeProfile(
				const profileSelectorParamsSlot_t<int>& a_data,
				const SlotProfile&                      a_profile) override;

			/*virtual void ApplyProfile(
				const profileSelectorParamsSlot_t<int>& a_data,
				const SlotProfile& a_profile) override;*/

			virtual void OnBaseConfigChange(
				int              a_handle,
				const void*      a_params,
				PostChangeAction a_action) override;

			virtual void
				OnFullConfigChange(
					int                           a_handle,
					const SlotConfigUpdateParams& a_params) override;

			virtual void OnPriorityConfigChange(
				int                                   a_handle,
				const SlotPriorityConfigUpdateParams& a_params) override;

			virtual void OnPriorityConfigClear(
				int                           a_handle,
				const SlotConfigUpdateParams& a_params) override;

			virtual void OnSingleSlotClear(
				int                                a_handle,
				const SingleSlotConfigClearParams& a_params) override;

			virtual void OnFullConfigClear(
				int                              a_handle,
				const FullSlotConfigClearParams& a_params) override;

			virtual void OnSexChanged(
				Data::ConfigSex a_newSex) override;

			virtual UIPopupQueue& GetPopupQueue() override;
			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData() override;
			virtual void                         OnCollapsibleStatesUpdate() override;

			void UpdateData(entrySlotData_t& a_data);

			virtual entrySlotData_t GetCurrentData(
				int a_handle) override;

			entrySlotData_t m_data;

			Controller& m_controller;
		};
	}
}