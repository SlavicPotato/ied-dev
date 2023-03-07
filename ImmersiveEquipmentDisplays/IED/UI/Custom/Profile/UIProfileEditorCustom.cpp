#include "pch.h"

#include "UIProfileEditorCustom.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIProfileEditorCustom::UIProfileEditorCustom(
			Controller& a_controller) :
			UIProfileEditorBase<CustomProfile>(
				UIProfileStrings::TitleCustom,
				"ied_pe_cust"),
			UICustomEditorWidget<UIGlobalEditorDummyHandle>(a_controller),
			UIPopupInterface(a_controller),
			UISettingsInterface(a_controller),
			UIEquipmentOverrideConditionsWidget(a_controller),
			m_controller(a_controller)
		{
		}

		UIProfileEditorCustom::~UIProfileEditorCustom() noexcept
		{
			GetProfileManager().RemoveSink(this);
		}

		void UIProfileEditorCustom::Initialize()
		{
			InitializeProfileBase();

			auto& settings = m_controller.GetSettings();

			SetSex(settings.data.ui.customProfileEditor.sex, false);
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
				return {};
			}

			auto& data = GetProfileManager().Data();
			auto  it   = data.find(*m_state.selected);
			if (it != data.end())
			{
				return { 0, std::addressof(it->second.Data()) };
			}
			else
			{
				return {};
			}
		}

		UIPopupQueue& UIProfileEditorCustom::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		void UIProfileEditorCustom::OnItemSelected(
			const stl::fixed_string& a_name)
		{
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
			CustomProfile&           a_profile)
		{
		}

		void UIProfileEditorCustom::OnProfileReload(
			const CustomProfile& a_profile)
		{
		}

		void UIProfileEditorCustom::OnSexChanged(
			Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetSettings();

			if (store.data.ui.customProfileEditor.sex != a_newSex)
			{
				store.set(
					store.data.ui.customProfileEditor.sex,
					a_newSex);
			}
		}

		Data::SettingHolder::EditorPanelCommon& UIProfileEditorCustom::GetEditorPanelSettings()
		{
			return m_controller.GetSettings().data.ui.customProfileEditor;
		}

		void UIProfileEditorCustom::OnEditorPanelSettingsChange()
		{
			auto& store = m_controller.GetSettings();
			store.mark_dirty();
		}

		UIData::UICollapsibleStates& UIProfileEditorCustom::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetSettings();

			return settings.data.ui.customProfileEditor.colStates;
		}

		void UIProfileEditorCustom::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
		}

		void UIProfileEditorCustom::OnBaseConfigChange(
			UIGlobalEditorDummyHandle,
			const void*      a_params,
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
			UIGlobalEditorDummyHandle,
			const CustomConfigUpdateParams& a_params)
		{
		}

		bool UIProfileEditorCustom::OnCreateNew(
			UIGlobalEditorDummyHandle    a_handle,
			const CustomConfigNewParams& a_params)
		{
			return true;
		}

		void UIProfileEditorCustom::OnErase(
			UIGlobalEditorDummyHandle      a_handle,
			const CustomConfigEraseParams& a_params)
		{
		}

		void UIProfileEditorCustom::DrawProfileEditorMenuBarItems()
		{
			if (ImGui::BeginMenu(UIL::LS(CommonStrings::Actions, "peb_1")))
			{
				EditorDrawMenuBarItems();

				ImGui::EndMenu();
			}
		}

		void UIProfileEditorCustom::DrawMenuBarItemsExtra()
		{
		}

		bool UIProfileEditorCustom::OnRename(
			UIGlobalEditorDummyHandle       a_handle,
			const CustomConfigRenameParams& a_params)
		{
			return true;
		}

		constexpr bool UIProfileEditorCustom::BaseConfigStoreCC() const
		{
			return false;
		}

		constexpr bool UIProfileEditorCustom::IsProfileEditor() const
		{
			return true;
		}

		/*WindowLayoutData UIProfileEditorCustom::GetWindowDimensions() const
		{
			return { 100.0f, 600.0f, -1.0f, false };
		}*/

		void UIProfileEditorCustom::DrawItem(CustomProfile& a_profile)
		{
			DrawCustomEditor(0, a_profile.Data());
		}

	}
}