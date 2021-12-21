#include "pch.h"

#include "UIProfileEditorCustom.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIProfileEditorCustom::UIProfileEditorCustom(Controller& a_controller) :
			UIProfileEditorBase<CustomProfile>(
				UIProfileStrings::TitleCustom,
				"ied_pe_cust",
				a_controller),
			UICustomEditorWidget<int>(a_controller),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UIProfileEditorCustom::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.customProfileEditor.sex, false);
		}

		constexpr Data::ConfigClass UIProfileEditorCustom::GetConfigClass() const
		{
			return Data::ConfigClass::Global;
		}

		ProfileManager<CustomProfile>& UIProfileEditorCustom::GetProfileManager() const
		{
			return GlobalProfileManager::GetSingleton<CustomProfile>();
		}

		UIPopupQueue& UIProfileEditorCustom::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		auto UIProfileEditorCustom::GetCurrentData()
			-> CustomEditorCurrentData
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

		UIPopupQueue& UIProfileEditorCustom::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		void UIProfileEditorCustom::OnItemSelected(
			const stl::fixed_string& a_name)
		{
			ResetFormSelectorWidgets();
		}

		void UIProfileEditorCustom::OnProfileRename(
			const stl::fixed_string& a_oldName,
			const stl::fixed_string& a_newName)
		{
		}

		void UIProfileEditorCustom::OnProfileDelete(
			const stl::fixed_string& a_name)
		{
		}

		void UIProfileEditorCustom::OnProfileSave(
			const stl::fixed_string& a_name,
			CustomProfile& a_profile)
		{
			ResetFormSelectorWidgets();
		}

		void UIProfileEditorCustom::OnProfileReload(
			const CustomProfile& a_profile)
		{
			ResetFormSelectorWidgets();
		}

		void UIProfileEditorCustom::OnSexChanged(
			Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.customProfileEditor.sex != a_newSex)
			{
				ResetFormSelectorWidgets();
				store.settings.Set(
					store.settings.data.ui.customProfileEditor.sex,
					a_newSex);
			}
		}

		Data::SettingHolder::EditorPanelCommon& UIProfileEditorCustom::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.customProfileEditor;
		}

		void UIProfileEditorCustom::OnEditorPanelSettingsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.MarkDirty();
		}

		UIData::UICollapsibleStates& UIProfileEditorCustom::GetCollapsibleStatesData()
		{
			auto& config = m_controller.GetConfigStore().settings;

			return config.data.ui.customProfileEditor.colStates;
		}

		void UIProfileEditorCustom::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		void UIProfileEditorCustom::OnBaseConfigChange(
			int,
			const void* a_params,
			PostChangeAction a_action)
		{
			auto params = static_cast<const SingleCustomConfigUpdateParams*>(a_params);

			if (GetEditorPanelSettings().sexSync)
			{
				auto sex = GetSex();

				params->entry(Data::GetOppositeSex(sex)) = params->entry(sex);
			}
		}

		void UIProfileEditorCustom::OnFullConfigChange(
			int,
			const CustomConfigUpdateParams& a_params)
		{
			ResetFormSelectorWidgets();
		}

		bool UIProfileEditorCustom::OnCreateNew(
			int a_handle,
			const CustomConfigNewParams& a_params)
		{
			return true;
		}

		void UIProfileEditorCustom::OnErase(
			int a_handle,
			const CustomConfigEraseParams& a_params)
		{
		}

		void UIProfileEditorCustom::DrawProfileEditorMenuBarItems()
		{
			if (ImGui::BeginMenu(LS(CommonStrings::Custom, "peb_1")))
			{
				DrawMenuBarItems();

				ImGui::EndMenu();
			}
		}

		void UIProfileEditorCustom::DrawMenuBarItemsExtra()
		{
		}

		bool UIProfileEditorCustom::OnRename(
			int a_handle,
			const CustomConfigRenameParams& a_params)
		{
			return true;
		}

		constexpr bool UIProfileEditorCustom::BaseConfigStoreCC() const
		{
			return false;
		}

		WindowLayoutData UIProfileEditorCustom::GetWindowDimensions() const
		{
			return { 100.0f, 600.0f, -1.0f, false };
		}

		void UIProfileEditorCustom::DrawItem(CustomProfile& a_profile)
		{
			DrawCustomEditor(0, a_profile.Data());
		}

	}
}