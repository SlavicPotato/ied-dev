#pragma once

#include "Widgets/UIConditionalVariablesEditorWidget.h"

#include "IED/UI/Widgets/UIProfileSelectorWidget.h"

#include "IED/UI/UILocalizationInterface.h"

#include "IED/GlobalProfileManager.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		struct profileSelectorParamsCondVar_t
		{
			Data::configConditionalVariablesHolder_t& data;
		};

		class UIConditionalVariablesEditor :
			public UIConditionalVariablesEditorWidget,
			public UIProfileSelectorWidget<
				profileSelectorParamsCondVar_t,
				CondVarProfile>,
			public virtual UILocalizationInterface
		{
		public:
			UIConditionalVariablesEditor(Controller& a_controller);

			virtual void EditorInitialize() override;
			virtual void EditorDraw() override;
			virtual void EditorOnOpen() override;
			virtual void EditorOnClose() override;
			virtual void EditorReset() override;

		private:
			virtual void OnCondVarEntryChange(
				const CondVarDataChangeParams& a_params) override;

			virtual CondVarProfile::base_type GetData(
				const profileSelectorParamsCondVar_t& a_params) override;

			virtual void ApplyProfile(
				const profileSelectorParamsCondVar_t& a_data,
				const CondVarProfile&                 a_profile) override;

			virtual void MergeProfile(
				const profileSelectorParamsCondVar_t& a_data,
				const CondVarProfile&                 a_profile) override;

			virtual bool DrawProfileImportOptions(
				const profileSelectorParamsCondVar_t& a_data,
				const CondVarProfile&                 a_profile,
				bool                                  a_isMerge) override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData() override;
			virtual void                         OnCollapsibleStatesUpdate() override;

			virtual std::optional<std::reference_wrapper<Data::configConditionalVariablesHolder_t>> GetCurrentData() override;

			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual void DrawMainHeaderControlsExtra(
				Data::configConditionalVariablesHolder_t& a_data) override;

			bool m_profileMergeOverwrite{ true };

			Controller& m_controller;
		};
	}
}