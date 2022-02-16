#pragma once

#include "../UICommon.h"
#include "UISlotEditorCommon.h"

#include "../Widgets/Lists/UIRaceListWidget.h"
#include "../Widgets/UIProfileSelectorWidget.h"
#include "Widgets/UISlotEditorWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UISlotEditorRace :
			// public UISlotEditorWidget<Game::FormID>,
			public UISlotEditorCommon<Game::FormID>,
			UIRaceList<entrySlotData_t>
		{
		public:
			UISlotEditorRace(Controller& a_controller);

			virtual void Initialize() override;
			virtual void Draw() override;
			virtual void OnOpen() override;
			virtual void OnClose() override;
			virtual void Reset() override;
			virtual void QueueUpdateCurrent() override;

		private:
			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual Data::SettingHolder::EditorPanelRaceSettings& GetRaceSettings() const override;

			virtual SlotEditorCurrentData GetCurrentData() override;

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;

			virtual void OnEditorPanelSettingsChange() override;

			virtual void ListResetAllValues(Game::FormID a_handle) override;

			Data::configSlotHolder_t& GetOrCreateConfigSlotHolder(
				Game::FormID a_handle) const override;

			virtual void MergeProfile(
				const profileSelectorParamsSlot_t<Game::FormID>& a_data,
				const SlotProfile&                               a_profile) override;

			virtual void ApplyProfile(
				const profileSelectorParamsSlot_t<Game::FormID>& a_data,
				const SlotProfile&                               a_profile) override;

			virtual entrySlotData_t GetData(
				Game::FormID a_formid) override;

			void OnBaseConfigChange(
				Game::FormID     a_handle,
				const void*      a_params,
				PostChangeAction a_action) override;

			virtual void OnFullConfigChange(
				Game::FormID                  a_handle,
				const SlotConfigUpdateParams& a_params) override;

			virtual void
				OnSingleSlotClear(
					Game::FormID                       a_handle,
					const SingleSlotConfigClearParams& a_params) override;

			virtual void OnFullConfigClear(
				Game::FormID                     a_handle,
				const FullSlotConfigClearParams& a_params) override;

			virtual void OnSexChanged(
				Data::ConfigSex a_newSex) override;

			virtual void OnListOptionsChange() override;

			virtual UIPopupQueue& GetPopupQueue() override;
			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData() override;
			virtual void                         OnCollapsibleStatesUpdate() override;

			virtual const ImVec4* HighlightEntry(Game::FormID a_handle) override;

			virtual entrySlotData_t GetCurrentData(
				Game::FormID a_handle) override;

			Controller& m_controller;
		};

	}  // namespace UI
}  // namespace IED