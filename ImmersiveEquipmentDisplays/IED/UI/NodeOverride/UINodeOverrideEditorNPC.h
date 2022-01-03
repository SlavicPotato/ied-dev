#pragma once

#include "UINodeOverrideEditorCommon.h"

#include "../Widgets/Lists/UINPCLIstWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINodeOverrideEditorNPC :
			public UINodeOverrideEditorCommon<Game::FormID>,
			UINPCList<entryNodeOverrideData_t>
		{
		public:
			UINodeOverrideEditorNPC(Controller& a_controller);

			virtual void Initialize() override;

			virtual void Draw() override;
			virtual void OnOpen() override;
			virtual void OnClose() override;
			virtual void Reset() override;

		private:
			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const override;

			virtual NodeOverrideEditorCurrentData GetCurrentData() override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void OnCollapsibleStatesUpdate();

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
			virtual void OnEditorPanelSettingsChange() override;

			virtual void OnListOptionsChange() override;

			virtual void ListResetAllValues(
				Game::FormID a_handle) override;

			virtual void OnListChangeCurrentItem(
				const SetObjectWrapper<UINPCList<entryNodeOverrideData_t>::listValue_t>& a_oldHandle,
				const SetObjectWrapper<UINPCList<entryNodeOverrideData_t>::listValue_t>& a_newHandle) override;

			virtual entryNodeOverrideData_t GetData(
				Game::FormID a_formid) override;

			virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

			virtual void ApplyProfile(
				const profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
				const NodeOverrideProfile& a_profile) override;

			virtual void MergeProfile(
				const profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
				const NodeOverrideProfile& a_profile) override;

			virtual void OnUpdate(
				Game::FormID a_handle,
				const SingleNodeOverrideTransformUpdateParams& a_params) override;
			
			virtual void OnUpdate(
				Game::FormID a_handle,
				const SingleNodeOverridePlacementUpdateParams& a_params) override;

			virtual void OnUpdate(
				Game::FormID a_handle,
				const NodeOverrideUpdateParams& a_params) override;

			virtual void OnClearTransform(
				Game::FormID a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;
			
			virtual void OnClearPlacement(
				Game::FormID a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearAllTransforms(
				Game::FormID a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearAllPlacement(
				Game::FormID a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;

			virtual Data::configNodeOverrideHolder_t& GetOrCreateConfigHolder(
				Game::FormID) const override;

			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIPopupQueue& GetPopupQueue() override;

			virtual const ImVec4* HighlightEntry(Game::FormID a_handle) override;

			Controller& m_controller;
		};
	}
}
