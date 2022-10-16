#pragma once

#include "IED/UI/UICommon.h"

#include "IED/GlobalProfileManager.h"

#include "IED/UI/EquipmentSlots/UISlotEditorBase.h"

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
		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUIProfileEditorSlot;

			UIProfileEditorSlot(Controller& a_controller);

			virtual void Initialize() override;

			virtual void DrawProfileEditorMenuBarItems() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			virtual void DrawItem(SlotProfile& a_profile) override;

			virtual void DrawMenuBarItemsExtra() override;

			virtual bool ShowConfigClassIndicator() const override;

			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual ProfileManager<SlotProfile>& GetProfileManager() const override;
			virtual UIPopupQueue&                GetPopupQueue_ProfileBase() const override;

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
				SlotProfile&             a_profile) override;

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
				const void*      a_params,
				PostChangeAction a_action) override;

			virtual void
				OnFullConfigChange(
					int,
					const SlotConfigUpdateParams& a_params) override;

			virtual void OnPriorityConfigChange(
				int                                   a_handle,
				const SlotPriorityConfigUpdateParams& a_params) override;

			virtual void OnPriorityConfigClear(
				int                           a_handle,
				const SlotConfigUpdateParams& a_params) override;

			virtual void OnSingleSlotClear(
				int,
				const SingleSlotConfigClearParams& a_params) override;

			virtual void OnFullConfigClear(
				int                              a_handle,
				const FullSlotConfigClearParams& a_params) override;

			bool CreateSlot(Data::ObjectSlot a_slot);
			void CreateAllSlots();

			void DrawAddSlotMenu();

			virtual constexpr bool BaseConfigStoreCC() const override;

			virtual WindowLayoutData GetWindowDimensions() const;

			std::optional<cachedItem_t<entrySlotData_t>> m_cachedItem;

			Controller& m_controller;
		};
	}
}