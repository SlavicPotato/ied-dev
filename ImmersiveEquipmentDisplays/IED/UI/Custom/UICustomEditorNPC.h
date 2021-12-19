#pragma once

#include "../UICommon.h"
#include "../Widgets/Lists/UINPCLIstWidget.h"

#include "UICustomEditorCommon.h"

#include "IED/GlobalProfileManager.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UICustomEditorNPC :
			public UICustomEditorCommon<Game::FormID>,
			UINPCList<entryCustomData_t>
		{
		public:
			UICustomEditorNPC(Controller& a_controller);

			virtual void Initialize() override;

			virtual void Draw() override;
			virtual void OnOpen() override;
			virtual void OnClose() override;
			virtual void Reset() override;
			virtual void QueueUpdateCurrent() override;

		private:
			virtual constexpr Data::ConfigClass GetConfigClass() const override;
			virtual const ActorInfoHolder& GetActorInfoHolder() const override;
			virtual const NPCInfoHolder& GetNPCInfoHolder() const override;

			std::uint64_t GetActorInfoUpdateID() const override;

			virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const override;

			virtual Data::configCustomHolder_t& GetOrCreateConfigSlotHolder(
				Game::FormID a_handle) const override;

			virtual const entryCustomData_t& GetData(
				Game::FormID a_formid) override;

			virtual CustomEditorCurrentData GetCurrentData() override;

			virtual const SetObjectWrapper<Game::FormID>& GetCrosshairRef() override;

			virtual UIPopupQueue& GetPopupQueue() override;
			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void OnCollapsibleStatesUpdate();

			virtual void OnListOptionsChange() override;

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
			virtual void OnEditorPanelSettingsChange() override;

			virtual void ListResetAllValues(
				Game::FormID a_handle) override;

			virtual void OnListChangeCurrentItem(
				const SetObjectWrapper<UINPCList<entryCustomData_t>::listValue_t>& a_oldHandle,
				const SetObjectWrapper<UINPCList<entryCustomData_t>::listValue_t>& a_newHandle) override;

			virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

			virtual void ApplyProfile(
				profileSelectorParamsCustom_t<Game::FormID>& a_data,
				const CustomProfile& a_profile) override;
			
			virtual void MergeProfile(
				profileSelectorParamsCustom_t<Game::FormID>& a_data,
				const CustomProfile& a_profile) override;

			virtual void OnBaseConfigChange(
				Game::FormID a_handle,
				const void* a_params,
				PostChangeAction a_action) override;

			virtual void OnFullConfigChange(
				Game::FormID a_handle,
				const CustomConfigUpdateParams& a_params) override;

			virtual bool OnCreateNew(
				Game::FormID a_handle,
				const CustomConfigNewParams& a_params) override;

			virtual void OnErase(
				Game::FormID a_handle,
				const CustomConfigEraseParams& a_params) override;

			virtual bool OnRename(
				Game::FormID a_handle,
				const CustomConfigRenameParams& a_params) override;

			entryCustomData_t m_empty;
			Controller& m_controller;
		};

	}
}