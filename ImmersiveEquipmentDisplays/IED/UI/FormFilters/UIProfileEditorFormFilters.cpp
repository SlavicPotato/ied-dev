#include "pch.h"

#include "UIProfileEditorFormFilters.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIProfileEditorFormFilters::UIProfileEditorFormFilters(
			Controller& a_controller) :
			UIProfileEditorBase<FormFilterProfile>(
				UIProfileStrings::TitleFormFilters,
				"ied_pe_ff",
				a_controller),
			UIFormFilterWidget<FFPEFormFilterParams_t>(a_controller, m_formSelector),
			UILocalizationInterface(a_controller),
			m_formSelector(a_controller, FormInfoFlags::kNone, false),
			m_controller(a_controller)
		{
			SetOnChangeFunc([this](auto&) {
				m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
			});
		}

		void UIProfileEditorFormFilters::Initialize()
		{
			InitializeProfileBase();
		}

		void UIProfileEditorFormFilters::DrawProfileEditorMenuBarItems()
		{
			if (ImGui::BeginMenu(LS(UIWidgetCommonStrings::FormFilters, "peb_1")))
			{
				DrawMenuBarItems();
				ImGui::EndMenu();
			}
		}

		void UIProfileEditorFormFilters::DrawItem(
			FormFilterProfile& a_profile)
		{
			FFPEFormFilterParams_t params{
				a_profile
			};

			DrawFormFiltersBase(params, a_profile.Data());
		}

		UIPopupQueue& UIProfileEditorFormFilters::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		ProfileManager<FormFilterProfile>& UIProfileEditorFormFilters::GetProfileManager() const
		{
			return GlobalProfileManager::GetSingleton<FormFilterProfile>();
		}

		void UIProfileEditorFormFilters::OnItemSelected(
			const stl::fixed_string& a_item)
		{
		}

		void UIProfileEditorFormFilters::OnProfileRename(
			const stl::fixed_string& a_oldName,
			const stl::fixed_string& a_newName)
		{
			auto& store = m_controller.GetConfigStore();

			bool chg = false;

			store.active.slot.visit(
				[&](Data::configSlot_t& a_conf) {
					if (a_conf.raceFilter.filterFlags.test(Data::FormFilterFlags::kUseProfile) &&
				        a_conf.raceFilter.profile.name == a_oldName)
					{
						a_conf.raceFilter.profile.name = a_newName;
						chg = true;
					}

					if (a_conf.itemFilter.filterFlags.test(Data::FormFilterFlags::kUseProfile) &&
				        a_conf.itemFilter.profile.name == a_oldName)
					{
						a_conf.itemFilter.profile.name = a_newName;
						chg = true;
					}
				});

			store.active.custom.visit(
				[&](Data::configCustom_t& a_conf) {
					if (a_conf.raceFilter.filterFlags.test(Data::FormFilterFlags::kUseProfile) &&
				        a_conf.raceFilter.profile.name == a_oldName)
					{
						a_conf.raceFilter.profile.name = a_newName;
						chg = true;
					}
				});

			if (chg)
			{
				m_changedConfig = true;
				m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
			}
		}

		void UIProfileEditorFormFilters::OnProfileDelete(
			const stl::fixed_string& a_name)
		{
			m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
		}

		void UIProfileEditorFormFilters::OnProfileSave(
			const stl::fixed_string& a_name,
			FormFilterProfile& a_profile)
		{
		}

		void UIProfileEditorFormFilters::OnProfileReload(
			const FormFilterProfile& a_profile)
		{
			m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
		}

		void UIProfileEditorFormFilters::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		WindowLayoutData UIProfileEditorFormFilters::GetWindowDimensions() const
		{
			return { 200.0f, 600.0f, -1.0f, false };
		}

		void UIProfileEditorFormFilters::DrawMenuBarItems()
		{
		}
	}
}