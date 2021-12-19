#pragma once

#include "UINodeOverrideEditorCommon.h"

#include "../Widgets/Lists/UIRaceListWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINodeOverrideEditorRace :
			public UINodeOverrideEditorCommon<Game::FormID>,
			UIRaceList<entryNodeOverrideData_t>
		{
		public:
			UINodeOverrideEditorRace(Controller& a_controller);

			virtual void Initialize() override;

			virtual void Draw() override;
			virtual void OnOpen() override;
			virtual void OnClose() override;
			virtual void Reset() override;

		private:
			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual const ActorInfoHolder& GetActorInfoHolder() const override;

			virtual Data::SettingHolder::EditorPanelRaceSettings& GetRaceSettings() const override;

			virtual const SetObjectWrapper<Game::FormID>& GetCrosshairRef() override;

			virtual NodeOverrideEditorCurrentData GetCurrentData() override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void OnCollapsibleStatesUpdate();

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
			virtual void OnEditorPanelSettingsChange() override;

			virtual void OnListOptionsChange() override;

			virtual void ListResetAllValues(
				Game::FormID a_handle) override;

			virtual const entryNodeOverrideData_t& GetData(
				Game::FormID a_formid) override;

			virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

			virtual void ApplyProfile(
				profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
				const NodeOverrideProfile& a_profile) override;

			virtual void OnUpdate(
				Game::FormID a_handle,
				const SingleNodeOverrideUpdateParams& a_params) override;
			
			virtual void OnUpdate(
				Game::FormID a_handle,
				const SingleNodeOverrideParentUpdateParams& a_params) override;

			virtual void OnUpdate(
				Game::FormID a_handle,
				const NodeOverrideUpdateParams& a_params) override;

			virtual void OnClear(
				Game::FormID a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;
			
			virtual void OnClearParent(
				Game::FormID a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual Data::configNodeOverrideHolder_t& GetOrCreateConfigHolder(
				Game::FormID) const override;

			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			entryNodeOverrideData_t m_empty;
			entryNodeOverrideData_t m_temp;

			Controller& m_controller;
		};
	}
}
