#pragma once

#include "IED/UI/Widgets/Lists/UINPCLIstWidget.h"

#include "UIOutfitEditorCommon.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		namespace OM
		{
			class UIOutfitEditorNPC :
				public UIOutfitEntryEditorCommon<
					UIOutfitEditorParamsHandle<Game::FormID>>,
				UINPCList<UIOutfitEntryListItemWrapper>
			{
			public:
				UIOutfitEditorNPC(Controller& a_controller);
				~UIOutfitEditorNPC() noexcept override;

				virtual void EditorInitialize() override;
				virtual void EditorDraw() override;
				virtual void EditorOnOpen() override;
				virtual void EditorOnClose() override;
				virtual void EditorReset() override;
				virtual void EditorQueueUpdateCurrent() override;

			private:
				virtual constexpr Data::ConfigClass GetConfigClass() const override;

				virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const override;

				virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;
				virtual void                                    OnEditorPanelSettingsChange() override;

				virtual UIOutfitEntryListItemWrapper GetData(Game::FormID a_handle) override;

				virtual void OnListChangeCurrentItem(
					const std::optional<UINPCList<UIOutfitEntryListItemWrapper>::listValue_t>& a_oldHandle,
					const std::optional<UINPCList<UIOutfitEntryListItemWrapper>::listValue_t>& a_newHandle) override;

				virtual void OnSexChanged(Data::ConfigSex a_newSex) override;

				virtual void OnListOptionsChange() override;

				virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

				virtual UIData::UICollapsibleStates& GetCollapsibleStatesData() override;
				virtual void                         OnCollapsibleStatesUpdate() override;

				virtual void ListResetAllValues(Game::FormID a_handle) override;

				virtual entryOutfitData_t& GetOrCreateEntryData(
					const UIOutfitEditorParamsHandle<Game::FormID>& a_params) const override;

				virtual std::optional<std::reference_wrapper<entryOutfitData_t>> GetEntryData(
					const UIOutfitEditorParamsHandle<Game::FormID>& a_params) const override;

				virtual void OnOutfitEntryChange(const change_param_type& a_params) override;
				virtual void OnOutfitEntryErase(const UIOutfitEditorParamsHandle<Game::FormID>& a_params) override;

				virtual void ApplyProfile(
					const profileSelectorParamsOutfit_t<UIOutfitEditorParamsHandle<Game::FormID>>& a_params,
					const OutfitProfile&                                                           a_profile) override;

				virtual const ImVec4* HighlightEntry(Game::FormID a_handle) override;

				Controller& m_controller;
			};
		}
	}
}