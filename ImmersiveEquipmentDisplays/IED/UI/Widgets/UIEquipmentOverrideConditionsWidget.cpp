#include "pch.h"

#include "UIBipedObjectSelectorWidget.h"
#include "UIComparisonOperatorSelector.h"
#include "UIConditionExtraSelectorWidget.h"
#include "UIEquipmentOverrideConditionsWidget.h"
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
					result.action = BaseConfigEditorAction::Swap;
					result.dir    = SwapDirection::Up;
				}

				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

				if (ImGui::ArrowButton("down", ImGuiDir_Down))
				{
					result.action = BaseConfigEditorAction::Swap;
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
							result.action    = BaseConfigEditorAction::Insert;
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
								result.action    = BaseConfigEditorAction::Insert;
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
								result.action    = BaseConfigEditorAction::Insert;
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
							result.action    = BaseConfigEditorAction::Insert;
							result.biped     = m_ooNewBiped;
							result.entryType = Data::EquipmentOverrideConditionType::BipedSlot;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Actor, "6"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Actor;
					}

					if (UIL::LCG_MI(CommonStrings::NPC, "7"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::NPC;
					}

					if (UIL::LCG_MI(CommonStrings::Race, "8"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Race;
					}

					if (UIL::LCG_MI(CommonStrings::Furniture, "9"))
					{
						result.action    = BaseConfigEditorAction::Insert;
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
								result.action    = BaseConfigEditorAction::Insert;
								result.form      = m_aoNewEntryID;
								result.entryType = Data::EquipmentOverrideConditionType::Quest;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Location, "B"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Location;
					}

					if (UIL::LCG_MI(CommonStrings::Worldspace, "C"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Worldspace;
					}

					if (UIL::LCG_MI(CommonStrings::Package, "D"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Package;
					}

					if (UIL::LCG_MI(CommonStrings::Weather, "E"))
					{
						result.action    = BaseConfigEditorAction::Insert;
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
								result.action    = BaseConfigEditorAction::Insert;
								result.form      = m_aoNewEntryGlobID;
								result.entryType = Data::EquipmentOverrideConditionType::Global;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(UIWidgetCommonStrings::Mounting, "G"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Mounting;
					}

					if (UIL::LCG_MI(UIWidgetCommonStrings::Mounted, "H"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Mounted;
					}

					if (UIL::LCG_MI(UIWidgetCommonStrings::CurrentItem, "I"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Presence;
					}

					if (UIL::LCG_MI(CommonStrings::Idle, "J"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Idle;
					}

					if (UIL::LCG_MI(CommonStrings::Skeleton, "K"))
					{
						result.action    = BaseConfigEditorAction::Insert;
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
								result.action    = BaseConfigEditorAction::Insert;
								result.form      = m_aoNewEntryFactionID;
								result.entryType = Data::EquipmentOverrideConditionType::Faction;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Effect, "M"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Effect;
					}

					if (UIL::LCG_BM(CommonStrings::Variable, "N"))
					{
						if (DrawDescriptionPopup())
						{
							result.action = BaseConfigEditorAction::Insert;
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
								result.action    = BaseConfigEditorAction::Insert;
								result.form      = m_aoNewEntryPerkID;
								result.entryType = Data::EquipmentOverrideConditionType::Perk;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Cell, "P"))
					{
						result.action    = BaseConfigEditorAction::Insert;
						result.entryType = Data::EquipmentOverrideConditionType::Cell;
					}

					if (UIL::LCG_BM(CommonStrings::Extra, "X"))
					{
						if (UIConditionExtraSelectorWidget::DrawExtraConditionSelector(
								m_ooNewExtraCond))
						{
							result.action    = BaseConfigEditorAction::Insert;
							result.excond    = m_ooNewExtraCond;
							result.entryType = Data::EquipmentOverrideConditionType::Extra;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Group, "Y"))
					{
						result.action    = BaseConfigEditorAction::Insert;
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
						result.action = BaseConfigEditorAction::Paste;
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_MI(CommonStrings::Delete, "2"))
				{
					result.action = BaseConfigEditorAction::Delete;
				}

				ImGui::Separator();

				if (a_header)
				{
					if (UIL::LCG_MI(CommonStrings::Copy, "3"))
					{
						result.action = BaseConfigEditorAction::Copy;
					}

					auto clipData = UIClipboard::Get<Data::equipmentOverrideConditionList_t>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::PasteOver, "4"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = BaseConfigEditorAction::PasteOver;
					}
				}
				else
				{
					if (UIL::LCG_MI(CommonStrings::Copy, "3"))
					{
						result.action = BaseConfigEditorAction::Copy;
					}

					auto clipData = UIClipboard::Get<Data::equipmentOverrideCondition_t>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::PasteOver, "5"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = BaseConfigEditorAction::PasteOver;
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

			switch (match->fbf.type)
			{
			case Data::EquipmentOverrideConditionType::Form:
			case Data::EquipmentOverrideConditionType::Type:
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

				if (match->fbf.type == Data::EquipmentOverrideConditionType::Form &&
				    !match->flags.test_any(Data::EquipmentOverrideConditionFlags::kMatchMaskAny))
				{
					ImGui::Spacing();

					result |= DrawFormCountExtraSegment(match);
				}

				break;

			case Data::EquipmentOverrideConditionType::Furniture:

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(CommonStrings::LayingDown, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				break;

			case Data::EquipmentOverrideConditionType::BipedSlot:
				{
					result |= ImGui::CheckboxFlagsT(
						UIL::LS(UINodeOverrideEditorWidgetStrings::MatchSkin, "1"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag2));

					UITipsInterface::DrawTip(UITip::MatchSkin);

					bool disabled = !match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag1);

					UICommon::PushDisabled(disabled);

					result |= ImGui::CheckboxFlagsT(
						"!##2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch3));

					UICommon::PopDisabled(disabled);

					ImGui::SameLine();

					result |= ImGui::CheckboxFlagsT(
						UIL::LS(UINodeOverrideEditorWidgetStrings::IsBolt, "3"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

					disabled = !match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag3);

					UICommon::PushDisabled(disabled);

					result |= ImGui::CheckboxFlagsT(
						"!##4",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch4));

					UICommon::PopDisabled(disabled);

					ImGui::SameLine();

					result |= ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::GeometryVisible, "5"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag3));
				}
				break;

			case Data::EquipmentOverrideConditionType::Location:

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::MatchParent, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				UITipsInterface::DrawTip(UITip::MatchChildLoc);

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::MatchEither, "2"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kMatchCategoryOperOR));

				UITipsInterface::DrawTip(UITip::MatchEitherFormKW);

				break;

			case Data::EquipmentOverrideConditionType::Worldspace:

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::MatchParent, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				UITipsInterface::DrawTip(UITip::MatchWorldspaceParent);

				break;

			case Data::EquipmentOverrideConditionType::Race:

				result |= ImGui::CheckboxFlagsT(
					"!##1",
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch3));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::IsPlayable, "2"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				result |= ImGui::CheckboxFlagsT(
					"!##3",
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch4));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::ChildRace, "4"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag2));

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
										match->fbf.presenceEquipedHandMatch == Data::PresenceEquippedHandMatch::kEither))
								{
									match->fbf.presenceEquipedHandMatch = Data::PresenceEquippedHandMatch::kEither;
									result                              = true;
								}

								ImGui::SameLine();

								if (ImGui::RadioButton(
										UIL::LS(CommonStrings::Left, "D"),
										match->fbf.presenceEquipedHandMatch == Data::PresenceEquippedHandMatch::kLeft))
								{
									match->fbf.presenceEquipedHandMatch = Data::PresenceEquippedHandMatch::kLeft;
									result                              = true;
								}

								ImGui::SameLine();

								if (ImGui::RadioButton(
										UIL::LS(CommonStrings::Right, "E"),
										match->fbf.presenceEquipedHandMatch == Data::PresenceEquippedHandMatch::kRight))
								{
									match->fbf.presenceEquipedHandMatch = Data::PresenceEquippedHandMatch::kRight;
									result                              = true;
								}
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

				ImGui::Spacing();
				ImGui::Text("%s:", UIL::LS(CommonStrings::Info));
				ImGui::SameLine();
				UITipsInterface::DrawTip(UITip::IdleCondition);

				break;

			case Data::EquipmentOverrideConditionType::Skeleton:

				if (ImGui::RadioButton(
						UIL::LS(CommonStrings::ID, "1"),
						!match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag1)))
				{
					result = true;
					match->flags.clear(Data::EquipmentOverrideConditionFlags::kExtraFlag1);
				}

				ImGui::SameLine();

				if (ImGui::RadioButton(
						UIL::LS(CommonStrings::Signature, "2"),
						match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag1)))
				{
					result = true;
					match->flags.set(Data::EquipmentOverrideConditionFlags::kExtraFlag1);
				}

				ImGui::Separator();
				ImGui::Spacing();

				if (match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag1))
				{
					result |= ImGui::InputScalar(
						UIL::LS(CommonStrings::Signature, "3"),
						ImGuiDataType_U64,
						std::addressof(match->skeletonSignature),
						nullptr,
						nullptr,
						"%llu",
						ImGuiInputTextFlags_EnterReturnsTrue |
							ImGuiInputTextFlags_CharsDecimal);
				}
				else
				{
					result |= ImGui::InputScalar(
						UIL::LS(CommonStrings::ID, "3"),
						ImGuiDataType_S32,
						std::addressof(match->skeletonID),
						nullptr,
						nullptr,
						"%d",
						ImGuiInputTextFlags_EnterReturnsTrue |
							ImGuiInputTextFlags_CharsDecimal);
				}

				break;

			case Data::EquipmentOverrideConditionType::Mounting:

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::IsMountedActorHorse, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				break;

			case Data::EquipmentOverrideConditionType::Cell:

				result |= ImGui::CheckboxFlagsT(
					"!##1",
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch3));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(CommonStrings::Interior, "2"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				result |= ImGui::CheckboxFlagsT(
					"!##3",
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch4));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::PublicArea, "4"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag2));

				break;

			case Data::EquipmentOverrideConditionType::NPC:

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::MatchNPCOrTemplate, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

				UITipsInterface::DrawTip(UITip::MatchNPCOrTemplate);

				break;

			case Data::EquipmentOverrideConditionType::Extra:

				switch (match->extraCondType)
				{
				case Data::ExtraConditionType::kSunAngle:

					result |= ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::Absolute, "1"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

					ImGui::Spacing();
					ImGui::Text("%s:", UIL::LS(CommonStrings::Info));
					ImGui::SameLine();
					UITipsInterface::DrawTip(UITip::SunAngle);

					break;
				}

				break;
			}

			ImGui::PopID();

			return result;
		}

		bool UIEquipmentOverrideConditionsWidget::DrawConditionItemExtra(
			ConditionParamItem           a_item,
			ConditionParamItemExtraArgs& a_args)
		{
			auto match = static_cast<Data::equipmentOverrideCondition_t*>(a_args.p3);

			bool result = false;

			ImGui::PushID("match_item_extra");

			switch (match->fbf.type)
			{
			case Data::EquipmentOverrideConditionType::Type:
			case Data::EquipmentOverrideConditionType::Furniture:
			case Data::EquipmentOverrideConditionType::BipedSlot:
			case Data::EquipmentOverrideConditionType::Location:
			case Data::EquipmentOverrideConditionType::Package:

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

					if (match->fbf.type == Data::EquipmentOverrideConditionType::BipedSlot &&
					    match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag2))
					{
						a_args.disable = true;
					}

					ImGui::SameLine();
				}
				else if (
					a_item == ConditionParamItem::Keyword ||
					a_item == ConditionParamItem::PackageType)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}

				break;

			case Data::EquipmentOverrideConditionType::Form:

				if (a_item == ConditionParamItem::Keyword)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();
				}

				break;
			case Data::EquipmentOverrideConditionType::Effect:

				if (a_item == ConditionParamItem::FormAny)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_3",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch3));

					ImGui::SameLine();

					a_args.formFilter = UIFormBrowserFilter::EffectSource;
				}

				[[fallthrough]];
			case Data::EquipmentOverrideConditionType::Actor:
			case Data::EquipmentOverrideConditionType::NPC:
			case Data::EquipmentOverrideConditionType::Race:
			case Data::EquipmentOverrideConditionType::Idle:
			case Data::EquipmentOverrideConditionType::Cell:

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::Keyword)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();
				}

				break;

			case Data::EquipmentOverrideConditionType::Extra:

				if (a_item == ConditionParamItem::Form)
				{
					switch (match->extraCondType)
					{
					case Data::ExtraConditionType::kShoutEquipped:
					case Data::ExtraConditionType::kCombatStyle:
					case Data::ExtraConditionType::kClass:

						result = ImGui::CheckboxFlagsT(
							"!##ctl_neg_1",
							stl::underlying(std::addressof(match->flags.value)),
							stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

						ImGui::SameLine();

						a_args.hide = false;
						break;
					default:
						a_args.hide = true;
						break;
					}
				}

				break;

			case Data::EquipmentOverrideConditionType::Weather:

				if (a_item == ConditionParamItem::Form)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::WeatherClass)
				{
					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();
				}

				break;
			case Data::EquipmentOverrideConditionType::Mounting:
			case Data::EquipmentOverrideConditionType::Mounted:

				switch (a_item)
				{
				case ConditionParamItem::Form:

					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

					ImGui::SameLine();

					break;

				case ConditionParamItem::Race:

					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch2));

					ImGui::SameLine();

					break;

				case ConditionParamItem::Keyword:

					result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_3",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch3));

					ImGui::SameLine();

					break;
				}

				break;
			case Data::EquipmentOverrideConditionType::Faction:
			case Data::EquipmentOverrideConditionType::Perk:

				switch (a_item)
				{
				case ConditionParamItem::CompOper:

					result = ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::Rank, "ctl_tog_1"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::EquipmentOverrideConditionFlags::kExtraFlag1));

					if (match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag1))
					{
						ImGui::SameLine();
					}

					[[fallthrough]];
				case ConditionParamItem::Int32:

					a_args.hide = !match->flags.test(Data::EquipmentOverrideConditionFlags::kExtraFlag1);

					break;
				}

				break;

			case Data::EquipmentOverrideConditionType::Variable:

				switch (a_item)
				{
				case ConditionParamItem::Int32:

					if (match->condVarType != ConditionalVariableType::kInt32)
					{
						a_args.hide = true;
					}

					break;
				case ConditionParamItem::Float:

					if (match->condVarType != ConditionalVariableType::kFloat)
					{
						a_args.hide = true;
					}

					break;
				case ConditionParamItem::Form:

					switch (match->vcSource)
					{
					case Data::VariableConditionSource::kActor:
					case Data::VariableConditionSource::kNPC:
					case Data::VariableConditionSource::kRace:

						result |= ImGui::CheckboxFlagsT(
							"!##ctl_neg_1",
							stl::underlying(std::addressof(match->flags.value)),
							stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch1));

						ImGui::SameLine();

						break;
					default:
						a_args.hide = true;
						break;
					}

					break;

				case ConditionParamItem::VarCondSource:

					if (match->vcSource == Data::VariableConditionSource::kSelf)
					{
						result |= ImGui::CheckboxFlagsT(
							"!##ctl_neg_2",
							stl::underlying(std::addressof(match->flags.value)),
							stl::underlying(Data::EquipmentOverrideConditionFlags::kNegateMatch2));

						ImGui::SameLine();
					}

					break;

				case ConditionParamItem::FormAny:

					if (match->condVarType != ConditionalVariableType::kForm)
					{
						a_args.hide = true;
					}

					break;

					/*case ConditionParamItem::CompOper:

					if (match->condVarType == ConditionalVariableType::kForm)
					{
						a_args.hide = true;
					}

					break;*/
				}

				break;
			}

			ImGui::PopID();

			return result;
		}

		void UIEquipmentOverrideConditionsWidget::OnConditionItemChange(
			ConditionParamItem                    a_item,
			const ConditionParamItemOnChangeArgs& a_args)
		{
			auto match = static_cast<Data::equipmentOverrideCondition_t*>(a_args.p3);

			switch (match->fbf.type)
			{
			case Data::EquipmentOverrideConditionType::Variable:

				if (a_item == ConditionParamItem::VarCondSource)
				{
					match->form = {};
				}

				break;
			}
		}

		BaseConfigEditorAction UIEquipmentOverrideConditionsWidget::DrawEquipmentOverrideConditionHeaderContextMenu(
			Data::equipmentOverrideConditionList_t& a_entry,
			update_func_t                           a_updFunc)
		{
			BaseConfigEditorAction action{ BaseConfigEditorAction ::None };

			const auto result = DrawEquipmentOverrideConditionContextMenu(true);

			switch (result.action)
			{
			case BaseConfigEditorAction::Insert:
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

			case BaseConfigEditorAction::Copy:
				UIClipboard::Set(a_entry);
				break;

			case BaseConfigEditorAction::Paste:
				if (auto clipData = UIClipboard::Get<Data::equipmentOverrideCondition_t>())
				{
					a_entry.emplace_back(*clipData);

					a_updFunc();

					action = result.action;
				}
				break;
			case BaseConfigEditorAction::PasteOver:
				if (auto clipData = UIClipboard::Get<Data::equipmentOverrideConditionList_t>())
				{
					a_entry = *clipData;

					a_updFunc();

					action = result.action;
				}
				break;
			case BaseConfigEditorAction::Delete:
				a_entry.clear();

				action = result.action;

				a_updFunc();

				break;
			}

			return action;
		}

		void UIEquipmentOverrideConditionsWidget::DrawEquipmentOverrideConditionTree(
			Data::equipmentOverrideConditionList_t& a_entry,
			update_func_t                           a_updFunc,
			Localization::StringID                  a_title)
		{
			ImGui::PushID("cond_tree_area");

			const auto r = DrawEquipmentOverrideConditionHeaderContextMenu(
				a_entry,
				a_updFunc);

			bool empty = a_entry.empty();

			if (!empty)
			{
				switch (r)
				{
				case BaseConfigEditorAction::Paste:
				case BaseConfigEditorAction::PasteOver:
				case BaseConfigEditorAction::Insert:
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
				if (e.fbf.type == Data::EquipmentOverrideConditionType::Group)
				{
					a_offset++;
					a_result = std::max(a_result, a_offset);

					GetConditionListDepth(e.group.conditions, a_result, a_offset);

					a_offset--;
				}
			}
		}

		void UIEquipmentOverrideConditionsWidget::DrawEquipmentOverrideEntryConditionTable(
			Data::equipmentOverrideConditionList_t& a_entry,
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

				GetConditionListDepth(a_entry, res, off);

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

				auto it = a_entry.begin();

				while (it != a_entry.end())
				{
					ImGui::PushID(i);

					ImGui::TableNextRow();

					//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });

					ImGui::TableSetColumnIndex(0);

					const auto result = DrawEquipmentOverrideConditionContextMenu(false);

					switch (result.action)
					{
					case BaseConfigEditorAction::Delete:
						it = a_entry.erase(it);
						a_updFunc();
						break;
					case BaseConfigEditorAction::Insert:

						switch (result.entryType)
						{
						case Data::EquipmentOverrideConditionType::Type:
							if (result.slot != Data::ObjectSlotExtra::kNone)
							{
								it = a_entry.emplace(
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
								it = a_entry.emplace(
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

							it = a_entry.emplace(
								it,
								result.entryType);

							a_updFunc();

							break;
						case Data::EquipmentOverrideConditionType::BipedSlot:

							if (result.biped != BIPED_OBJECT::kNone)
							{
								it = a_entry.emplace(
									it,
									result.biped);

								a_updFunc();
							}

							break;
						case Data::EquipmentOverrideConditionType::Variable:

							it = a_entry.emplace(it, result.entryType, result.desc);

							a_updFunc();

							break;
						case Data::EquipmentOverrideConditionType::Extra:

							if (result.excond != Data::ExtraConditionType::kNone)
							{
								it = a_entry.emplace(
									it,
									result.excond);

								a_updFunc();
							}

							break;
						}

						break;

					case BaseConfigEditorAction::Swap:

						if (IterSwap(a_entry, it, result.dir))
						{
							a_updFunc();
						}

						break;

					case BaseConfigEditorAction::Copy:

						UIClipboard::Set(*it);

						break;

					case BaseConfigEditorAction::Paste:

						if (auto clipData = UIClipboard::Get<Data::equipmentOverrideCondition_t>())
						{
							it = a_entry.emplace(
								it,
								*clipData);

							a_updFunc();
						}

						break;

					case BaseConfigEditorAction::PasteOver:

						if (auto clipData = UIClipboard::Get<Data::equipmentOverrideCondition_t>())
						{
							*it = *clipData;

							a_updFunc();
						}

						break;
					}

					if (it != a_entry.end())
					{
						auto& e = *it;

						ImGui::TableSetColumnIndex(1);

						if (e.fbf.type == Data::EquipmentOverrideConditionType::Group)
						{
							ImGui::TextUnformatted(UIL::LS(CommonStrings::Group));

							ImGui::TableSetColumnIndex(2);

							ImGui::PushID("cond_grp");

							DrawEquipmentOverrideConditionHeaderContextMenu(
								e.group.conditions,
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

							if (e.fbf.type == Data::EquipmentOverrideConditionType::Variable)
							{
								switch (e.vcSource)
								{
								case Data::VariableConditionSource::kActor:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Actor));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(false);
									break;
								case Data::VariableConditionSource::kNPC:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::NPC));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
									break;
								case Data::VariableConditionSource::kRace:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Race));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
									break;
								default:
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Common));
									m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
									break;
								}
							}
							else
							{
								UpdateMatchParamAllowedTypes(e.fbf.type);
							}

							const char* tdesc;
							const char* vdesc;

							switch (e.fbf.type)
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
									m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Shout));
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
								case Data::ExtraConditionType::kKeyIDToggled:
									m_condParamEditor.SetNext<ConditionParamItem::KeyBindID>(e.s0);
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
								tdesc = e.fbf.type == Data::EquipmentOverrideConditionType::Mounting ?
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
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = UIL::LS(CommonStrings::Cell);

								break;

							default:
								tdesc = nullptr;
								vdesc = nullptr;
								break;
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
		}

		void UIEquipmentOverrideConditionsWidget::UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType a_type)
		{
			switch (a_type)
			{
			case Data::EquipmentOverrideConditionType::Race:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Race));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Furniture:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Furniture));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Quest:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Quest));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Actor:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Actor));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(false);
				break;
			case Data::EquipmentOverrideConditionType::NPC:
			case Data::EquipmentOverrideConditionType::Mounting:
			case Data::EquipmentOverrideConditionType::Mounted:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::NPC));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Location:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Location));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Worldspace:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Worldspace));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Package:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Package));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Weather:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Weather));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Global:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Global));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Idle:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Idle));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(false);
				break;
			case Data::EquipmentOverrideConditionType::Faction:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Faction));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Effect:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Effect));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Perk:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Perk));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Data::EquipmentOverrideConditionType::Cell:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Cell));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(false);
				break;
			default:
				m_condParamEditor.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Common));
				m_condParamEditor.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			}
		}
	}
}