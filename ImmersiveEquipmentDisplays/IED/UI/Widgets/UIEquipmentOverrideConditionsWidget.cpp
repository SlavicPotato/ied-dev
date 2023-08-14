#include "pch.h"

#include "UIBipedObjectSelectorWidget.h"
#include "UIComparisonOperatorSelector.h"
#include "UIConditionExtraSelectorWidget.h"
#include "UIEquipmentOverrideConditionsWidget.h"
#include "UIFormTypeSelectorWidget.h"
#include "UIObjectTypeSelectorWidget.h"
#include "UIPopupToggleButtonWidget.h"

#include "IED/UI/UIClipboard.h"
#include "IED/UI/UIFormBrowserCommonFilters.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/NodeOverride/Widgets/UINodeOverrideEditorWidgetStrings.h"

#include "UIBaseConfigWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		UIEquipmentOverrideConditionsWidget::UIEquipmentOverrideConditionsWidget(
			Controller& a_controller) :
			m_condParamEditor(a_controller)
		{
			m_condParamEditor.SetExtraInterface(this);
		}

		auto UIEquipmentOverrideConditionsWidget::DrawEquipmentOverrideConditionContextMenu(
			bool a_header)
			-> UIEquipmentOverrideResult
		{
			UIEquipmentOverrideResult result;

			ImGui::PushID("eo_entry_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_aoNewEntryID        = {};
				m_aoNewEntryKWID      = {};
				m_aoNewEntryRaceID    = {};
				m_aoNewEntryActorID   = {};
				m_aoNewEntryNPCID     = {};
				m_aoNewEntryGlobID    = {};
				m_aoNewEntryFactionID = {};
				m_aoNewEntryEffectID  = {};
				m_aoNewEntryPerkID    = {};
				m_ooNewBiped          = BIPED_OBJECT::kNone;
				m_aoNewSlot           = Data::ObjectSlotExtra::kNone;
				m_ooNewExtraCond      = Data::ExtraConditionType::kNone;
				ClearDescriptionPopupBuffer();
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (!a_header)
			{
				if (ImGui::ArrowButton("up", ImGuiDir_Up))
				{
					result.action = UIEquipmentOverrideAction::Swap;
					result.dir    = SwapDirection::Up;
				}

				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

				if (ImGui::ArrowButton("down", ImGuiDir_Down))
				{
					result.action = UIEquipmentOverrideAction::Swap;
					result.dir    = SwapDirection::Down;
				}
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Add, "1"))
				{
					if (UIL::LCG_BM(CommonStrings::Type, "2"))
					{
						if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector("##ss", m_aoNewSlot))
						{
							result.action    = UIEquipmentOverrideAction::Insert;
							result.slot      = m_aoNewSlot;
							result.entryType = Data::EquipmentOverrideConditionType::Type;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(CommonStrings::Form, "3"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Form);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_aoNewEntryID))
						{
							if (m_aoNewEntryID)
							{
								result.action    = UIEquipmentOverrideAction::Insert;
								result.form      = m_aoNewEntryID;
								result.entryType = Data::EquipmentOverrideConditionType::Form;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(CommonStrings::Keyword, "4"))
					{
						if (m_condParamEditor.GetKeywordPicker().DrawFormSelector(
								m_aoNewEntryKWID))
						{
							if (m_aoNewEntryKWID)
							{
								result.action    = UIEquipmentOverrideAction::Insert;
								result.form      = m_aoNewEntryKWID;
								result.entryType = Data::EquipmentOverrideConditionType::Keyword;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(UIWidgetCommonStrings::BipedSlot, "5"))
					{
						if (UIBipedObjectSelectorWidget::DrawBipedObjectSelector(
								UIL::LS(CommonStrings::Biped, "bp"),
								m_ooNewBiped))
						{
							result.action    = UIEquipmentOverrideAction::Insert;
							result.biped     = m_ooNewBiped;
							result.entryType = Data::EquipmentOverrideConditionType::BipedSlot;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Hand, "5a"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Hand;
					}

					if (UIL::LCG_MI(CommonStrings::Actor, "6"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Actor;
					}

					if (UIL::LCG_MI(CommonStrings::NPC, "7"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::NPC;
					}

					if (UIL::LCG_MI(CommonStrings::Race, "8"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Race;
					}

					if (UIL::LCG_MI(CommonStrings::Furniture, "9"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Furniture;

						ImGui::CloseCurrentPopup();
					}

					if (UIL::LCG_BM(CommonStrings::Quest, "A"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Quest);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_aoNewEntryID))
						{
							if (m_aoNewEntryID)
							{
								result.action    = UIEquipmentOverrideAction::Insert;
								result.form      = m_aoNewEntryID;
								result.entryType = Data::EquipmentOverrideConditionType::Quest;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Location, "B"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Location;
					}

					if (UIL::LCG_MI(CommonStrings::Worldspace, "C"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Worldspace;
					}

					if (UIL::LCG_MI(CommonStrings::Package, "D"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Package;
					}

					if (UIL::LCG_MI(CommonStrings::Weather, "E"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Weather;
					}

					if (UIL::LCG_BM(CommonStrings::Global, "F"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Global);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_aoNewEntryGlobID))
						{
							if (m_aoNewEntryGlobID)
							{
								result.action    = UIEquipmentOverrideAction::Insert;
								result.form      = m_aoNewEntryGlobID;
								result.entryType = Data::EquipmentOverrideConditionType::Global;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(UIWidgetCommonStrings::Mounting, "G"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Mounting;
					}

					if (UIL::LCG_MI(UIWidgetCommonStrings::Mounted, "H"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Mounted;
					}

					if (UIL::LCG_MI(UIWidgetCommonStrings::CurrentItem, "I"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Presence;
					}

					if (UIL::LCG_MI(CommonStrings::Idle, "J"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Idle;
					}

					if (UIL::LCG_MI(CommonStrings::Skeleton, "K"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Skeleton;
					}

					if (UIL::LCG_BM(CommonStrings::Faction, "L"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Faction);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_aoNewEntryFactionID))
						{
							if (m_aoNewEntryFactionID)
							{
								result.action    = UIEquipmentOverrideAction::Insert;
								result.form      = m_aoNewEntryFactionID;
								result.entryType = Data::EquipmentOverrideConditionType::Faction;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Effect, "M"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Effect;
					}

					if (UIL::LCG_BM(CommonStrings::Variable, "N"))
					{
						if (DrawDescriptionPopup())
						{
							result.action = UIEquipmentOverrideAction::Insert;
							result.desc   = GetDescriptionPopupBuffer();

							result.entryType = Data::EquipmentOverrideConditionType::Variable;

							ClearDescriptionPopupBuffer();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(CommonStrings::Perk, "O"))
					{
						UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType::Perk);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_aoNewEntryPerkID))
						{
							if (m_aoNewEntryPerkID)
							{
								result.action    = UIEquipmentOverrideAction::Insert;
								result.form      = m_aoNewEntryPerkID;
								result.entryType = Data::EquipmentOverrideConditionType::Perk;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Cell, "P"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Cell;
					}

					if (UIL::LCG_BM(CommonStrings::Extra, "X"))
					{
						if (UIConditionExtraSelectorWidget::DrawExtraConditionSelector(
								m_ooNewExtraCond))
						{
							result.action    = UIEquipmentOverrideAction::Insert;
							result.excond    = m_ooNewExtraCond;
							result.entryType = Data::EquipmentOverrideConditionType::Extra;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Group, "Y"))
					{
						result.action    = UIEquipmentOverrideAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Group;

						ImGui::CloseCurrentPopup();
					}

					ImGui::Separator();

					auto clipData = UIClipboard::Get<Data::equipmentOverrideCondition_t>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::Paste, "Z"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = UIEquipmentOverrideAction::Paste;
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_MI(CommonStrings::Delete, "2"))
				{
					result.action = UIEquipmentOverrideAction::Delete;
				}

				ImGui::Separator();

				if (a_header)
				{
					if (UIL::LCG_MI(CommonStrings::Copy, "3"))
					{
						result.action = UIEquipmentOverrideAction::Copy;
					}

					auto clipData = UIClipboard::Get<Data::equipmentOverrideConditionList_t>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::PasteOver, "4"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = UIEquipmentOverrideAction::PasteOver;
					}
				}
				else
				{
					if (UIL::LCG_MI(CommonStrings::Copy, "3"))
					{
						result.action = UIEquipmentOverrideAction::Copy;
					}

					auto clipData = UIClipboard::Get<Data::equipmentOverrideCondition_t>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::PasteOver, "5"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = UIEquipmentOverrideAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		bool UIEquipmentOverrideConditionsWidget::DrawFormCountExtraSegment(
			Data::equipmentOverrideCondition_t* a_match)
		{
			ImGui::PushID("fcnt_extra");

			bool result = false;

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Count, "0"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

			bool disabled = !a_match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag1);

			UICommon::PushDisabled(disabled);

			ImGui::SameLine();

			ImGui::PushItemWidth(ImGui::GetFontSize() * 6.5f);

			result |= UIComparisonOperatorSelector::DrawComparisonOperatorSelector(a_match->compOperator);

			ImGui::PopItemWidth();

			ImGui::SameLine();

			result |= ImGui::InputScalar(
				"##1",
				ImGuiDataType_U32,
				std::addressof(a_match->count),
				nullptr,
				nullptr,
				"%u",
				ImGuiInputTextFlags_EnterReturnsTrue);

			UICommon::PopDisabled(disabled);

			ImGui::PopID();

			return result;
		}

		bool UIEquipmentOverrideConditionsWidget::DrawConditionParamExtra(void* a_p1, const void*)
		{
			auto match = static_cast<Data::equipmentOverrideCondition_t*>(a_p1);

			ImGui::PushID("match_param_extra");

			bool result = false;

			switch (match->flags.bf().type)
			{
			case Data::EquipmentOverrideConditionType::Type:

				if (match->flags.test_any(Data::EquipmentOverrideConditionFlags::kMatchMaskAny))
				{
					const bool disabled =
						!match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag1);

					UICommon::PushDisabled(disabled);

					result |= ImGui::CheckboxFlagsT(
						"!##t_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch3));

					UICommon::PopDisabled(disabled);

					ImGui::SameLine();

					result |= ImGui::CheckboxFlagsT(
						UIL::LS(UINodeOverrideEditorWidgetStrings::IsBolt, "t_2"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));
				}

				[[fallthrough]];

			case Data::EquipmentOverrideConditionType::Form:
			case Data::EquipmentOverrideConditionType::Keyword:

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(CommonStrings::Equipped, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchEquipped));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(CommonStrings::Or, "2"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchCategoryOperOR));

				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("%s:", UIL::LS(CommonStrings::Displayed));
				ImGui::SameLine();

				if (ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::All, "3"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchEquipmentSlots)))
				{
					if (match->flags.test(Data::EquipmentOverrideConditionFlags::kMatchEquipmentSlots))
					{
						match->flags.clear(Data::EquipmentOverrideConditionFlags::kMatchThisItem);
					}

					result = true;
				}

				ImGui::SameLine();

				if (ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::This, "4"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchThisItem)))
				{
					if (match->flags.test(Data::EquipmentOverrideConditionFlags::kMatchThisItem))
					{
						match->flags.clear(Data::EquipmentOverrideConditionFlags::kMatchEquipmentSlots);
					}

					result = true;
				}

				UITipsInterface::DrawTip(UITip::EquippedConditionsEquipment);

				if (match->flags.bf().type == Data::EquipmentOverrideConditionType::Form &&
				    !match->flags.test_any(Data::EquipmentOverrideConditionFlags::kMatchMaskAny))
				{
					ImGui::Spacing();

					result |= DrawFormCountExtraSegment(match);
				}

				break;

			case Data::EquipmentOverrideConditionType::Furniture:

				DrawConditionParamExtra_Furniture(match, result);

				break;

			case Data::EquipmentOverrideConditionType::BipedSlot:

				DrawConditionParamExtra_BipedSlot(match, result);

				break;

			case Data::EquipmentOverrideConditionType::Location:

				DrawConditionParamExtra_Location(match, result);

				break;

			case Data::EquipmentOverrideConditionType::Worldspace:

				DrawConditionParamExtra_Worldspace(match, result);

				break;

			case Data::EquipmentOverrideConditionType::Race:

				DrawConditionParamExtra_Race(match, result);

				break;

			case Data::EquipmentOverrideConditionType::Presence:
				{
					result |= ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::IsAvailable, "0"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag2));

					ImGui::SameLine();

					result |= ImGui::CheckboxFlagsT(
						UIL::LS(UINodeOverrideEditorWidgetStrings::IsBolt, "1"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag3));

					ImGui::SameLine();

					result |= ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::IsPlayable, "2"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag4));

					result |= ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::IsFavorited, "4"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag5));

					ImGui::Separator();

					if (!match->flags.test_any(Data::EquipmentOverrideConditionFlags::kExtraFlag2))
					{
						if (ImGui::CheckboxFlagsT(
								UIL::LS(CommonStrings::Equipped, "A"),
								stl::underlying(std::addressof(match->flags.value)),
								stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchEquipped)))
						{
							match->ui32a = static_cast<std::uint32_t>(-1);
							match->ui32b = 0;
							result       = true;
						}

						if (match->flags.test(Data::EquipmentOverrideConditionFlags::kMatchEquipped))
						{
							result |= UIBipedObjectSelectorWidget::DrawBipedObjectSelector(
								UIL::LS(CommonStrings::Biped, "B"),
								match->bipedSlot,
								true);

							if (match->bipedSlot == BIPED_OBJECT::kNone)
							{
								ImGui::AlignTextToFramePadding();
								ImGui::TextUnformatted(UIL::LS(CommonStrings::Hand));
								ImGui::SameLine();

								if (ImGui::RadioButton(
										UIL::LS(CommonStrings::Any, "C"),
										match->flags.bf().presenceEquipedHandMatch == Data::PresenceEquippedHandMatch::kEither))
								{
									match->flags.bf().presenceEquipedHandMatch = Data::PresenceEquippedHandMatch::kEither;
									result                                     = true;
								}

								ImGui::SameLine();

								if (ImGui::RadioButton(
										UIL::LS(CommonStrings::Left, "D"),
										match->flags.bf().presenceEquipedHandMatch == Data::PresenceEquippedHandMatch::kLeft))
								{
									match->flags.bf().presenceEquipedHandMatch = Data::PresenceEquippedHandMatch::kLeft;
									result                                     = true;
								}

								ImGui::SameLine();

								if (ImGui::RadioButton(
										UIL::LS(CommonStrings::Right, "E"),
										match->flags.bf().presenceEquipedHandMatch == Data::PresenceEquippedHandMatch::kRight))
								{
									match->flags.bf().presenceEquipedHandMatch = Data::PresenceEquippedHandMatch::kRight;
									result                                     = true;
								}
							}
							else
							{
								result |= ImGui::CheckboxFlagsT(
									UIL::LS(UIWidgetCommonStrings::GeometryVisible, "F"),
									stl::underlying(std::addressof(match->flags.value)),
									stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag8));
							}
						}

						ImGui::Separator();

						result |= ImGui::CheckboxFlagsT(
							UIL::LS(CommonStrings::Displayed, "F"),
							stl::underlying(std::addressof(match->flags.value)),
							stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchEquipmentSlots));

						if (match->flags.test(Data::EquipmentOverrideConditionFlags::kMatchEquipmentSlots))
						{
							result |= UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
								UIL::LS(CommonStrings::Slot, "G"),
								match->slot,
								true);
						}

						ImGui::Separator();

						if (!match->flags.test_any(Data::EquipmentOverrideConditionFlags::kMatchMaskEquippedAndSlots))
						{
							result |= DrawFormCountExtraSegment(match);

							ImGui::Separator();
						}
					}

					result |= ImGui::CheckboxFlagsT(
						"!##8",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();

					m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::EquipSlot));
					m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);

					result |= m_condParamEditor.GetFormPicker().DrawFormPicker(
						"9",
						static_cast<Localization::StringID>(UIFormBrowserStrings::EquipSlot),
						match->form);

					ImGui::Spacing();

					ImGui::Text("%s:", UIL::LS(CommonStrings::Info));
					ImGui::SameLine();
					UITipsInterface::DrawTip(UITip::Presence);
				}

				break;

			case Data::EquipmentOverrideConditionType::Idle:

				DrawConditionParamExtra_Idle(match, result);

				break;

			case Data::EquipmentOverrideConditionType::Skeleton:

				DrawConditionParamExtra_Skeleton(match, result);

				break;

			case Data::EquipmentOverrideConditionType::Mounting:

				DrawConditionParamExtra_Mounting(match, result);

				break;

			case Data::EquipmentOverrideConditionType::Cell:

				DrawConditionParamExtra_Cell(match, result);

				break;

			case Data::EquipmentOverrideConditionType::Hand:

				DrawConditionParamExtra_Hand(match, m_condParamEditor, result);

				break;

			case Data::EquipmentOverrideConditionType::NPC:

				DrawConditionParamExtra_NPC(match, result);

				break;

			case Data::EquipmentOverrideConditionType::Extra:

				DrawConditionParamExtra_Extra(match, result);

				break;
			}

			ImGui::PopID();

			return result;
		}

		bool UIEquipmentOverrideConditionsWidget::DrawConditionItemExtra(
			ConditionParamItem           a_item,
			ConditionParamItemExtraArgs& a_args)
		{
			bool result = false;

			ImGui::PushID("match_item_extra");

			DrawConditionItemExtra_All(a_item, a_args, result);

			ImGui::PopID();

			return result;
		}

		void UIEquipmentOverrideConditionsWidget::OnConditionItemChange(
			ConditionParamItem                    a_item,
			const ConditionParamItemOnChangeArgs& a_args)
		{
			OnConditionItemChangeImpl(a_item, a_args);
		}

		void UIEquipmentOverrideConditionsWidget::OnConditionListDrawn(
			const Data::equipmentOverrideConditionSet_t& a_condition)
		{
		}

		UIEquipmentOverrideAction UIEquipmentOverrideConditionsWidget::DrawEquipmentOverrideConditionHeaderContextMenu(
			Data::equipmentOverrideConditionList_t& a_entry,
			update_func_t                           a_updFunc)
		{
			UIEquipmentOverrideAction action{ UIEquipmentOverrideAction ::None };

			const auto result = DrawEquipmentOverrideConditionContextMenu(true);

			switch (result.action)
			{
			case UIEquipmentOverrideAction::Insert:
				{
					switch (result.entryType)
					{
					case Data::EquipmentOverrideConditionType::Type:
						if (result.slot != Data::ObjectSlotExtra::kNone)
						{
							a_entry.emplace_back(
								result.slot);

							action = result.action;

							a_updFunc();
						}
						break;
					case Data::EquipmentOverrideConditionType::Form:
					case Data::EquipmentOverrideConditionType::Keyword:
					case Data::EquipmentOverrideConditionType::Quest:
					case Data::EquipmentOverrideConditionType::Global:
					case Data::EquipmentOverrideConditionType::Faction:
					case Data::EquipmentOverrideConditionType::Perk:
						if (result.form)
						{
							a_entry.emplace_back(
								result.entryType,
								result.form);

							action = result.action;

							a_updFunc();
						}
						break;
					case Data::EquipmentOverrideConditionType::Actor:
					case Data::EquipmentOverrideConditionType::NPC:
					case Data::EquipmentOverrideConditionType::Race:
					case Data::EquipmentOverrideConditionType::Furniture:
					case Data::EquipmentOverrideConditionType::Group:
					case Data::EquipmentOverrideConditionType::Location:
					case Data::EquipmentOverrideConditionType::Worldspace:
					case Data::EquipmentOverrideConditionType::Package:
					case Data::EquipmentOverrideConditionType::Weather:
					case Data::EquipmentOverrideConditionType::Mounting:
					case Data::EquipmentOverrideConditionType::Mounted:
					case Data::EquipmentOverrideConditionType::Presence:
					case Data::EquipmentOverrideConditionType::Idle:
					case Data::EquipmentOverrideConditionType::Skeleton:
					case Data::EquipmentOverrideConditionType::Effect:
					case Data::EquipmentOverrideConditionType::Cell:
					case Data::EquipmentOverrideConditionType::Hand:

						a_entry.emplace_back(
							result.entryType);

						action = result.action;

						a_updFunc();

						break;
					case Data::EquipmentOverrideConditionType::BipedSlot:
						if (result.biped != BIPED_OBJECT::kNone)
						{
							a_entry.emplace_back(
								result.biped);

							action = result.action;

							a_updFunc();
						}
						break;
					case Data::EquipmentOverrideConditionType::Variable:

						a_entry.emplace_back(result.entryType, result.desc);

						action = result.action;

						a_updFunc();

						break;
					case Data::EquipmentOverrideConditionType::Extra:
						if (result.excond != Data::ExtraConditionType::kNone)
						{
							a_entry.emplace_back(
								result.excond);

							action = result.action;

							a_updFunc();
						}
					}
				}

				ImGui::SetNextItemOpen(true);

				break;

			case UIEquipmentOverrideAction::Copy:
				UIClipboard::Set(a_entry);
				break;

			case UIEquipmentOverrideAction::Paste:
				if (auto clipData = UIClipboard::Get<Data::equipmentOverrideCondition_t>())
				{
					a_entry.emplace_back(*clipData);

					a_updFunc();

					action = result.action;
				}
				break;
			case UIEquipmentOverrideAction::PasteOver:
				if (auto clipData = UIClipboard::Get<Data::equipmentOverrideConditionList_t>())
				{
					a_entry = *clipData;

					a_updFunc();

					action = result.action;
				}
				break;
			case UIEquipmentOverrideAction::Delete:
				a_entry.clear();

				action = result.action;

				a_updFunc();

				break;
			}

			return action;
		}

		void UIEquipmentOverrideConditionsWidget::DrawEquipmentOverrideConditionTree(
			Data::equipmentOverrideConditionSet_t& a_entry,
			update_func_t                           a_updFunc,
			Localization::StringID                  a_title)
		{
			ImGui::PushID("cond_tree_area");

			const auto r = DrawEquipmentOverrideConditionHeaderContextMenu(
				a_entry.list,
				a_updFunc);

			bool empty = a_entry.list.empty();

			if (!empty)
			{
				switch (r)
				{
				case UIEquipmentOverrideAction::Paste:
				case UIEquipmentOverrideAction::PasteOver:
				case UIEquipmentOverrideAction::Insert:
					ImGui::SetNextItemOpen(true);
					break;
				}
			}

			UICommon::PushDisabled(empty);

			if (ImGui::TreeNodeEx(
					"cond_tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					UIL::LS(a_title)))
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawEquipmentOverrideEntryConditionTable(
						a_entry,
						false,
						a_updFunc);
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);

			ImGui::PopID();
		}

		void UIEquipmentOverrideConditionsWidget::GetConditionListDepth(
			const Data::equipmentOverrideConditionList_t& a_in,
			std::uint32_t&                                a_result,
			std::uint32_t&                                a_offset) noexcept
		{
			for (auto& e : a_in)
			{
				if (e.flags.bf().type == Data::EquipmentOverrideConditionType::Group)
				{
					a_offset++;
					a_result = std::max(a_result, a_offset);

					GetConditionListDepth(e.group.conditions.list, a_result, a_offset);

					a_offset--;
				}
			}
		}

		void UIEquipmentOverrideConditionsWidget::DrawEquipmentOverrideEntryConditionTable(
			Data::equipmentOverrideConditionSet_t& a_entry,
			bool                                    a_isnested,
			update_func_t                           a_updFunc)
		{
			constexpr int   NUM_COLUMNS   = 5;
			constexpr float MIN_TAB_WIDTH = 320.0f;

			float           width;
			float           height     = 0.0f;
			float           innerWidth = 0.0f;
			ImGuiTableFlags flags      = ImGuiTableFlags_None;

			if (a_isnested)
			{
				ImGui::PushStyleVar(
					ImGuiStyleVar_CellPadding,
					{ 2.f, 2.f });

				width = -1.0f;
			}
			else
			{
				ImGui::PushStyleVar(
					ImGuiStyleVar_CellPadding,
					{ 5.f, 5.f });

				std::uint32_t res = 0;
				std::uint32_t off = 0;

				GetConditionListDepth(a_entry.list, res, off);

				if (res > 2)
				{
					const auto avail       = ImGui::GetContentRegionAvail().x;
					const auto wantedWidth = MIN_TAB_WIDTH + MIN_TAB_WIDTH * static_cast<float>(res);

					if (wantedWidth > avail)
					{
						flags = ImGuiTableFlags_ScrollX |
						        ImGuiTableFlags_ScrollY;

						innerWidth = wantedWidth;
						height     = 400.0f;
					}
				}

				width = -ImGui::GetFontSize();
			}

			if (ImGui::BeginTable(
					"eo_entry_match_table",
					NUM_COLUMNS,
					flags |
						ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_SizingStretchProp |
						ImGuiTableFlags_NoSavedSettings,
					{ width, height },
					innerWidth))
			{
				auto w =
					(ImGui::GetFontSize() + ImGui::GetStyle().ItemInnerSpacing.x) * 3.0f + 2.0f;

				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, w);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Type), ImGuiTableColumnFlags_None, 40.0f);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Edit), ImGuiTableColumnFlags_None, MIN_TAB_WIDTH);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::And), ImGuiTableColumnFlags_None, 15.0f);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Not), ImGuiTableColumnFlags_None, 15.0f);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				int i = 0;

				auto it = a_entry.list.begin();

				while (it != a_entry.list.end())
				{
					ImGui::PushID(i);

					ImGui::TableNextRow();

					//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });

					ImGui::TableSetColumnIndex(0);

					const auto result = DrawEquipmentOverrideConditionContextMenu(false);

					switch (result.action)
					{
					case UIEquipmentOverrideAction::Delete:
						it = a_entry.list.erase(it);
						a_updFunc();
						break;
					case UIEquipmentOverrideAction::Insert:

						switch (result.entryType)
						{
						case Data::EquipmentOverrideConditionType::Type:
							if (result.slot != Data::ObjectSlotExtra::kNone)
							{
								it = a_entry.list.emplace(
									it,
									result.slot);

								a_updFunc();
							}
							break;
						case Data::EquipmentOverrideConditionType::Form:
						case Data::EquipmentOverrideConditionType::Keyword:
						case Data::EquipmentOverrideConditionType::Quest:
						case Data::EquipmentOverrideConditionType::Global:
						case Data::EquipmentOverrideConditionType::Faction:
						case Data::EquipmentOverrideConditionType::Perk:
							if (result.form)
							{
								it = a_entry.list.emplace(
									it,
									result.entryType,
									result.form);

								a_updFunc();
							}
							break;
						case Data::EquipmentOverrideConditionType::Actor:
						case Data::EquipmentOverrideConditionType::NPC:
						case Data::EquipmentOverrideConditionType::Race:
						case Data::EquipmentOverrideConditionType::Furniture:
						case Data::EquipmentOverrideConditionType::Group:
						case Data::EquipmentOverrideConditionType::Location:
						case Data::EquipmentOverrideConditionType::Worldspace:
						case Data::EquipmentOverrideConditionType::Package:
						case Data::EquipmentOverrideConditionType::Weather:
						case Data::EquipmentOverrideConditionType::Mounting:
						case Data::EquipmentOverrideConditionType::Mounted:
						case Data::EquipmentOverrideConditionType::Presence:
						case Data::EquipmentOverrideConditionType::Idle:
						case Data::EquipmentOverrideConditionType::Skeleton:
						case Data::EquipmentOverrideConditionType::Effect:
						case Data::EquipmentOverrideConditionType::Cell:
						case Data::EquipmentOverrideConditionType::Hand:

							it = a_entry.list.emplace(
								it,
								result.entryType);

							a_updFunc();

							break;
						case Data::EquipmentOverrideConditionType::BipedSlot:

							if (result.biped != BIPED_OBJECT::kNone)
							{
								it = a_entry.list.emplace(
									it,
									result.biped);

								a_updFunc();
							}

							break;
						case Data::EquipmentOverrideConditionType::Variable:

							it = a_entry.list.emplace(it, result.entryType, result.desc);

							a_updFunc();

							break;
						case Data::EquipmentOverrideConditionType::Extra:

							if (result.excond != Data::ExtraConditionType::kNone)
							{
								it = a_entry.list.emplace(
									it,
									result.excond);

								a_updFunc();
							}

							break;
						}

						break;

					case UIEquipmentOverrideAction::Swap:

						if (IterSwap(a_entry.list, it, result.dir))
						{
							a_updFunc();
						}

						break;

					case UIEquipmentOverrideAction::Copy:

						UIClipboard::Set(*it);

						break;

					case UIEquipmentOverrideAction::Paste:

						if (auto clipData = UIClipboard::Get<Data::equipmentOverrideCondition_t>())
						{
							it = a_entry.list.emplace(
								it,
								*clipData);

							a_updFunc();
						}

						break;

					case UIEquipmentOverrideAction::PasteOver:

						if (auto clipData = UIClipboard::Get<Data::equipmentOverrideCondition_t>())
						{
							*it = *clipData;

							a_updFunc();
						}

						break;
					}

					if (it != a_entry.list.end())
					{
						auto& e = *it;

						ImGui::TableSetColumnIndex(1);

						if (e.flags.bf().type == Data::EquipmentOverrideConditionType::Group)
						{
							ImGui::TextUnformatted(UIL::LS(CommonStrings::Group));

							ImGui::TableSetColumnIndex(2);

							ImGui::PushID("cond_grp");

							DrawEquipmentOverrideConditionHeaderContextMenu(
								e.group.conditions.list,
								a_updFunc);

							DrawEquipmentOverrideEntryConditionTable(
								e.group.conditions,
								true,
								a_updFunc);

							ImGui::PopID();
						}
						else
						{
							m_condParamEditor.Reset();

							DoUpdateMatchParamAllowedTypes(e, m_condParamEditor);

							const char* tdesc;
							const char* vdesc;

							switch (e.flags.bf().type)
							{
							case Data::EquipmentOverrideConditionType::Form:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = UIL::LS(CommonStrings::Form);

								break;
							case Data::EquipmentOverrideConditionType::Type:

								m_condParamEditor.SetNext<ConditionParamItem::EquipmentSlotExtra>(
									e.slot);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::EquipmentSlotExtra);
								tdesc = UIL::LS(CommonStrings::Type);

								break;
							case Data::EquipmentOverrideConditionType::Keyword:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearKeyword);

								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								tdesc = UIL::LS(CommonStrings::Keyword);
								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Keyword);

								break;
							case Data::EquipmentOverrideConditionType::Race:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = UIL::LS(CommonStrings::Race);

								break;
							case Data::EquipmentOverrideConditionType::Actor:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = UIL::LS(CommonStrings::Actor);

								break;
							case Data::EquipmentOverrideConditionType::NPC:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = UIL::LS(CommonStrings::NPC);

								break;
							case Data::EquipmentOverrideConditionType::Furniture:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Furniture);
								tdesc = UIL::LS(CommonStrings::Furniture);

								break;
							case Data::EquipmentOverrideConditionType::BipedSlot:

								m_condParamEditor.SetNext<ConditionParamItem::BipedSlot>(
									e.bipedSlot);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::BipedSlot);
								tdesc = UIL::LS(CommonStrings::Biped);

								break;
							case Data::EquipmentOverrideConditionType::Quest:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::QuestCondType>(
									e.questCondType);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = UIL::LS(CommonStrings::Quest);

								break;
							case Data::EquipmentOverrideConditionType::Extra:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::CondExtra>(
									e.extraCondType);

								switch (e.extraCondType)
								{
								case Data::ExtraConditionType::kShoutEquipped:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::ShoutAndSpell));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
									m_condParamEditor.SetNext<ConditionParamItem::Form>(e.form.get_id());
									break;
								case Data::ExtraConditionType::kCombatStyle:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::CombatStyle));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
									m_condParamEditor.SetNext<ConditionParamItem::Form>(e.form.get_id());
									break;
								case Data::ExtraConditionType::kClass:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Class));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
									m_condParamEditor.SetNext<ConditionParamItem::Form>(e.form.get_id());
									break;
								case Data::ExtraConditionType::kTimeOfDay:
									m_condParamEditor.SetNext<ConditionParamItem::TimeOfDay>(e.timeOfDay);
									break;
								case Data::ExtraConditionType::kRandomPercent:
									m_condParamEditor.SetNext<ConditionParamItem::Percent>(e.percent);
									break;
								case Data::ExtraConditionType::kNodeMonitor:
									m_condParamEditor.SetNext<ConditionParamItem::NodeMon>(e.uid);
									break;
								case Data::ExtraConditionType::kLevel:
									m_condParamEditor.SetNext<ConditionParamItem::CompOper>(e.compOperator2);
									m_condParamEditor.SetNext<ConditionParamItem::UInt32>(e.level);
									break;
								case Data::ExtraConditionType::kDayOfWeek:
									m_condParamEditor.SetNext<ConditionParamItem::DayOfWeek>(e.dayOfWeek);
									break;
								case Data::ExtraConditionType::kLifeState:
									m_condParamEditor.SetNext<ConditionParamItem::LifeState>(e.lifeState);
									break;
								case Data::ExtraConditionType::kActorValue:
									m_condParamEditor.SetNext<ConditionParamItem::ActorValue>(e.actorValue);
									m_condParamEditor.SetNext<ConditionParamItem::CompOper>(e.compOperator2);
									m_condParamEditor.SetNext<ConditionParamItem::Float>(e.avMatch);
									break;
								case Data::ExtraConditionType::kSunAngle:
									m_condParamEditor.SetNext<ConditionParamItem::CompOper>(e.compOperator2);
									m_condParamEditor.SetNext<ConditionParamItem::SunAngle>(e.sunAngle);
									break;
								case Data::ExtraConditionType::kKeyBindState:
									m_condParamEditor.SetNext<ConditionParamItem::KeyBindID>(e.s0);
									m_condParamEditor.SetNext<ConditionParamItem::CompOper>(e.compOperator2);
									m_condParamEditor.SetNext<ConditionParamItem::UInt32>(e.keyBindState, CommonStrings::State);
									break;
								case Data::ExtraConditionType::kLightingTemplate:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::LightingTemplate));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
									m_condParamEditor.SetNext<ConditionParamItem::Form>(e.form.get_id());
									break;
								case Data::ExtraConditionType::kInteriorAmbientLightLevel:
									m_condParamEditor.SetNext<ConditionParamItem::CompOper>(e.compOperator2);
									m_condParamEditor.SetNext<ConditionParamItem::Float>(e.ambientLightLevel);
									break;
								}

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::CondExtra);
								tdesc = UIL::LS(CommonStrings::Extra);

								break;
							case Data::EquipmentOverrideConditionType::Location:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = UIL::LS(CommonStrings::Location);

								break;
							case Data::EquipmentOverrideConditionType::Worldspace:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = UIL::LS(CommonStrings::Worldspace);

								break;
							case Data::EquipmentOverrideConditionType::Package:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::PackageType>(
									e.procedureType);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::PackageType);
								tdesc = UIL::LS(CommonStrings::Package);

								break;
							case Data::EquipmentOverrideConditionType::Weather:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::WeatherClass>(
									e.weatherClass);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::WeatherClass);
								tdesc = UIL::LS(CommonStrings::Weather);

								break;
							case Data::EquipmentOverrideConditionType::Global:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::CompOper>(
									e.compOperator);
								m_condParamEditor.SetNext<ConditionParamItem::Float>(
									e.f32a);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = UIL::LS(CommonStrings::Global);

								break;
							case Data::EquipmentOverrideConditionType::Mounting:
							case Data::EquipmentOverrideConditionType::Mounted:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.form2.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Race>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr, true);
								tdesc = e.flags.bf().type == Data::EquipmentOverrideConditionType::Mounting ?
								            UIL::LS(UIWidgetCommonStrings::Mounting) :
								            UIL::LS(UIWidgetCommonStrings::Mounted);

								break;
							case Data::EquipmentOverrideConditionType::Presence:
								{
									m_condParamEditor.SetNext<ConditionParamItem::Extra>(
										e);

									auto& db = m_condParamEditor.GetDescBuffer();

									if (e.flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag2))
									{
										stl::snprintf(
											db,
											"%s",
											UIL::LS(UIWidgetCommonStrings::IsAvailable));
									}
									else
									{
										if (e.flags.test(Data::EquipmentOverrideConditionFlags::kMatchMaskEquippedAndSlots))
										{
											stl::snprintf(
												db,
												"%s/%s",
												UIL::LS(CommonStrings::Equipped),
												UIL::LS(CommonStrings::Displayed));
										}
										else if (e.flags.test(
													 Data::EquipmentOverrideConditionFlags::kMatchEquipped))
										{
											stl::snprintf(
												db,
												"%s",
												UIL::LS(CommonStrings::Equipped));
										}
										else if (e.flags.test(
													 Data::EquipmentOverrideConditionFlags::kMatchEquipmentSlots))
										{
											stl::snprintf(
												db,
												"%s",
												UIL::LS(CommonStrings::Displayed));
										}
										else
										{
											if (e.flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag1))
											{
												stl::snprintf(
													db,
													"%s %s %u",
													UIL::LS(CommonStrings::Count),
													UIComparisonOperatorSelector::comp_operator_to_desc(e.compOperator),
													e.count);
											}
											else
											{
												stl::snprintf(
													db,
													"%s",
													UIL::LS(UIBaseConfigString::InventoryCheck));
											}
										}
									}

									vdesc = db;
									tdesc = UIL::LS(UIWidgetCommonStrings::CurrentItem);
								}
								break;

							case Data::EquipmentOverrideConditionType::Idle:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = UIL::LS(CommonStrings::Idle);

								break;

							case Data::EquipmentOverrideConditionType::Skeleton:
								{
									m_condParamEditor.SetNext<ConditionParamItem::Extra>(
										e);

									auto& buffer = m_condParamEditor.GetDescBuffer();

									if (e.flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag1))
									{
										stl::snprintf(buffer, "%s: %llu", UIL::LS(CommonStrings::Signature), e.skeletonSignature);
									}
									else
									{
										stl::snprintf(buffer, "%s: %d", UIL::LS(CommonStrings::ID), e.skeletonID);
									}

									vdesc = buffer;
									tdesc = UIL::LS(CommonStrings::Skeleton);
								}

								break;

							case Data::EquipmentOverrideConditionType::Faction:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::CompOper>(
									e.compOperator);
								m_condParamEditor.SetNext<ConditionParamItem::Int32>(
									e.factionRank);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = UIL::LS(CommonStrings::Faction);

								break;

							case Data::EquipmentOverrideConditionType::Effect:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								/*m_condParamEditor.SetNext<ConditionParamItem::FormAny>(
									e.form2.get_id(),
									CommonStrings::Source);*/
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = UIL::LS(CommonStrings::Effect);

								break;

							case Data::EquipmentOverrideConditionType::Variable:

								m_condParamEditor.SetNext<ConditionParamItem::CondVarType>(
									e.condVarType,
									e.s0);
								m_condParamEditor.SetNext<ConditionParamItem::VarCondSource>(
									e.vcSource);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id(),
									CommonStrings::Form);
								m_condParamEditor.SetNext<ConditionParamItem::CompOper>(
									e.compOperator);
								m_condParamEditor.SetNext<ConditionParamItem::Int32>(
									e.i32a);
								m_condParamEditor.SetNext<ConditionParamItem::Float>(
									e.f32a);
								m_condParamEditor.SetNext<ConditionParamItem::FormAny>(
									e.form2.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::CondVarType);
								tdesc = UIL::LS(CommonStrings::Variable);

								break;

							case Data::EquipmentOverrideConditionType::Perk:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearForm);

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::CompOper>(
									e.compOperator);
								m_condParamEditor.SetNext<ConditionParamItem::Int32>(
									e.perkRank);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = UIL::LS(CommonStrings::Perk);

								break;

							case Data::EquipmentOverrideConditionType::Cell:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::LightingTemplateInheritanceFlags>(
									e.lightingTemplateInheritanceFlags);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = UIL::LS(CommonStrings::Cell);

								break;

							case Data::EquipmentOverrideConditionType::Hand:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								{
									const auto d = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);

									auto& b = m_condParamEditor.GetDescBuffer2();

									if (d[0])
									{
										stl::snprintf(
											b,
											"%s%s, T:%hhu",
											e.flags.test(Data::EquipmentOverrideConditionFlags::kMatchLeftHand) ? "L, " : "",
											d,
											e.formType);
									}
									else
									{
										stl::snprintf(
											b,
											"%sT:%hhu",
											e.flags.test(Data::EquipmentOverrideConditionFlags::kMatchLeftHand) ? "L, " : "",
											e.formType);
									}

									vdesc = b;
									tdesc = UIL::LS(CommonStrings::Hand);
								}

								break;

							default:
								tdesc = nullptr;
								vdesc = nullptr;
								break;
							}

							if (!tdesc)
							{
								tdesc = "";
							}

							if (!vdesc)
							{
								vdesc = "";
							}

							ImGui::TextUnformatted(tdesc);

							ImGui::TableSetColumnIndex(2);

							const bool r = ImGui::Selectable(
								UIL::LMKID<3>(vdesc, "sel_ctl"),
								false,
								ImGuiSelectableFlags_DontClosePopups);

							UICommon::ToolTip(vdesc);

							if (r)
							{
								m_condParamEditor.OpenConditionParamEditorPopup();
							}

							if (m_condParamEditor.DrawConditionParamEditorPopup())
							{
								a_updFunc();
							}
						}

						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

						ImGui::TableSetColumnIndex(3);

						if (ImGui::CheckboxFlagsT(
								"##oper_and",
								stl::underlying(std::addressof(e.flags.value)),
								stl::underlying(Data::EquipmentOverrideConditionFlags::kAnd)))
						{
							a_updFunc();
						}

						ImGui::TableSetColumnIndex(4);

						if (ImGui::CheckboxFlagsT(
								"##oper_not",
								stl::underlying(std::addressof(e.flags.value)),
								stl::underlying(Data::EquipmentOverrideConditionFlags::kNot)))
						{
							a_updFunc();
						}

						ImGui::PopStyleVar();

						++it;
						i++;
					}

					//ImGui::PopStyleVar();

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();

			if (!a_isnested)
			{
				OnConditionListDrawn(a_entry);
			}
		}

		void UIEquipmentOverrideConditionsWidget::UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType a_type)
		{
			UpdateMatchParamAllowedTypesImpl(a_type, m_condParamEditor);
		}
	}
}