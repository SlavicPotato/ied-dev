#pragma once

#include "IED/ConfigCommon.h"

#include "IED/UI/Widgets/UIFormTypeSelectorWidget.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"
#include "UIFormSelectorWidget.h"

#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UIFormLookupInterface.h"

#include "IED/GlobalProfileManager.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		enum class FormFilterAction : std::uint32_t
		{
			None = 0,
			Paste,
			Add,
			Delete
		};

		template <class T>
		class UIFormFilterWidget :
			UIFormLookupInterface
		{
			using callback_func_t = std::function<void(T&)>;

		public:
			UIFormFilterWidget(
				Controller&           a_controller,
				UIFormSelectorWidget& a_formSelector);

			void DrawFormFiltersTree(
				const char*               a_label,
				T&                        a_params,
				Data::configFormFilter_t& a_data);

			template <class Tfo>
			void DrawFormFiltersTree(
				const char*               a_label,
				T&                        a_params,
				Data::configFormFilter_t& a_data,
				Tfo                       a_func);

			void DrawFormFilters(
				T&                        a_params,
				Data::configFormFilter_t& a_data);

			void DrawFormFiltersBase(
				T&                            a_params,
				Data::configFormFilterBase_t& a_data);

			constexpr void SetOnChangeFunc(callback_func_t a_func)
			{
				m_onChangeFunc = std::move(a_func);
			}

		private:
			FormFilterAction DrawFormFiltersHeaderTreeContextMenu(
				T&                        a_params,
				Data::configFormFilter_t& a_data);

			FormFilterAction DrawFormFilterHeaderContextMenu(
				T&                     a_params,
				Data::configFormSet_t& a_data);

			void DrawFormFilterTable(
				T&                     a_params,
				Data::configFormSet_t& a_data);

			FormFilterAction DrawFormFilterEntryContextMenu();

			void DrawProfileSelectorCombo(
				T&                        a_params,
				Data::configFormFilter_t& a_data);

			Game::FormID          m_ffNewEntryID;
			UIFormSelectorWidget& m_formSelector;
			callback_func_t       m_onChangeFunc;
		};

		template <class T>
		UIFormFilterWidget<T>::UIFormFilterWidget(
			Controller&           a_controller,
			UIFormSelectorWidget& a_formSelector) :
			UIFormLookupInterface(a_controller),
			m_formSelector(a_formSelector)
		{
		}

		template <class T>
		void UIFormFilterWidget<T>::DrawFormFiltersTree(
			const char*               a_label,
			T&                        a_params,
			Data::configFormFilter_t& a_data)
		{
			DrawFormFiltersTree(a_label, a_params, a_data, [] {});
		}

		template <class T>
		template <class Tfo>
		void UIFormFilterWidget<T>::DrawFormFiltersTree(
			const char*               a_label,
			T&                        a_params,
			Data::configFormFilter_t& a_data,
			Tfo                       a_func)
		{
			if (DrawFormFiltersHeaderTreeContextMenu(
					a_params,
					a_data) == FormFilterAction::Paste)
			{
				ImGui::SetNextItemOpen(true);
			}

			if (ImGui::TreeNodeEx(
					"ff_tree",
					ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					a_label))
			{
				ImGui::Spacing();

				a_func();
				DrawFormFilters(a_params, a_data);

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		template <class T>
		void UIFormFilterWidget<T>::DrawFormFilters(
			T&                        a_params,
			Data::configFormFilter_t& a_data)
		{
			ImGui::PushID("ff_area");

			ImGui::PushItemWidth(ImGui::GetFontSize() * -14.0f);

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::UseProfile, "ctl"),
					stl::underlying(std::addressof(a_data.filterFlags.value)),
					stl::underlying(Data::FormFilterFlags::kUseProfile)))
			{
				if (!a_data.filterFlags.test(Data::FormFilterFlags::kUseProfile))
				{
					a_data.profile.name.clear();
				}
				m_onChangeFunc(a_params);
			}

			if (a_data.filterFlags.test(Data::FormFilterFlags::kUseProfile))
			{
				DrawProfileSelectorCombo(a_params, a_data);
			}
			else
			{
				DrawFormFiltersBase(a_params, a_data);
			}

			ImGui::PopItemWidth();

			ImGui::PopID();
		}

		template <class T>
		inline void UIFormFilterWidget<T>::DrawFormFiltersBase(
			T&                            a_params,
			Data::configFormFilterBase_t& a_data)
		{
			ImGui::PushID("ff_base_area");

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::DenyAll, "ctl"),
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::FormFilterBaseFlags::kDenyAll)))
			{
				m_onChangeFunc(a_params);
			}

			ImGui::Spacing();

			ImGui::PushID("ff_allow");

			auto result = DrawFormFilterHeaderContextMenu(
				a_params,
				a_data.allow);

			bool empty = a_data.allow.empty();

			if (!empty)
			{
				if (result == FormFilterAction::Add ||
				    result == FormFilterAction::Paste)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			UICommon::PushDisabled(empty);

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::Allow)))
			{
				if (!empty)
				{
					ImGui::Spacing();
					DrawFormFilterTable(a_params, a_data.allow);
					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);

			ImGui::PopID();

			ImGui::PushID("ff_deny");

			const bool disabled = a_data.flags.test(Data::FormFilterBaseFlags::kDenyAll);

			UICommon::PushDisabled(disabled);

			result = DrawFormFilterHeaderContextMenu(
				a_params,
				a_data.deny);

			empty = a_data.deny.empty();

			if (!empty)
			{
				if (result == FormFilterAction::Add ||
				    result == FormFilterAction::Paste)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			UICommon::PushDisabled(empty);

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::Deny)))
			{
				if (!empty)
				{
					ImGui::Spacing();
					DrawFormFilterTable(a_params, a_data.deny);
					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);

			UICommon::PopDisabled(disabled);

			ImGui::PopID();

			ImGui::PopID();
		}

		template <class T>
		FormFilterAction UIFormFilterWidget<T>::DrawFormFiltersHeaderTreeContextMenu(
			T&                        a_params,
			Data::configFormFilter_t& a_data)
		{
			FormFilterAction result{ FormFilterAction::None };

			ImGui::PushID("ff_header_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_MI(CommonStrings::Copy, "1"))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configFormFilter_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Paste, "2"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;

						m_onChangeFunc(
							a_params);

						result = FormFilterAction::Paste;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		FormFilterAction UIFormFilterWidget<T>::DrawFormFilterHeaderContextMenu(
			T&                     a_params,
			Data::configFormSet_t& a_data)
		{
			FormFilterAction result{ FormFilterAction::None };

			ImGui::PushID("ffh_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_ffNewEntryID = {};
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(UIWidgetCommonStrings::AddOne, "1"))
				{
					if (m_formSelector.DrawFormSelector(
							m_ffNewEntryID))
					{
						a_data.emplace(m_ffNewEntryID);

						m_onChangeFunc(a_params);

						result = FormFilterAction::Add;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_BM(UIWidgetCommonStrings::AddMultiple, "2"))
				{
					if (m_formSelector.DrawFormSelectorMulti())
					{
						bool added = false;

						for (auto& e : m_formSelector.GetSelectedEntries().getvec())
						{
							added |= a_data.emplace(e->second.formid).second;
						}

						m_formSelector.ClearSelectedEntries();

						if (added)
						{
							m_onChangeFunc(a_params);

							result = FormFilterAction::Add;
						}

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_MI(CommonStrings::Clear, "3"))
				{
					a_data.clear();

					m_onChangeFunc(a_params);
				}

				ImGui::Separator();

				if (UIL::LCG_MI(CommonStrings::Copy, "4"))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configFormSet_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Paste, "5"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;

						m_onChangeFunc(a_params);

						result = FormFilterAction::Paste;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIFormFilterWidget<T>::DrawFormFilterTable(
			T&                     a_params,
			Data::configFormSet_t& a_data)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5, 5 });

			constexpr int NUM_COLUMNS = 3;

			if (ImGui::BeginTable(
					"ff_table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0 }))
			{
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() + 2.0f);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::FormID), ImGuiTableColumnFlags_None, 75.0f);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Info), ImGuiTableColumnFlags_None, 250.0f);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				auto it = a_data.begin();

				while (it != a_data.end())
				{
					ImGui::PushID(*it);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					auto result = DrawFormFilterEntryContextMenu();
					switch (result)
					{
					case FormFilterAction::Delete:
						it = a_data.erase(it);

						m_onChangeFunc(a_params);

						break;
					}

					if (it != a_data.end())
					{
						auto form = *it;

						ImGui::TableSetColumnIndex(1);

						ImGui::Text("%.8X", form.get());

						ImGui::TableSetColumnIndex(2);

						if (auto formInfo = LookupForm(form))
						{
							if (auto typeDesc = UIFormTypeSelectorWidget::form_type_to_desc(formInfo->form.type))
							{
								ImGui::Text(
									"[%s] %s",
									typeDesc,
									formInfo->form.name.c_str());
							}
							else
							{
								ImGui::Text(
									"[%hhu] %s",
									formInfo->form.type,
									formInfo->form.name.c_str());
							}
						}

						++it;
					}

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		template <class T>
		FormFilterAction UIFormFilterWidget<T>::DrawFormFilterEntryContextMenu()
		{
			FormFilterAction result{ FormFilterAction::None };

			ImGui::PushID("ff_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			//ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_MI(CommonStrings::Delete, "1"))
				{
					result = FormFilterAction::Delete;
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UIFormFilterWidget<T>::DrawProfileSelectorCombo(
			T&                        a_params,
			Data::configFormFilter_t& a_data)
		{
			auto& data = GlobalProfileManager::GetSingleton<FormFilterProfile>().Data();

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Profile, "selector_combo"),
					a_data.profile.name.c_str(),
					ImGuiComboFlags_HeightLarge))
			{
				bool selectedNew = false;

				for (const auto& e : data.getvec())
				{
					ImGui::PushID(e);

					bool selected = a_data.profile.name == e->first;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
						{
							ImGui::SetScrollHereY();
						}
					}

					if (ImGui::Selectable(
							UIL::LMKID<3>(e->second.Name().c_str(), "1"),
							selected))
					{
						a_data.profile.name = e->first;
						selectedNew         = true;
					}

					ImGui::PopID();
				}

				if (selectedNew)
				{
					m_onChangeFunc(a_params);
				}

				ImGui::EndCombo();
			}
		}

	}
}