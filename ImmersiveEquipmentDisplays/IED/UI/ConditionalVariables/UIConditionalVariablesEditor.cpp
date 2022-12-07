#include "pch.h"

#include "UIConditionalVariablesEditor.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIConditionalVariablesEditor::UIConditionalVariablesEditor(
			Controller& a_controller) :
			UIConditionalVariablesEditorWidget(a_controller),
			UIProfileSelectorWidget<
				profileSelectorParamsCondVar_t,
				CondVarProfile>(
				a_controller,
				UIProfileSelectorFlags::kEnableApply |
					UIProfileSelectorFlags::kEnableMerge),
			UIEquipmentOverrideConditionsWidget(a_controller),
			UILocalizationInterface(a_controller),
			UITipsInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UIConditionalVariablesEditor::EditorInitialize()
		{
			InitializeProfileBase();
		}

		void UIConditionalVariablesEditor::EditorDraw()
		{
			auto& data = m_controller.GetConfigStore().active.condvars;

			DrawCondVarEditor(data);
		}

		void UIConditionalVariablesEditor::EditorOnOpen()
		{
		}

		void UIConditionalVariablesEditor::EditorOnClose()
		{
		}

		void UIConditionalVariablesEditor::EditorReset()
		{
		}

		void UIConditionalVariablesEditor::OnCondVarEntryChange(
			const CondVarDataChangeParams& a_params)
		{
			switch (a_params.action)
			{
			case CondVarEntryChangeAction::kReset:
				m_controller.QueueClearVariablesOnAll(true);
				break;
			}
		}

		CondVarProfile::base_type UIConditionalVariablesEditor::GetData(
			const profileSelectorParamsCondVar_t& a_params)
		{
			return a_params.data;
		}

		void UIConditionalVariablesEditor::ApplyProfile(
			const profileSelectorParamsCondVar_t& a_data,
			const CondVarProfile&                 a_profile)
		{
			a_data.data = a_profile.Data();

			m_controller.QueueClearVariablesOnAll(true);
		}

		void UIConditionalVariablesEditor::MergeProfile(
			const profileSelectorParamsCondVar_t& a_data,
			const CondVarProfile&                 a_profile)
		{
			auto& data = a_profile.Data();

			for (auto& e : data.data)
			{
				auto r = a_data.data.data.emplace(e);

				if (m_profileMergeOverwrite &&
				    !r.second)
				{
					r.first->second = e.second;
				}
			}

			m_controller.QueueClearVariablesOnAll(true);
		}

		bool UIConditionalVariablesEditor::DrawProfileImportOptions(
			const profileSelectorParamsCondVar_t& a_data,
			const CondVarProfile&                 a_profile,
			bool                                  a_isMerge)
		{
			if (a_isMerge)
			{
				ImGui::PushID("import_opts");

				ImGui::Checkbox(
					LS(CommonStrings::Overwrite, "1"),
					std::addressof(m_profileMergeOverwrite));

				ImGui::PopID();
			}

			return true;
		}

		UIData::UICollapsibleStates& UIConditionalVariablesEditor::GetCollapsibleStatesData()
		{
			return m_controller.GetSettings().data.ui.condVarEditor.colStates;
		}

		void UIConditionalVariablesEditor::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
		}

		std::optional<std::reference_wrapper<Data::configConditionalVariablesHolder_t>> UIConditionalVariablesEditor::GetCurrentData()
		{
			return m_controller.GetConfigStore().active.condvars;
		}

		UIPopupQueue& UIConditionalVariablesEditor::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		void UIConditionalVariablesEditor::DrawMainHeaderControlsExtra(
			Data::configConditionalVariablesHolder_t& a_data)
		{
			if (TreeEx(
					"tree_prof",
					false,
					"%s",
					LS(CommonStrings::Profile)))
			{
				ImGui::Spacing();

				ImGui::PushItemWidth(ImGui::GetFontSize() * -15.5f);

				profileSelectorParamsCondVar_t params{ a_data };

				DrawProfileSelector(params);

				ImGui::PopItemWidth();

				ImGui::Spacing();

				ImGui::TreePop();
			}

			ImGui::Separator();
		}
	}
}