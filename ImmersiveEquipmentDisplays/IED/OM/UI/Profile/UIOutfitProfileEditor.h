#pragma once

#include "IED/UI/Profile/UIProfileEditorBase.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/OM/UI/Widgets/UIOutfitEntryEditorWidget.h"

#include "IED/GlobalProfileManager.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		namespace OM
		{
			class UIOutfitProfileEditor :
				public UIProfileEditorBase<OutfitProfile>,
				public UIOutfitEntryEditorWidget<UIGlobalEditorDummyHandle>
			{
			public:
				static constexpr auto CHILD_ID = ChildWindowID::kProfileEditorOutfit;

				UIOutfitProfileEditor(Controller& a_controller);
				~UIOutfitProfileEditor() noexcept override;

				virtual void Initialize() override;

				virtual void DrawProfileEditorMenuBarItems() override;

				virtual std::uint32_t GetContextID() override
				{
					return static_cast<std::uint32_t>(CHILD_ID);
				}

			private:
				virtual void DrawItem(OutfitProfile& a_profile) override;

				virtual constexpr Data::ConfigClass GetConfigClass() const override;

				virtual ProfileManager<OutfitProfile>& GetProfileManager() const override;
				virtual UIPopupQueue&                  GetPopupQueue_ProfileBase() const override;

				virtual void OnItemSelected(
					const stl::fixed_string& a_item) override;

				virtual void OnProfileRename(
					const stl::fixed_string& a_oldName,
					const stl::fixed_string& a_newName) override;

				virtual void OnProfileDelete(
					const stl::fixed_string& a_name) override;

				virtual void OnProfileSave(
					const stl::fixed_string& a_name,
					OutfitProfile&           a_profile) override;

				virtual void OnProfileReload(
					const OutfitProfile& a_profile) override;

				virtual void OnSexChanged(
					Data::ConfigSex a_newSex) override;

				virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;

				virtual void OnEditorPanelSettingsChange() override;

				virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();

				virtual void OnCollapsibleStatesUpdate();

				virtual constexpr bool IsProfileEditor() const override;

				virtual void OnOutfitEntryChange(const change_param_type& a_params) override;

				Controller& m_controller;
			};
		}
	}
}