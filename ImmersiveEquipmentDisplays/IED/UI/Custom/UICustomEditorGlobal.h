#pragma once

#include "IED/UI/UICommon.h"
#include "IED/UI/Widgets/UIProfileSelectorWidget.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "UICustomEditorCommon.h"

#include "IED/GlobalProfileManager.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UICustomEditorGlobal :
			public UICustomEditorCommon<UIGlobalEditorDummyHandle>
		{
		public:
			UICustomEditorGlobal(Controller& a_controller);

			virtual void EditorInitialize() override;

			virtual void EditorDraw() override;
			virtual void EditorOnOpen() override;
			virtual void EditorOnClose() override;
			virtual void EditorReset() override;
			virtual void EditorQueueUpdateCurrent() override;

		private:
			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual Data::configCustomHolder_t& GetOrCreateConfigSlotHolder(
				UIGlobalEditorDummyHandle) const override;

			virtual CustomEditorCurrentData GetCurrentData() override;

			virtual UIPopupQueue& GetPopupQueue() override;
			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void                         OnCollapsibleStatesUpdate();

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
			virtual void                                    OnEditorPanelSettingsChange() override;

			virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

			virtual void ApplyProfile(
				const profileSelectorParamsCustom_t<UIGlobalEditorDummyHandle>& a_data,
				const CustomProfile&                                                a_profile) override;

			virtual void MergeProfile(
				const profileSelectorParamsCustom_t<UIGlobalEditorDummyHandle>& a_data,
				const CustomProfile&                                                a_profile) override;

			virtual void OnBaseConfigChange(
				UIGlobalEditorDummyHandle a_handle,
				const void*                   a_params,
				PostChangeAction              a_action) override;

			virtual void OnFullConfigChange(
				UIGlobalEditorDummyHandle   a_handle,
				const CustomConfigUpdateParams& a_params) override;

			virtual bool OnCreateNew(
				UIGlobalEditorDummyHandle a_handle,
				const CustomConfigNewParams&  a_params) override;

			virtual void OnErase(
				UIGlobalEditorDummyHandle  a_handle,
				const CustomConfigEraseParams& a_params) override;

			virtual bool OnRename(
				UIGlobalEditorDummyHandle   a_handle,
				const CustomConfigRenameParams& a_params) override;

			void UpdateData();

			entryCustomData_t m_data;
			Controller&       m_controller;
		};
	}
}