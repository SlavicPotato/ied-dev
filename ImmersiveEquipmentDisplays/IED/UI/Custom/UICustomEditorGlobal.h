#pragma once

#include "../UICommon.h"
#include "../Widgets/UIProfileSelectorWidget.h"

#include "UICustomEditorCommon.h"

#include "IED/GlobalProfileManager.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UICustomEditorGlobal :
			public UICustomEditorCommon<int>
		{
		public:
			UICustomEditorGlobal(Controller& a_controller);

			virtual void Initialize() override;

			virtual void Draw() override;
			virtual void OnOpen() override;
			virtual void OnClose() override;
			virtual void Reset() override;
			virtual void QueueUpdateCurrent() override;

		private:
			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual Data::configCustomHolder_t& GetOrCreateConfigSlotHolder(
				int) const override;

			virtual CustomEditorCurrentData GetCurrentData() override;

			virtual UIPopupQueue& GetPopupQueue() override;
			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void OnCollapsibleStatesUpdate();

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
			virtual void OnEditorPanelSettingsChange() override;

			virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

			virtual void ApplyProfile(
				profileSelectorParamsCustom_t<int>& a_data,
				const CustomProfile& a_profile) override;
			
			virtual void MergeProfile(
				profileSelectorParamsCustom_t<int>& a_data,
				const CustomProfile& a_profile) override;

			virtual void OnBaseConfigChange(
				int a_handle,
				const void* a_params,
				PostChangeAction a_action) override;

			virtual void OnFullConfigChange(
				int a_handle,
				const CustomConfigUpdateParams& a_params) override;

			virtual bool OnCreateNew(
				int a_handle,
				const CustomConfigNewParams& a_params) override;

			virtual void OnErase(
				int a_handle,
				const CustomConfigEraseParams& a_params) override;

			virtual bool OnRename(
				int a_handle,
				const CustomConfigRenameParams& a_params) override;

			void UpdateData();

			entryCustomData_t m_data;
			Controller& m_controller;
		};
	}
}