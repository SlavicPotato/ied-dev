#pragma once

#include "UINodeOverrideEditorCommon.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINodeOverrideEditorGlobal :
			public UINodeOverrideEditorCommon<int>
		{
		public:
			UINodeOverrideEditorGlobal(Controller& a_controller);

			virtual void Initialize() override;

			virtual void Draw() override;
			virtual void OnOpen() override;
			virtual void OnClose() override;
			virtual void Reset() override;

		private:
			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual NodeOverrideEditorCurrentData GetCurrentData() override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void OnCollapsibleStatesUpdate();

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
			virtual void OnEditorPanelSettingsChange() override;

			virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

			virtual void ApplyProfile(
				profileSelectorParamsNodeOverride_t<int>& a_data,
				const NodeOverrideProfile& a_profile) override;

			virtual void OnUpdate(
				int a_handle,
				const SingleNodeOverrideUpdateParams& a_params) override;

			virtual void OnUpdate(
				int a_handle,
				const SingleNodeOverrideParentUpdateParams& a_params) override;

			virtual void OnUpdate(
				int a_handle,
				const NodeOverrideUpdateParams& a_params) override;

			virtual void OnClear(
				int a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearParent(
				int a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual Data::configNodeOverrideHolder_t& GetOrCreateConfigHolder(
				int) const override;

			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			void UpdateData();

			entryNodeOverrideData_t m_data;

			Controller& m_controller;
		};
	}
}