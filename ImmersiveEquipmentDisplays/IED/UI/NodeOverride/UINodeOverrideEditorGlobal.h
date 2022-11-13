#pragma once

#include "UINodeOverrideEditorCommon.h"

#include "IED/UI/Widgets/UIGlobalConfigTypeSelectorWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINodeOverrideEditorGlobal :
			UIGlobalConfigTypeSelectorWidget,
			public UINodeOverrideEditorCommon<int>
		{
		public:
			UINodeOverrideEditorGlobal(Controller& a_controller);

			virtual void EditorInitialize() override;

			virtual void EditorDraw() override;
			virtual void EditorOnOpen() override;
			virtual void EditorOnClose() override;
			virtual void EditorReset() override;

		private:
			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual NodeOverrideEditorCurrentData GetCurrentData() override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void                         OnCollapsibleStatesUpdate();

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
			virtual void                                    OnEditorPanelSettingsChange() override;

			virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

			virtual void ApplyProfile(
				const profileSelectorParamsNodeOverride_t<int>& a_data,
				const NodeOverrideProfile&                      a_profile) override;

			virtual void MergeProfile(
				const profileSelectorParamsNodeOverride_t<int>& a_data,
				const NodeOverrideProfile&                      a_profile) override;

			virtual void OnUpdate(
				int                                            a_handle,
				const SingleNodeOverrideTransformUpdateParams& a_params) override;

			virtual void OnUpdate(
				int                                            a_handle,
				const SingleNodeOverridePlacementUpdateParams& a_params) override;
			
			virtual void OnUpdate(
				int                                            a_handle,
				const SingleNodeOverridePhysicsUpdateParams& a_params) override;

			virtual void OnUpdate(
				int                             a_handle,
				const NodeOverrideUpdateParams& a_params) override;

			virtual void OnClearTransform(
				int                                  a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearPlacement(
				int                                  a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;
			
			virtual void OnClearPhysics(
				int                                  a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearAllTransforms(
				int                                     a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearAllPlacement(
				int                                     a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;
			
			virtual void OnClearAllPhysics(
				int                                     a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;

			virtual Data::configNodeOverrideHolder_t& GetOrCreateConfigHolder(
				int) const override;

			virtual Data::configNodeOverrideHolder_t*
				GetConfigHolder(int) const override;

			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIPopupQueue& GetPopupQueue() override;

			void UpdateData(entryNodeOverrideData_t& a_data);

			entryNodeOverrideData_t m_data;

			Controller& m_controller;
		};
	}
}