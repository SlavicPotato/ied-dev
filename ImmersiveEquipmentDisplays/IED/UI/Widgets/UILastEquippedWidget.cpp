#include "pch.h"

#include "UILastEquippedWidget.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/Custom/Widgets/UICustomEditorStrings.h"

namespace IED
{
	namespace UI
	{
		UILastEquippedWidget::UILastEquippedWidget(Controller& a_controller) :
			UIEquipmentOverrideConditionsWidget(a_controller)
		{
		}

		void UILastEquippedWidget::DrawLastEquippedPanel(
			Data::configLastEquipped_t& a_data,
			update_func_t               a_updateFunc)
		{
			ImGui::PushID("leq_panel");

			const auto r = DrawEquipmentOverrideConditionHeaderContextMenu(
				a_data.filterConditions.list,
				a_updateFunc);

			const bool empty = a_data.filterConditions.list.empty();

			if (!empty)
			{
				if (r == UIEquipmentOverrideAction::PasteOver ||
				    r == UIEquipmentOverrideAction::Insert)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			UICommon::PushDisabled(empty);

			if (ImGui::TreeNodeEx(
					"flt_tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					UIL::LS(UICustomEditorString::LastEquippedFilterCond)))
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawEquipmentOverrideEntryConditionTable(
						a_data.filterConditions,
						false,
						a_updateFunc);
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);

			if (DrawBipedObjectListTree(
					"bol_tree",
					a_data.bipedSlots,
					[&] {
						ImGui::Columns(2, nullptr, false);

						bool result = ImGui::CheckboxFlagsT(
							UIL::LS(UICustomEditorString::DisableIfOccupied, "1"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::LastEquippedFlags::kDisableIfSlotOccupied));

						result |= ImGui::CheckboxFlagsT(
							UIL::LS(UICustomEditorString::PrioritizeRecentSlots, "2"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::LastEquippedFlags::kPrioritizeRecentBipedSlots));

						ImGui::NextColumn();

						const bool d = a_data.flags.test(Data::LastEquippedFlags::kDisableIfSlotOccupied);

						UICommon::PushDisabled(d);

						result |= ImGui::CheckboxFlagsT(
							UIL::LS(UICustomEditorString::SkipOccupiedSlots, "3"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::LastEquippedFlags::kSkipOccupiedBipedSlots));

						UICommon::PopDisabled(d);

						ImGui::Columns();

						ImGui::Spacing();

						return result;
					}))
			{
				a_updateFunc();
			}

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UICustomEditorString::FallBackToSlotted, "ctl_1"),
					stl::underlying(std::addressof(a_data.flags.value)),
					stl::underlying(Data::LastEquippedFlags::kFallBackToSlotted)))
			{
				a_updateFunc();
			}

			if (a_data.flags.test(Data::LastEquippedFlags::kFallBackToSlotted))
			{
				if (DrawObjectSlotListTree(
						"esl_tree",
						a_data.slots,
						[&] {
							bool result = false;

							ImGui::Columns(2, nullptr, false);

							result |= ImGui::CheckboxFlagsT(
								UIL::LS(UICustomEditorString::PrioritizeRecentSlots, "1"),
								stl::underlying(std::addressof(a_data.flags.value)),
								stl::underlying(Data::LastEquippedFlags::kPrioritizeRecentDisplaySlots));

							ImGui::NextColumn();

							result |= ImGui::CheckboxFlagsT(
								UIL::LS(UICustomEditorString::SkipOccupiedSlots, "2"),
								stl::underlying(std::addressof(a_data.flags.value)),
								stl::underlying(Data::LastEquippedFlags::kSkipOccupiedDisplaySlots));

							ImGui::Columns();

							ImGui::Spacing();

							return result;
						}))
				{
					a_updateFunc();
				}
			}

			ImGui::PopID();
		}
	}
}