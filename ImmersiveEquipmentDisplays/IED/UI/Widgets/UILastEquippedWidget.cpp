#include "pch.h"

#include "UILastEquippedWidget.h"

#include "UIObjectTypeSelectorWidget.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/Custom/Widgets/UICustomEditorStrings.h"

namespace IED
{
	namespace UI
	{
		UILastEquippedWidget::UILastEquippedWidget(Controller& a_controller) :
			UIBipedObjectList(),
			UIEquipmentOverrideConditionsWidget(a_controller)
		{
		}

		void UILastEquippedWidget::DrawLastEquippedPanel(
			Data::configLastEquipped_t& a_data,
			update_func_t               a_updateFunc)
		{
			ImGui::PushID("leq_panel");

			const auto r = DrawEquipmentOverrideConditionHeaderContextMenu(
				a_data.filterConditions,
				a_updateFunc);

			bool empty = a_data.filterConditions.empty();

			if (!empty)
			{
				if (r == BaseConfigEditorAction::PasteOver ||
				    r == BaseConfigEditorAction::Insert)
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

			if (DrawBipedObjectTree(
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
							stl::underlying(Data::LastEquippedFlags::kPrioritizeRecentSlots));

						ImGui::NextColumn();

						bool d = a_data.flags.test(Data::LastEquippedFlags::kDisableIfSlotOccupied);

						UICommon::PushDisabled(d);

						result |= ImGui::CheckboxFlagsT(
							UIL::LS(UICustomEditorString::SkipOccupiedSlots, "3"),
							stl::underlying(std::addressof(a_data.flags.value)),
							stl::underlying(Data::LastEquippedFlags::kSkipOccupiedSlots));

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
				if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
					UIL::LS(CommonStrings::Slot, "ctl_2"),
					a_data.slot))
				{
					a_updateFunc();
				}
			}

			ImGui::PopID();
		}
	}
}