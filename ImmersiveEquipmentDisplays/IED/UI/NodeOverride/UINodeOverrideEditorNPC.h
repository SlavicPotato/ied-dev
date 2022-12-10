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
			~UINodeOverrideEditorNPC() noexcept override;

			virtual void EditorInitialize() override;

			virtual void EditorDraw() override;
			virtual void EditorOnOpen() override;
			virtual void EditorOnClose() override;
			virtual void EditorReset() override;

		private:
			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const override;

			virtual NodeOverrideEditorCurrentData GetCurrentData() override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void                         OnCollapsibleStatesUpdate();

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
			virtual void                                    OnEditorPanelSettingsChange() override;

			virtual void OnListOptionsChange() override;

			virtual void ListResetAllValues(
				Game::FormID a_handle) override;

			virtual void OnListChangeCurrentItem(
				const std::optional<UINPCList<entryNodeOverrideData_t>::listValue_t>& a_oldHandle,
				const std::optional<UINPCList<entryNodeOverrideData_t>::listValue_t>& a_newHandle) override;

			virtual entryNodeOverrideData_t GetData(
				Game::FormID a_formid) override;

			virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

			virtual void ApplyProfile(
				const profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
				const NodeOverrideProfile&                               a_profile) override;

			virtual void MergeProfile(
				const profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
				const NodeOverrideProfile&                               a_profile) override;

			virtual void OnUpdate(
				Game::FormID                                   a_handle,
				const SingleNodeOverrideTransformUpdateParams& a_params) override;

			virtual void OnUpdate(
				Game::FormID                                   a_handle,
				const SingleNodeOverridePlacementUpdateParams& a_params) override;
			
			virtual void OnUpdate(
				Game::FormID                                   a_handle,
				const SingleNodeOverridePhysicsUpdateParams& a_params) override;

			virtual void OnUpdate(
				Game::FormID                    a_handle,
				const NodeOverrideUpdateParams& a_params) override;

			virtual void OnClearTransform(
				Game::FormID                         a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearPlacement(
				Game::FormID                         a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearPhysics(
				Game::FormID                         a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearAllTransforms(
				Game::FormID                            a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearAllPlacement(
				Game::FormID                            a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;
			
			virtual void OnClearAllPhysics(
				Game::FormID                            a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;

			virtual Data::configNodeOverrideHolder_t& GetOrCreateConfigHolder(
				Game::FormID a_handle) const override;

			virtual Data::configNodeOverrideHolder_t*
				GetConfigHolder(Game::FormID a_handle) const override;

			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIPopupQueue& GetPopupQueue() override;

			virtual const ImVec4* HighlightEntry(Game::FormID a_handle) override;

			Controller& m_controller;
		};
	}
}
