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
				"ied_pe_ff"),
			UIFormFilterWidget<FFPEFormFilterParams_t>(a_controller, m_formSelector),
			m_formSelector(a_controller, FormInfoFlags::kNone, false),
			m_controller(a_controller)
		{
			SetOnChangeFunc([this](auto&) {
				m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
			});
		}

		UIProfileEditorFormFilters::~UIProfileEditorFormFilters() noexcept
		{
			GetProfileManager().RemoveSink(this);
		}

		void UIProfileEditorFormFilters::Initialize()
		{
			InitializeProfileBase();
		}

		void UIProfileEditorFormFilters::DrawProfileEditorMenuBarItems()
		{
			if (ImGui::BeginMenu(UIL::LS(CommonStrings::Actions, "peb_1")))
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

		static constexpr bool upd_base_filt(
			const stl::fixed_string&   a_oldName,
			const stl::fixed_string&   a_newName,
			Data::configBaseFilters_t& a_conf)
		{
			bool result = false;

			if (a_conf.raceFilter.filterFlags.test(Data::FormFilterFlags::kUseProfile) &&
			    a_conf.raceFilter.profile.name == a_oldName)
			{
				a_conf.raceFilter.profile.name = a_newName;
				result                         = true;
			}

			if (a_conf.npcFilter.filterFlags.test(Data::FormFilterFlags::kUseProfile) &&
			    a_conf.npcFilter.profile.name == a_oldName)
			{
				a_conf.npcFilter.profile.name = a_newName;
				result                        = true;
			}

			if (a_conf.actorFilter.filterFlags.test(Data::FormFilterFlags::kUseProfile) &&
			    a_conf.actorFilter.profile.name == a_oldName)
			{
				a_conf.actorFilter.profile.name = a_newName;
				result                          = true;
			}

			return result;
		}

		void UIProfileEditorFormFilters::OnProfileRename(
			const stl::fixed_string& a_oldName,
			const stl::fixed_string& a_newName)
		{
			auto& store = m_controller.GetActiveConfig();

			bool chg = false;

			store.slot.visit(
				[&](Data::configSlot_t& a_conf) {
					if (a_conf.filters)
					{
						if (upd_base_filt(a_oldName, a_newName, *a_conf.filters))
						{
							chg = true;
						}
					}

					if (a_conf.itemFilter.filterFlags.test(Data::FormFilterFlags::kUseProfile) &&
				        a_conf.itemFilter.profile.name == a_oldName)
					{
						a_conf.itemFilter.profile.name = a_newName;
						chg                            = true;
					}
				});

			store.custom.visit(
				[&](Data::configCustom_t& a_conf) {
					if (a_conf.filters)
					{
						if (upd_base_filt(a_oldName, a_newName, *a_conf.filters))
						{
							chg = true;
						}
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
			FormFilterProfile&       a_profile)
		{
		}

		void UIProfileEditorFormFilters::OnProfileReload(
			const FormFilterProfile& a_profile)
		{
			m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
		}

		void UIProfileEditorFormFilters::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
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