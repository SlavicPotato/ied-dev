#pragma once

#include "../UICommon.h"
#include "UISlotEditorCommon.h"

#include "IED/Data.h"

#include "../Widgets/UIProfileSelectorWidget.h"
#include "Widgets/UISlotEditorWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UISlotEditorGlobal :
			// public UISlotEditorWidget<int>,
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

			virtual Data::SettingHolder::EditorPanelCommon &GetEditorPanelSettings() override;

			virtual void OnEditorPanelSettingsChange() override;

			virtual void MergeProfile(
				profileSelectorParamsSlot_t<int>& a_data,
				const SlotProfile& a_profile) override;

			virtual void OnBaseConfigChange(
				int a_handle,
				const void* a_params,
				PostChangeAction a_action) override;

			virtual void
				OnFullConfigChange(
					int a_handle,
					const SlotConfigUpdateParams& a_params) override;

			virtual void OnSingleSlotClear(
				int a_handle,
				const void* a_params) override;

			virtual void OnFullConfigClear(
				int a_handle) override;

			virtual void OnSexChanged(
				Data::ConfigSex a_newSex) override;

			virtual UIPopupQueue& GetPopupQueue() override;
			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData() override;
			virtual void OnCollapsibleStatesUpdate();

			void DrawTypeSelectorRadio();
			void UpdateData();

			Data::configStoreSlot_t::result_copy m_data;

			Controller& m_controller;
		};
	}  // namespace UI
}  // namespace IED