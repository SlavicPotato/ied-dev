#pragma once

#include "../../UICommon.h"

#include "IED/GlobalProfileManager.h"

#include "../Widgets/UINodeOverrideEditorWidget.h"

#include "IED/UI/Profile/UIProfileEditorBase.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIProfileEditorNodeOverride :
			public UIProfileEditorBase<NodeOverrideProfile>,
			public UINodeOverrideEditorWidget<int>
		{
		public:
			UIProfileEditorNodeOverride(Controller& a_controller);

			virtual void Initialize() override;

			virtual void DrawProfileEditorMenuBarItems() override;

		private:
			virtual void DrawItem(NodeOverrideProfile& a_profile) override;

			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual ProfileManager<NodeOverrideProfile>& GetProfileManager() const override;
			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual NodeOverrideEditorCurrentData GetCurrentData() override;

			virtual void OnItemSelected(
				const stl::fixed_string& a_item) override;

			virtual void OnProfileRename(
				const stl::fixed_string& a_oldName,
				const stl::fixed_string& a_newName) override;

			virtual void OnProfileDelete(
				const stl::fixed_string& a_name) override;

			virtual void OnProfileSave(
				const stl::fixed_string& a_name,
				NodeOverrideProfile& a_profile) override;

			virtual void OnProfileReload(
				const NodeOverrideProfile& a_profile) override;

			virtual void OnSexChanged(
				Data::ConfigSex a_newSex) override;

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;

			virtual void OnEditorPanelSettingsChange() override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();

			virtual void OnCollapsibleStatesUpdate();

			virtual void OnUpdate(
				int a_handle,
				const SingleNodeOverrideUpdateParams& a_params) override;
			
			virtual void OnUpdate(
				int a_handle,
				const SingleNodeOverrideParentUpdateParams& a_params) override;

			/*virtual void OnUpdate(
				int a_handle,
				const NodeOverrideUpdateParams& a_params) override;*/

			virtual void OnClear(
				int a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;
			
			virtual void OnClearParent(
				int a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual WindowLayoutData GetWindowDimensions() const;

			Controller& m_controller;
		};
	}  // namespace UI
}  // namespace IED