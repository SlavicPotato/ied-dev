#pragma once

#include "IED/UI/Widgets/Lists/UIRaceListWidget.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "UIOutfitEditorCommon.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		namespace OM
		{
			class UIOutfitEditorGlobal :
				public UIOutfitEntryEditorCommon<
					UIOutfitEditorParamsHandle<UIGlobalEditorDummyHandle>>
			{
			public:
				UIOutfitEditorGlobal(Controller& a_controller);
				~UIOutfitEditorGlobal() noexcept override;

				virtual void EditorInitialize() override;
				virtual void EditorDraw() override;
				virtual void EditorOnOpen() override;
				virtual void EditorOnClose() override;
				virtual void EditorReset() override;
				virtual void EditorQueueUpdateCurrent() override;

			private:
				virtual constexpr Data::ConfigClass GetConfigClass() const override;

				virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
				virtual void                                    OnEditorPanelSettingsChange() override;

				virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

				virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

				virtual UIData::UICollapsibleStates& GetCollapsibleStatesData() override;
				virtual void                         OnCollapsibleStatesUpdate() override;

				virtual entryOutfitData_t& GetOrCreateEntryData(
					const UIOutfitEditorParamsHandle<Game::FormID>& a_params) const override;

				virtual std::optional<std::reference_wrapper<entryOutfitData_t>> GetEntryData(
					const UIOutfitEditorParamsHandle<Game::FormID>& a_params) const override;

				virtual void OnOutfitEntryChange(const change_param_type& a_params) override;
				virtual void OnOutfitEntryErase(const UIOutfitEditorParamsHandle<Game::FormID>& a_params) override;

				virtual void ApplyProfile(
					const profileSelectorParamsOutfit_t<UIOutfitEditorParamsHandle<UIGlobalEditorDummyHandle>>& a_params,
					const OutfitProfile&                                                                        a_profile) override;

				void UpdateData();

				UIOutfitEntryListItemWrapper m_data;

				Controller& m_controller;
			};
		}
	}
}