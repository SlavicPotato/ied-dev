#include "pch.h"

#include "UIProfileEditorConditionalVariables.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIProfileEditorConditionalVariables::UIProfileEditorConditionalVariables(
			Controller& a_controller) :
			UIProfileEditorBase<CondVarProfile>(
				UIProfileStrings::TitleConditionalVariables,
				"ied_pe_cv"),
			UIConditionalVariablesEditorWidget(a_controller),
			UIEquipmentOverrideConditionsWidget(a_controller),
			m_controller(a_controller)
		{
		}

		UIProfileEditorConditionalVariables::~UIProfileEditorConditionalVariables() noexcept
		{
			GetProfileManager().RemoveSink(this);
		}

		void UIProfileEditorConditionalVariables::Initialize()
		{
			InitializeProfileBase();
		}

		void UIProfileEditorConditionalVariables::DrawProfileEditorMenuBarItems()
		{
			if (ImGui::BeginMenu(UIL::LS(CommonStrings::Actions, "peb_1")))
			{
				EditorDrawMenuBarItems();

				ImGui::EndMenu();
			}
		}

		void UIProfileEditorConditionalVariables::DrawItem(CondVarProfile& a_profile)
		{
			DrawCondVarEditor(a_profile.Data());
		}

		ProfileManager<CondVarProfile>& UIProfileEditorConditionalVariables::GetProfileManager() const
		{
			return GlobalProfileManager::GetSingleton<CondVarProfile>();
		}

		UIPopupQueue& UIProfileEditorConditionalVariables::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIData::UICollapsibleStates& UIProfileEditorConditionalVariables::GetCollapsibleStatesData()
		{
			return m_controller.GetSettings().data.ui.condVarProfileEditor.colStates;
		}

		void UIProfileEditorConditionalVariables::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
		}

		std::optional<std::reference_wrapper<Data::configConditionalVariablesHolder_t>> UIProfileEditorConditionalVariables::GetCurrentData()
		{
			if (!m_state.selected)
			{
				return {};
			}

			auto& data = GetProfileManager().Data();
			auto  it   = data.find(*m_state.selected);
			if (it != data.end())
			{
				return it->second.Data();
			}
			else
			{
				return {};
			}
		}

		void UIProfileEditorConditionalVariables::OnItemSelected(
			const stl::fixed_string& a_item)
		{
		}

		void UIProfileEditorConditionalVariables::OnProfileRename(
			const stl::fixed_string& a_oldName,
			const stl::fixed_string& a_newName)
		{
		}

		void UIProfileEditorConditionalVariables::OnProfileDelete(
			const stl::fixed_string& a_name)
		{
		}

		void UIProfileEditorConditionalVariables::OnProfileSave(
			const stl::fixed_string& a_name,
			CondVarProfile&          a_profile)
		{
		}

		void UIProfileEditorConditionalVariables::OnProfileReload(
			const CondVarProfile& a_profile)
		{
		}

		void UIProfileEditorConditionalVariables::OnCondVarEntryChange(
			const CondVarDataChangeParams& a_params)
		{
		}

		bool UIProfileEditorConditionalVariables::IsProfileEditor() const
		{
			return true;
		}
	}
}