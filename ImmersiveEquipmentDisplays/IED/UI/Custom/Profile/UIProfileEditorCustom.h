#pragma once

#include "IED/UI/UICommon.h"

#include "IED/GlobalProfileManager.h"

#include "IED/UI/Custom/Widgets/UICustomEditorWidget.h"

#include "IED/UI/Profile/UIProfileEditorBase.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIProfileEditorCustom :
			public UIProfileEditorBase<CustomProfile>,
			public UICustomEditorWidget<UIGlobalEditorDummyHandle>
		{
		public:
			static constexpr auto CHILD_ID = ChildWindowID::kUIProfileEditorCustom;

			UIProfileEditorCustom(Controller& a_controller);			
			~UIProfileEditorCustom() noexcept override;

			virtual void Initialize() override;

			virtual void DrawProfileEditorMenuBarItems() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			virtual void DrawItem(CustomProfile& a_profile) override;

			virtual void DrawMenuBarItemsExtra() override;

			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual ProfileManager<CustomProfile>& GetProfileManager() const override;
			virtual UIPopupQueue&                  GetPopupQueue_ProfileBase() const override;

			virtual CustomEditorCurrentData GetCurrentData() override;

			virtual UIPopupQueue& GetPopupQueue() override;

			virtual void OnItemSelected(
				const stl::fixed_string& a_item) override;

			virtual void OnProfileRename(
				const stl::fixed_string& a_oldName,
				const stl::fixed_string& a_newName) override;

			virtual void OnProfileDelete(
				const stl::fixed_string& a_name) override;

			virtual void OnProfileSave(
				const stl::fixed_string& a_name,
				CustomProfile&           a_profile) override;

			virtual void OnProfileReload(
				const CustomProfile& a_profile) override;

			virtual void OnSexChanged(
				Data::ConfigSex a_newSex) override;

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;

			virtual void OnEditorPanelSettingsChange() override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();

			virtual void OnCollapsibleStatesUpdate();

			virtual void OnBaseConfigChange(
				UIGlobalEditorDummyHandle,
				const void*      a_params,
				PostChangeAction a_action) override;

			virtual void OnFullConfigChange(
				UIGlobalEditorDummyHandle,
				const CustomConfigUpdateParams& a_params) override;

			virtual bool OnCreateNew(
				UIGlobalEditorDummyHandle    a_handle,
				const CustomConfigNewParams& a_params) override;

			virtual void OnErase(
				UIGlobalEditorDummyHandle      a_handle,
				const CustomConfigEraseParams& a_params) override;

			virtual bool OnRename(
				UIGlobalEditorDummyHandle       a_handle,
				const CustomConfigRenameParams& a_params) override;

			virtual constexpr bool BaseConfigStoreCC() const override;

			virtual constexpr bool IsProfileEditor() const override;

			//virtual WindowLayoutData GetWindowDimensions() const;

			Controller& m_controller;
		};
	}
}