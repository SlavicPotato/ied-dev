#pragma once

#include "IED/ConfigInventory.h"

#include "../UIFormLookupInterface.h"
#include "Form/UIFormPickerWidget.h"
#include "UIFormTypeSelectorWidget.h"
#include "UIPopupToggleButtonWidget.h"
#include "UIWidgetsCommon.h"

namespace IED
{
	class Controller;

	namespace UI
	{

		enum class ExtraItemsAction : std::uint32_t
		{
			None,
			Add
		};

		template <class Tp>
		class UIExtraItemsWidget :
			public UIFormLookupInterface
		{
		public:
			UIExtraItemsWidget(
				Controller& a_controller) noexcept :
				UIFormLookupInterface(a_controller)
			{
			}

		protected:
			bool DrawExtraItemsImpl(
				const Tp&  a_params,
				const bool a_disabled);

		private:
			std::pair<bool, ExtraItemsAction> DrawExtraItemsHeaderContextMenu(
				const Tp& a_params);

			bool DrawExtraItemsTable(
				const Tp& a_params);

			virtual Data::configInventory_t& GetInventoryConfig(const Tp& a_params) = 0;
			virtual UIFormPickerWidget&      GetFormPicker()                        = 0;

			Game::FormID m_fsNew;
		};

		template <class Tp>
		bool UIExtraItemsWidget<Tp>::DrawExtraItemsImpl(
			const Tp&  a_params,
			const bool a_disabled)
		{
			bool result = false;

			auto& data = GetInventoryConfig(a_params);

			ImGui::PushID("extra_items");

			const auto hresult = DrawExtraItemsHeaderContextMenu(a_params);
			result |= hresult.first;

			bool treeDisabled = a_disabled || data.extraItems.empty();

			if (!treeDisabled && !a_disabled)
			{
				if (hresult.second == ExtraItemsAction::Add)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			UICommon::PushDisabled(treeDisabled);

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(UIWidgetCommonStrings::AdditionalItems)))
			{
				if (!treeDisabled)
				{
					ImGui::Spacing();

					if (ImGui::CheckboxFlagsT(
							UIL::LS(UIWidgetCommonStrings::SelectRandomForm, "hctl_1"),
							stl::underlying(std::addressof(data.flags.value)),
							stl::underlying(Data::InventoryFlags::kSelectInvRandom)))
					{
						result = true;
					}

					ImGui::Spacing();

					result |= DrawExtraItemsTable(a_params);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(treeDisabled);

			ImGui::PopID();

			return result;
		}

		template <class Tp>
		std::pair<bool, ExtraItemsAction> UIExtraItemsWidget<Tp>::DrawExtraItemsHeaderContextMenu(
			const Tp& a_params)
		{
			std::pair<bool, ExtraItemsAction> result{ false, ExtraItemsAction ::None };

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_fsNew = {};
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(UIWidgetCommonStrings::AddOne, "1"))
				{
					if (GetFormPicker().DrawFormSelector(m_fsNew))
					{
						auto& data = GetInventoryConfig(a_params);

						if (std::find(
								data.extraItems.begin(),
								data.extraItems.end(),
								m_fsNew) == data.extraItems.end())
						{
							data.extraItems.emplace_back(m_fsNew);

							result = { true, ExtraItemsAction::Add };

							m_fsNew = {};
						}

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_BM(UIWidgetCommonStrings::AddMultiple, "2"))
				{
					auto& fp = GetFormPicker();

					if (fp.DrawFormSelectorMulti())
					{
						auto& data = GetInventoryConfig(a_params);

						bool added = false;

						for (auto& e : fp.GetSelectedEntries().getvec())
						{
							if (std::find(
									data.extraItems.begin(),
									data.extraItems.end(),
									e->second.formid) == data.extraItems.end())
							{
								data.extraItems.emplace_back(e->second.formid);
								added = true;
							}
						}

						fp.ClearSelectedEntries();

						if (added)
						{
							result = { true, ExtraItemsAction::Add };
						}

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_MI(UIWidgetCommonStrings::ClearAll, "3"))
				{
					auto& data = GetInventoryConfig(a_params);

					data.extraItems.clear();

					result = { true, ExtraItemsAction::None };
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class Tp>
		bool UIExtraItemsWidget<Tp>::DrawExtraItemsTable(
			const Tp& a_params)
		{
			bool result = false;

			auto& data = GetInventoryConfig(a_params);

			if (data.extraItems.empty())
			{
				return result;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.f, 4.f });

			constexpr int NUM_COLUMNS = 3;

			if (ImGui::BeginTable(
					"table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.f }))
			{
				auto w =
					((ImGui::GetFontSize() + ImGui::GetStyle().ItemInnerSpacing.x) * 2.0f) + 2.0f +
					ImGui::CalcTextSize("X", nullptr, true).x + (4.0f * 2.0f + 2.0f);

				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, w);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::FormID), ImGuiTableColumnFlags_None, 75.0f);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Info), ImGuiTableColumnFlags_None, 250.0f);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				auto it = data.extraItems.begin();

				int i = 0;

				while (it != data.extraItems.end())
				{
					ImGui::PushID(i);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

					if (ImGui::Button("X"))
					{
						it = data.extraItems.erase(it);

						result = true;
					}

					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					if (ImGui::ArrowButton("up", ImGuiDir_Up))
					{
						if (it != data.extraItems.end())
						{
							if (IterSwap(data.extraItems, it, SwapDirection::Up))
							{
								result = true;
							}
						}
					}

					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					if (ImGui::ArrowButton("down", ImGuiDir_Down))
					{
						if (it != data.extraItems.end())
						{
							if (IterSwap(data.extraItems, it, SwapDirection::Down))
							{
								result = true;
							}
						}
					}

					ImGui::PopStyleVar();

					if (it != data.extraItems.end())
					{
						ImGui::TableSetColumnIndex(1);

						ImGui::Text("%.8X", it->get());

						ImGui::TableSetColumnIndex(2);

						if (auto formInfo = this->LookupForm(*it))
						{
							if (auto typeDesc = UIFormTypeSelectorWidget::form_type_to_desc(formInfo->form.type))
							{
								ImGui::Text("[%s] %s", typeDesc, formInfo->form.name.c_str());
							}
							else
							{
								ImGui::Text("[%hhu] %s", formInfo->form.type, formInfo->form.name.c_str());
							}
						}

						++it;
						i++;
					}

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();

			return result;
		}

	}
}