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
				"IED_pe_ff",
				a_controller),
			UIFormFilterWidget<FFPEFormFilterParams_t>(a_controller, m_formSelector),
			UILocalizationInterface(a_controller),
			m_formSelector(a_controller, FormInfoFlags::kNone, false),
			m_controller(a_controller)
		{
			SetOnChangeFunc([this](FFPEFormFilterParams_t&) {
				m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
			});
		}

		void UIProfileEditorFormFilters::Initialize()
		{
			InitializeProfileBase();
		}

		void UIProfileEditorFormFilters::DrawProfileEditorMenuBarItems()
		{
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

			store.active.slot.visit(
				[this, &a = a_oldName, &b = a_newName](
					Data::configSlot_t& a_conf) {
					if (a_conf.raceFilter.filterFlags.test(Data::FormFilterFlags::kUseProfile) &&
				        a_conf.raceFilter.profile.name == a)
					{
						a_conf.raceFilter.profile.name = b;
						m_changedConfig = true;
					}

					if (a_conf.itemFilter.filterFlags.test(Data::FormFilterFlags::kUseProfile) &&
				        a_conf.itemFilter.profile.name == a)
					{
						a_conf.itemFilter.profile.name = b;
						m_changedConfig = true;
					}
				});

			store.active.custom.visit(
				[this, &a = a_oldName, &b = a_newName](
					Data::configCustom_t& a_conf) {
					if (a_conf.raceFilter.filterFlags.test(Data::FormFilterFlags::kUseProfile) &&
				        a_conf.raceFilter.profile.name == a)
					{
						a_conf.raceFilter.profile.name = b;
						m_changedConfig = true;
					}
				});
		}

		void UIProfileEditorFormFilters::OnProfileDelete(
			const stl::fixed_string& a_name)
		{
		}

		void UIProfileEditorFormFilters::OnProfileSave(
			const stl::fixed_string& a_name,
			FormFilterProfile& a_profile)
		{
		}

		void UIProfileEditorFormFilters::OnProfileReload(
			const FormFilterProfile& a_profile)
		{
		}

		void UIProfileEditorFormFilters::OnCollapsibleStatesUpdate()
		{
		}
	}
}