#pragma once

#include "../../UICommon.h"

#include "IED/GlobalProfileManager.h"

#include "../UISlotEditorBase.h"

#include "IED/UI/Profile/UIProfileEditorBase.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIProfileEditorSlot :
			public UIProfileEditorBase<SlotProfile>,
			public UISlotEditorBase<int>
		{
			struct cachedItem_t
			{
				stl::fixed_string name;
				Data::configStoreSlot_t::result_copy data;
			};

		public:
			UIProfileEditorSlot(Controller& a_controller);

			virtual void Initialize() override;

			virtual void DrawProfileEditorMenuBarItems() override;

		private:
			virtual void DrawItem(SlotProfile& a_profile) override;

			virtual void DrawMenuBarItemsExtra() override;

			virtual bool ShowConfigClassIndicator() const override;

			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual ProfileManager<SlotProfile>& GetProfileManager() const override;
			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual SlotEditorCurrentData GetCurrentData() override;

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
				SlotProfile& a_profile) override;

			virtual void OnProfileReload(
				const SlotProfile& a_profile) override;

			virtual void OnSexChanged(
				Data::ConfigSex a_newSex) override;

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;

			virtual void OnEditorPanelSettingsChange() override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();

			virtual void OnCollapsibleStatesUpdate();

			virtual void OnBaseConfigChange(
				int,
				const void* a_params,
				PostChangeAction a_action) override;

			virtual void
				OnFullConfigChange(
					int,
					const SlotConfigUpdateParams& a_params) override;

			virtual void OnSingleSlotClear(
				int,
				const void* a_params) override;

			virtual void OnFullConfigClear(
				int a_handle) override;

			bool CreateSlot(Data::ObjectSlot a_slot);
			void CreateAllSlots();

			void DrawAddSlotMenu();

			virtual constexpr bool BaseConfigStoreCC() const override;

			virtual WindowLayoutData GetWindowDimensions() const;

			SetObjectWrapper<cachedItem_t> m_cachedItem;

			Controller& m_controller;
		};
	}  // namespace UI
}  // namespace IED