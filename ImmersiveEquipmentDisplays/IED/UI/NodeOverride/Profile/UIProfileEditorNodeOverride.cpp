#include "pch.h"

#include "UIProfileEditorNodeOverride.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIProfileEditorNodeOverride::UIProfileEditorNodeOverride(Controller& a_controller) :
			UIProfileEditorBase<NodeOverrideProfile>(
				UIProfileStrings::TitleNodeOverride,
				"ied_pe_no",
				a_controller),
			UINodeOverrideEditorWidget<int>(a_controller),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UIProfileEditorNodeOverride::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.transformProfileEditor.sex, false);
		}

		void UIProfileEditorNodeOverride::DrawItem(NodeOverrideProfile& a_profile)
		{
			DrawNodeOverrideEditor(0, a_profile.Data());
		}

		constexpr Data::ConfigClass UIProfileEditorNodeOverride::GetConfigClass() const
		{
			return Data::ConfigClass::Global;
		}

		ProfileManager<NodeOverrideProfile>& UIProfileEditorNodeOverride::GetProfileManager() const
		{
			return GlobalProfileManager::GetSingleton<NodeOverrideProfile>();
		}

		UIPopupQueue& UIProfileEditorNodeOverride::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		auto UIProfileEditorNodeOverride::GetCurrentData()
			-> NodeOverrideEditorCurrentData
		{
			if (!m_state.selected)
			{
				return { 0, nullptr };
			}

			auto& data = GetProfileManager().Data();
			auto it = data.find(*m_state.selected);
			if (it != data.end())
			{
				return { 0, std::addressof(it->second.Data()) };
			}
			else
			{
				return { 0, nullptr };
			}
		}

		void UIProfileEditorNodeOverride::OnItemSelected(
			const stl::fixed_string& a_name)
		{
		}

		void UIProfileEditorNodeOverride::OnProfileRename(
			const stl::fixed_string& a_oldName,
			const stl::fixed_string& a_newName)
		{
		}

		void UIProfileEditorNodeOverride::OnProfileDelete(
			const stl::fixed_string& a_name)
		{
		}

		void UIProfileEditorNodeOverride::OnProfileSave(
			const stl::fixed_string& a_name,
			NodeOverrideProfile& a_profile)
		{
		}

		void UIProfileEditorNodeOverride::OnProfileReload(
			const NodeOverrideProfile& a_profile)
		{
		}

		void UIProfileEditorNodeOverride::OnSexChanged(
			Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.transformProfileEditor.sex != a_newSex)
			{
				store.settings.Set(
					store.settings.data.ui.transformProfileEditor.sex,
					a_newSex);
			}
		}

		Data::SettingHolder::EditorPanelCommon& UIProfileEditorNodeOverride::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.transformProfileEditor;
		}

		void UIProfileEditorNodeOverride::OnEditorPanelSettingsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.MarkDirty();
		}

		UIData::UICollapsibleStates& UIProfileEditorNodeOverride::GetCollapsibleStatesData()
		{
			auto& config = m_controller.GetConfigStore().settings;

			return config.data.ui.transformProfileEditor.colStates;
		}

		void UIProfileEditorNodeOverride::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		void UIProfileEditorNodeOverride::OnUpdate(
			int a_handle,
			const SingleNodeOverrideUpdateParams& a_params)
		{
			if (GetEditorPanelSettings().sexSync)
			{
				auto sex = GetSex();

				a_params.entry(Data::GetOppositeSex(sex)) = a_params.entry(sex);
			}
		}

		void UIProfileEditorNodeOverride::OnUpdate(
			int a_handle,
			const SingleNodeOverrideParentUpdateParams& a_params)
		{
			if (GetEditorPanelSettings().sexSync)
			{
				auto sex = GetSex();

				a_params.entry(Data::GetOppositeSex(sex)) = a_params.entry(sex);
			}
		}

		/*void UIProfileEditorNodeOverride::OnUpdate(
			int a_handle,
			const NodeOverrideUpdateParams& a_params)
		{
		}*/

		void UIProfileEditorNodeOverride::OnClear(
			int a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
		}

		void UIProfileEditorNodeOverride::OnClearPlacement(
			int a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
		}

		void UIProfileEditorNodeOverride::OnClearAll(
			int a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
		}

		void UIProfileEditorNodeOverride::OnClearAllPlacement(
			int a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
		}

		WindowLayoutData UIProfileEditorNodeOverride::GetWindowDimensions() const
		{
			return { 250.0f, 600.0f, -1.0f, false };
		}

		UIPopupQueue& UIProfileEditorNodeOverride::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		void UIProfileEditorNodeOverride::DrawProfileEditorMenuBarItems()
		{
			if (ImGui::BeginMenu(LS(CommonStrings::Actions, "peb_1")))
			{
				DrawMenuBarItems();
				ImGui::EndMenu();
			}
		}

	}
}