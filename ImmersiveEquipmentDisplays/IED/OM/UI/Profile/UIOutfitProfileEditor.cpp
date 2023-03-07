#include "pch.h"

#include "UIOutfitProfileEditor.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		namespace OM
		{
			UIOutfitProfileEditor::UIOutfitProfileEditor(
				Controller& a_controller) :
				UIProfileEditorBase<OutfitProfile>(
					UIProfileStrings::TitleOutfit,
					"ied_pe_otft"),
				UIOutfitEntryEditorWidget<UIGlobalEditorDummyHandle>(a_controller),
				UISettingsInterface(a_controller),
				UIEquipmentOverrideConditionsWidget(a_controller),
				m_controller(a_controller)
			{
			}

			UIOutfitProfileEditor::~UIOutfitProfileEditor() noexcept
			{
				GetProfileManager().RemoveSink(this);
			}

			void UIOutfitProfileEditor::Initialize()
			{
				InitializeProfileBase();

				auto& settings = m_controller.GetSettings();

				SetSex(settings.data.ui.outfitProfileEditor.sex, false);
			}

			void UIOutfitProfileEditor::DrawProfileEditorMenuBarItems()
			{
				if (ImGui::BeginMenu(UIL::LS(CommonStrings::Actions, "peb_1")))
				{
					EditorDrawMenuBarItems();

					ImGui::EndMenu();
				}
			}

			void UIOutfitProfileEditor::DrawItem(OutfitProfile& a_profile)
			{
				const UIGlobalEditorDummyHandle             dummy;
				const UIOutfitEntryEditorWidgetParamWrapper params{ dummy };

				DrawOutfitEntryWidget(a_profile.Data(), params);
			}

			constexpr Data::ConfigClass UIOutfitProfileEditor::GetConfigClass() const
			{
				return Data::ConfigClass::Global;
			}

			ProfileManager<OutfitProfile>& UIOutfitProfileEditor::GetProfileManager() const
			{
				return GlobalProfileManager::GetSingleton<OutfitProfile>();
			}

			UIPopupQueue& UIOutfitProfileEditor::GetPopupQueue_ProfileBase() const
			{
				return m_controller.UIGetPopupQueue();
			}

			void UIOutfitProfileEditor::OnItemSelected(
				const stl::fixed_string& a_name)
			{
			}

			void UIOutfitProfileEditor::OnProfileRename(
				const stl::fixed_string& a_oldName,
				const stl::fixed_string& a_newName)
			{
			}

			void UIOutfitProfileEditor::OnProfileDelete(
				const stl::fixed_string& a_name)
			{
			}

			void UIOutfitProfileEditor::OnProfileSave(
				const stl::fixed_string& a_name,
				OutfitProfile&           a_profile)
			{
			}

			void UIOutfitProfileEditor::OnProfileReload(
				const OutfitProfile& a_profile)
			{
			}

			void UIOutfitProfileEditor::OnSexChanged(
				Data::ConfigSex a_newSex)
			{
				auto& store = m_controller.GetSettings();

				if (store.data.ui.outfitProfileEditor.sex != a_newSex)
				{
					store.set(
						store.data.ui.outfitProfileEditor.sex,
						a_newSex);
				}
			}

			Data::SettingHolder::EditorPanelCommon& UIOutfitProfileEditor::GetEditorPanelSettings()
			{
				return m_controller.GetSettings().data.ui.outfitProfileEditor;
			}

			void UIOutfitProfileEditor::OnEditorPanelSettingsChange()
			{
				auto& store = m_controller.GetSettings();
				store.mark_dirty();
			}

			UIData::UICollapsibleStates& UIOutfitProfileEditor::GetCollapsibleStatesData()
			{
				auto& settings = m_controller.GetSettings();

				return settings.data.ui.outfitProfileEditor.colStates;
			}

			void UIOutfitProfileEditor::OnCollapsibleStatesUpdate()
			{
				m_controller.GetSettings().mark_dirty();
			}

			constexpr bool UIOutfitProfileEditor::IsProfileEditor() const
			{
				return true;
			}

			void UIOutfitProfileEditor::OnOutfitEntryChange(const change_param_type& a_params)
			{
				if (GetEditorPanelSettings().sexSync)
				{
					const auto sex = GetSex();

					a_params.data.data(Data::GetOppositeSex(sex)) = a_params.data.data(sex);
				}
			}

		}
	}
}