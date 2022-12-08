#pragma once

#include "IED/UI/ConditionalVariables/Widgets/UIConditionalVariablesEditorWidget.h"

#include "IED/UI/Profile/UIProfileEditorBase.h"

#include "IED/UI/UICommon.h"

#include "IED/GlobalProfileManager.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIProfileEditorConditionalVariables :
			public UIProfileEditorBase<CondVarProfile>,
			public UIConditionalVariablesEditorWidget
		{
		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUIProfileEditorConditionalVariables;

			UIProfileEditorConditionalVariables(Controller& a_controller);

			~UIProfileEditorConditionalVariables() noexcept override;

			virtual void Initialize() override;

			virtual void DrawProfileEditorMenuBarItems() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			virtual void DrawItem(CondVarProfile& a_profile) override;

			virtual ProfileManager<CondVarProfile>& GetProfileManager() const override;
			virtual UIPopupQueue&                   GetPopupQueue_ProfileBase() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData() override;
			virtual void                         OnCollapsibleStatesUpdate() override;

			virtual std::optional<std::reference_wrapper<Data::configConditionalVariablesHolder_t>> GetCurrentData() override;

			virtual void OnItemSelected(
				const stl::fixed_string& a_item) override;

			virtual void OnProfileRename(
				const stl::fixed_string& a_oldName,
				const stl::fixed_string& a_newName) override;

			virtual void OnProfileDelete(
				const stl::fixed_string& a_name) override;

			virtual void OnProfileSave(
				const stl::fixed_string& a_name,
				CondVarProfile&          a_profile) override;

			virtual void OnProfileReload(
				const CondVarProfile& a_profile) override;

			virtual void OnCondVarEntryChange(
				const CondVarDataChangeParams& a_params) override;

			Controller& m_controller;
		};
	}
}