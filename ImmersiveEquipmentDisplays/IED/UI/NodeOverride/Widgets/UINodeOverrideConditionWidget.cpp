#include "pch.h"

#include "UINodeOverrideConditionWidget.h"

#include "IED/UI/Widgets/UIBipedObjectSelectorWidget.h"
#include "IED/UI/Widgets/UICMNodeSelector.h"
#include "IED/UI/Widgets/UIConditionExtraSelectorWidget.h"
#include "IED/UI/Widgets/UIFormTypeSelectorWidget.h"
#include "IED/UI/Widgets/UIObjectTypeSelectorWidget.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/UI/UIFormBrowserCommonFilters.h"

#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UINodeOverrideConditionWidget::UINodeOverrideConditionWidget(
			Controller& a_controller) :
			m_condParamEditor(a_controller)
		{
			m_condParamEditor.SetExtraInterface(this);
		}

		NodeOverrideCommonResult UINodeOverrideConditionWidget::DrawConditionContextMenu(
			NodeOverrideDataType a_type,
			const bool           a_ignoreNode)
		{
			NodeOverrideCommonResult result;

			ImGui::PushID("oo_offset_match_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				reset_oo();
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = NodeOverrideCommonAction::Swap;
				result.dir    = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = NodeOverrideCommonAction::Swap;
				result.dir    = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			//ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Insert, "1"))
				{
					if ((a_type == NodeOverrideDataType::kTransform ||
					     a_type == NodeOverrideDataType::kPhysics) &&
					    !a_ignoreNode)
					{
						if (UIL::LCG_BM(CommonStrings::Node, "2"))
						{
							stl::fixed_string c;

							if (UICMNodeSelectorWidget::DrawCMNodeSelector(
									"##node_sel",
									c,
									NodeOverrideData::GetCMENodeData(),
									nullptr))  // std::addressof(a_name)
							{
								result.action    = NodeOverrideCommonAction::Insert;
								result.str       = std::move(c);
								result.matchType = Data::NodeOverrideConditionType::Node;

								ImGui::CloseCurrentPopup();
							}

							ImGui::EndMenu();
						}
					}

					if (UIL::LCG_BM(CommonStrings::Item, "3"))
					{
						if (UIL::LCG_BM(CommonStrings::Form, "4"))
						{
							UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Form);

							if (m_condParamEditor.GetFormPicker().DrawFormSelector(
									m_ooNewEntryID))
							{
								if (m_ooNewEntryID)
								{
									result.action    = NodeOverrideCommonAction::Insert;
									result.form      = m_ooNewEntryID;
									result.matchType = Data::NodeOverrideConditionType::Form;
								}

								ImGui::CloseCurrentPopup();
							}
							ImGui::EndMenu();
						}

						if (UIL::LCG_BM(CommonStrings::Keyword, "5"))
						{
							if (m_condParamEditor.GetKeywordPicker().DrawFormSelector(
									m_ooNewEntryIDKW))
							{
								if (m_ooNewEntryIDKW)
								{
									result.action    = NodeOverrideCommonAction::Insert;
									result.form      = m_ooNewEntryIDKW;
									result.matchType = Data::NodeOverrideConditionType::Keyword;
								}

								ImGui::CloseCurrentPopup();
							}
							ImGui::EndMenu();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(UIWidgetCommonStrings::BipedSlot, "6"))
					{
						if (UIBipedObjectSelectorWidget::DrawBipedObjectSelector(
								UIL::LS(CommonStrings::Biped, "bp"),
								m_ooNewBiped))
						{
							result.action    = NodeOverrideCommonAction::Insert;
							result.biped     = m_ooNewBiped;
							result.matchType = Data::NodeOverrideConditionType::BipedSlot;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(CommonStrings::Type, "7"))
					{
						if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
								UIL::LS(CommonStrings::Type, "ss"),
								m_ooNewSlot))
						{
							result.action    = NodeOverrideCommonAction::Insert;
							result.type      = m_ooNewSlot;
							result.matchType = Data::NodeOverrideConditionType::Type;

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Hand, "7a")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Hand;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Actor, "8")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Actor;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::NPC, "9")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::NPC;
					}

					if (UIL::LCG_MI(CommonStrings::Race, "A"))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Race;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Furniture, "B")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Furniture;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Location, "C")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Location;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Worldspace, "D")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Worldspace;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Package, "E")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Package;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Weather, "F")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Weather;
					}

					if (UIL::LCG_BM(CommonStrings::Global, "G"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Global);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDGlob))
						{
							if (m_ooNewEntryIDGlob)
							{
								result.action    = NodeOverrideCommonAction::Insert;
								result.form      = m_ooNewEntryIDGlob;
								result.matchType = Data::NodeOverrideConditionType::Global;
							}

							ImGui::CloseCurrentPopup();
						}
						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(UIL::LS(UIWidgetCommonStrings::Mounting, "H")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Mounting;
					}

					if (ImGui::MenuItem(UIL::LS(UIWidgetCommonStrings::Mounted, "G")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Mounted;
					}

					if (UIL::LCG_MI(CommonStrings::Idle, "H"))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Idle;
					}

					if (UIL::LCG_MI(CommonStrings::Skeleton, "I"))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Skeleton;
					}

					if (UIL::LCG_BM(CommonStrings::Faction, "J"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Faction);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDFaction))
						{
							if (m_ooNewEntryIDFaction)
							{
								result.action    = NodeOverrideCommonAction::Insert;
								result.form      = m_ooNewEntryIDFaction;
								result.matchType = Data::NodeOverrideConditionType::Faction;
							}

							ImGui::CloseCurrentPopup();
						}
						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Effect, "K")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Effect;
					}

					if (UIL::LCG_BM(CommonStrings::Variable, "L"))
					{
						if (DrawDescriptionPopup())
						{
							result.action    = NodeOverrideCommonAction::Insert;
							result.desc      = GetDescriptionPopupBuffer();
							result.matchType = Data::NodeOverrideConditionType::Variable;

							ClearDescriptionPopupBuffer();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(CommonStrings::Perk, "M"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Perk);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDPerk))
						{
							if (m_ooNewEntryIDPerk)
							{
								result.action    = NodeOverrideCommonAction::Insert;
								result.form      = m_ooNewEntryIDPerk;
								result.matchType = Data::NodeOverrideConditionType::Perk;
							}

							ImGui::CloseCurrentPopup();
						}
						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Cell, "N")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Cell;
					}

					if (UIL::LCG_BM(CommonStrings::Extra, "Y"))
					{
						if (UIConditionExtraSelectorWidget::DrawExtraConditionSelector(
								m_ooNewExtraCond))
						{
							if (m_ooNewExtraCond != Data::ExtraConditionType::kNone)
							{
								result.action    = NodeOverrideCommonAction::Insert;
								result.excond    = m_ooNewExtraCond;
								result.matchType = Data::NodeOverrideConditionType::Extra;

								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Group, "Z")))
					{
						result.action    = NodeOverrideCommonAction::Insert;
						result.matchType = Data::NodeOverrideConditionType::Group;

						ImGui::CloseCurrentPopup();
					}

					ImGui::Separator();

					auto clipData = UIClipboard::Get<Data::configNodeOverrideCondition_t>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::Paste, "PA"),
							nullptr,
							false,
							clipData != nullptr))
					{
						result.action = NodeOverrideCommonAction::Paste;
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Delete, "2")))
				{
					result.action = NodeOverrideCommonAction::Delete;
				}

				ImGui::Separator();

				if (UIL::LCG_MI(CommonStrings::Copy, "3"))
				{
					result.action = NodeOverrideCommonAction::Copy;
				}

				auto clipData = UIClipboard::Get<Data::configNodeOverrideCondition_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "4"),
						nullptr,
						false,
						clipData != nullptr))
				{
					result.action = NodeOverrideCommonAction::PasteOver;
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			ImGui::PopID();

			return result;
		}

		NodeOverrideCommonAction UINodeOverrideConditionWidget::DrawConditionHeaderContextMenu(
			const stl::fixed_string&                 a_name,
			NodeOverrideDataType                     a_type,
			entryNodeOverrideData_t&                 a_data,
			Data::configNodeOverrideConditionList_t& a_entry,
			const bool                               a_exists,
			const bool                               a_ignoreNode,
			update_func_t                            a_updateFunc)
		{
			NodeOverrideCommonAction result{ NodeOverrideCommonAction::None };

			ImGui::PushID("oo_matchlist_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				reset_oo();

				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Add, "1"))
				{
					if ((a_type == NodeOverrideDataType::kTransform ||
					     a_type == NodeOverrideDataType::kPhysics) &&
					    !a_ignoreNode)
					{
						if (UIL::LCG_BM(CommonStrings::Node, "2"))
						{
							stl::fixed_string c;

							if (UICMNodeSelectorWidget::DrawCMNodeSelector(
									"##node_sel",
									c,
									NodeOverrideData::GetCMENodeData(),
									nullptr))  // std::addressof(a_name)
							{
								a_entry.emplace_back(std::move(c));

								a_updateFunc();

								result = NodeOverrideCommonAction::Insert;

								ImGui::CloseCurrentPopup();
							}

							ImGui::EndMenu();
						}
					}

					if (UIL::LCG_BM(CommonStrings::Item, "3"))
					{
						if (UIL::LCG_BM(CommonStrings::Form, "1"))
						{
							UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Form);

							if (m_condParamEditor.GetFormPicker().DrawFormSelector(
									m_ooNewEntryID))
							{
								if (m_ooNewEntryID)
								{
									a_entry.emplace_back(
										Data::NodeOverrideConditionType::Form,
										m_ooNewEntryID);

									a_updateFunc();

									result = NodeOverrideCommonAction::Insert;
								}

								ImGui::CloseCurrentPopup();
							}

							ImGui::EndMenu();
						}

						if (UIL::LCG_BM(CommonStrings::Keyword, "2"))
						{
							if (m_condParamEditor.GetKeywordPicker().DrawFormSelector(
									m_ooNewEntryIDKW))
							{
								if (m_ooNewEntryIDKW)
								{
									a_entry.emplace_back(
										Data::NodeOverrideConditionType::Keyword,
										m_ooNewEntryIDKW);

									a_updateFunc();

									result = NodeOverrideCommonAction::Insert;
								}

								ImGui::CloseCurrentPopup();
							}

							ImGui::EndMenu();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(UIWidgetCommonStrings::BipedSlot, "3"))
					{
						if (UIBipedObjectSelectorWidget::DrawBipedObjectSelector(
								UIL::LS(CommonStrings::Biped, "1"),
								m_ooNewBiped))
						{
							if (m_ooNewBiped != BIPED_OBJECT::kNone)
							{
								a_entry.emplace_back(m_ooNewBiped);

								a_updateFunc();

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(CommonStrings::Type, "4"))
					{
						if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
								UIL::LS(CommonStrings::Type, "1"),
								m_ooNewSlot))
						{
							if (m_ooNewSlot < Data::ObjectSlotExtra::kMax)
							{
								a_entry.emplace_back(m_ooNewSlot);

								a_updateFunc();

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Hand, "4a"))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Hand);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (UIL::LCG_MI(CommonStrings::Actor, "5"))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Actor);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (UIL::LCG_MI(CommonStrings::NPC, "6"))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::NPC);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (UIL::LCG_MI(CommonStrings::Race, "7"))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Race);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Furniture, "8")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Furniture);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Location, "9")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Location);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Worldspace, "A")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Worldspace);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Package, "B")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Package);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Weather, "C")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Weather);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (UIL::LCG_BM(CommonStrings::Global, "D"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Global);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDGlob))
						{
							if (m_ooNewEntryIDGlob)
							{
								a_entry.emplace_back(
									Data::NodeOverrideConditionType::Global,
									m_ooNewEntryIDGlob);

								a_updateFunc();

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(UIL::LS(UIWidgetCommonStrings::Mounting, "F")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Mounting);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(UIL::LS(UIWidgetCommonStrings::Mounted, "G")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Mounted);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (UIL::LCG_MI(CommonStrings::Idle, "H"))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Idle);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Skeleton, "I")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Skeleton);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (UIL::LCG_BM(CommonStrings::Faction, "J"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Faction);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDFaction))
						{
							if (m_ooNewEntryIDFaction)
							{
								a_entry.emplace_back(
									Data::NodeOverrideConditionType::Faction,
									m_ooNewEntryIDFaction);

								a_updateFunc();

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Effect, "K"))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Effect);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (UIL::LCG_BM(CommonStrings::Variable, "L"))
					{
						if (DrawDescriptionPopup())
						{
							a_entry.emplace_back(
								Data::NodeOverrideConditionType::Variable,
								GetDescriptionPopupBuffer());

							a_updateFunc();

							result = NodeOverrideCommonAction::Insert;

							ClearDescriptionPopupBuffer();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_BM(CommonStrings::Perk, "M"))
					{
						UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType::Perk);

						if (m_condParamEditor.GetFormPicker().DrawFormSelector(
								m_ooNewEntryIDPerk))
						{
							if (m_ooNewEntryIDPerk)
							{
								a_entry.emplace_back(
									Data::NodeOverrideConditionType::Perk,
									m_ooNewEntryIDPerk);

								a_updateFunc();

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (UIL::LCG_MI(CommonStrings::Cell, "N"))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Cell);

						a_updateFunc();

						result = NodeOverrideCommonAction::Insert;
					}

					if (UIL::LCG_BM(CommonStrings::Extra, "Y"))
					{
						if (UIConditionExtraSelectorWidget::DrawExtraConditionSelector(
								m_ooNewExtraCond))
						{
							if (m_ooNewExtraCond != Data::ExtraConditionType::kNone)
							{
								a_entry.emplace_back(
									m_ooNewExtraCond);

								a_updateFunc();

								result = NodeOverrideCommonAction::Insert;
							}

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(UIL::LS(CommonStrings::Group, "Z")))
					{
						a_entry.emplace_back(
							Data::NodeOverrideConditionType::Group);

						a_updateFunc();

						ImGui::CloseCurrentPopup();
					}

					ImGui::Separator();

					auto clipData = UIClipboard::Get<Data::configNodeOverrideCondition_t>();

					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::Paste, "PA"),
							nullptr,
							false,
							clipData != nullptr))
					{
						if (clipData)
						{
							a_entry.emplace_back(*clipData);

							a_updateFunc();

							result = NodeOverrideCommonAction::Insert;
						}
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Clear, "2")))
				{
					a_entry.clear();

					a_updateFunc();

					result = NodeOverrideCommonAction::Clear;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "3")))
				{
					UIClipboard::Set(a_entry);
				}

				auto clipData = UIClipboard::Get<Data::configNodeOverrideConditionList_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "4"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_entry = *clipData;

						a_updateFunc();

						result = NodeOverrideCommonAction::PasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			ImGui::PopID();

			return result;
		}

		static constexpr void GetConditionListDepth(
			const Data::configNodeOverrideConditionList_t& a_in,
			std::uint32_t&                                 a_result,
			std::uint32_t&                                 a_offset) noexcept
		{
			for (auto& e : a_in)
			{
				if (e.flags.bf().type == Data::NodeOverrideConditionType::Group)
				{
					a_offset++;
					a_result = std::max(a_result, a_offset);

					GetConditionListDepth(e.group.conditions, a_result, a_offset);

					a_offset--;
				}
			}
		}

		void UINodeOverrideConditionWidget::DrawConditionTable(
			const stl::fixed_string&                 a_name,
			NodeOverrideDataType                     a_type,
			entryNodeOverrideData_t&                 a_data,
			Data::configNodeOverrideConditionList_t& a_entry,
			const bool                               a_exists,
			bool                                     a_isnested,
			bool                                     a_ignoreNode,
			update_func_t                            a_updateFunc)
		{
			if (a_isnested)
			{
				ImGui::PushStyleVar(
					ImGuiStyleVar_CellPadding,
					{ 2.f, 2.f });
			}
			else
			{
				ImGui::PushStyleVar(
					ImGuiStyleVar_CellPadding,
					{ 5.f, 5.f });
			}

			constexpr int   NUM_COLUMNS   = 5;
			constexpr float MIN_TAB_WIDTH = 320.0f;

			float           width;
			float           height     = 0.0f;
			float           innerWidth = 0.0f;
			ImGuiTableFlags flags      = ImGuiTableFlags_None;

			if (a_isnested)
			{
				width = -1.0f;
			}
			else
			{
				std::uint32_t res = 0;
				std::uint32_t off = 0;

				GetConditionListDepth(a_entry, res, off);

				if (res > 2)
				{
					const auto avail       = ImGui::GetContentRegionAvail().x;
					const auto wantedWidth = MIN_TAB_WIDTH + MIN_TAB_WIDTH * static_cast<float>(res);

					if (wantedWidth > avail)
					{
						flags |= ImGuiTableFlags_ScrollX |
						         ImGuiTableFlags_ScrollY;

						innerWidth = wantedWidth;
						height     = 400.0f;
					}
				}

				if (a_type == NodeOverrideDataType::kTransform)
				{
					width = std::max(
						ImGui::GetFontSize() * 30.0f,
						ImGui::GetContentRegionAvail().x - ImGui::GetFontSize());
				}
				else
				{
					width = -1.0f;
				}
			}

			if (ImGui::BeginTable(
					"offset_match_table",
					NUM_COLUMNS,
					flags |
						ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ width, height },
					innerWidth))
			{
				const auto w =
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

					ImGui::TableSetColumnIndex(0);

					const auto result = DrawConditionContextMenu(
						a_type,
						a_ignoreNode);

					switch (result.action)
					{
					case NodeOverrideCommonAction::Insert:

						switch (result.matchType)
						{
						case Data::NodeOverrideConditionType::Node:

							it = a_entry.emplace(
								it,
								result.str);

							break;
						case Data::NodeOverrideConditionType::Form:
						case Data::NodeOverrideConditionType::Keyword:
						case Data::NodeOverrideConditionType::Global:
						case Data::NodeOverrideConditionType::Mounting:
						case Data::NodeOverrideConditionType::Mounted:
						case Data::NodeOverrideConditionType::Faction:
						case Data::NodeOverrideConditionType::Perk:

							it = a_entry.emplace(
								it,
								result.matchType,
								result.form);

							break;
						case Data::NodeOverrideConditionType::BipedSlot:
							if (result.biped != BIPED_OBJECT::kNone)
							{
								it = a_entry.emplace(
									it,
									result.biped);
							}

							break;
						case Data::NodeOverrideConditionType::Type:

							if (result.type < Data::ObjectSlotExtra::kMax)
							{
								it = a_entry.emplace(
									it,
									result.type);
							}

							break;

						case Data::NodeOverrideConditionType::Actor:
						case Data::NodeOverrideConditionType::NPC:
						case Data::NodeOverrideConditionType::Race:
						case Data::NodeOverrideConditionType::Furniture:
						case Data::NodeOverrideConditionType::Group:
						case Data::NodeOverrideConditionType::Location:
						case Data::NodeOverrideConditionType::Worldspace:
						case Data::NodeOverrideConditionType::Package:
						case Data::NodeOverrideConditionType::Weather:
						case Data::NodeOverrideConditionType::Idle:
						case Data::NodeOverrideConditionType::Skeleton:
						case Data::NodeOverrideConditionType::Effect:
						case Data::NodeOverrideConditionType::Cell:
						case Data::NodeOverrideConditionType::Hand:

							it = a_entry.emplace(
								it,
								result.matchType);

							break;
						case Data::NodeOverrideConditionType::Variable:

							it = a_entry.emplace(
								it,
								result.matchType,
								result.desc);

							break;
						case Data::NodeOverrideConditionType::Extra:

							if (result.excond != Data::ExtraConditionType::kNone)
							{
								it = a_entry.emplace(
									it,
									result.excond);
							}

							break;
						}

						a_updateFunc();

						break;
					case NodeOverrideCommonAction::Delete:

						it = a_entry.erase(it);

						a_updateFunc();

						break;
					case NodeOverrideCommonAction::Swap:

						if (IterSwap(a_entry, it, result.dir))
						{
							a_updateFunc();
						}

						break;
					case NodeOverrideCommonAction::Copy:

						UIClipboard::Set(*it);

						break;
					case NodeOverrideCommonAction::Paste:
					case NodeOverrideCommonAction::PasteOver:

						if (auto clipData = UIClipboard::Get<Data::configNodeOverrideCondition_t>())
						{
							if (result.action == NodeOverrideCommonAction::Paste)
							{
								it = a_entry.emplace(
									it,
									*clipData);
							}
							else
							{
								*it = *clipData;
							}

							a_updateFunc();
						}

						break;
					}

					if (it != a_entry.end())
					{
						auto& e = *it;

						ImGui::TableSetColumnIndex(1);

						if (e.flags.bf().type == Data::NodeOverrideConditionType::Group)
						{
							ImGui::TextUnformatted(UIL::LS(CommonStrings::Group));

							ImGui::TableSetColumnIndex(2);

							ImGui::PushID("cond_grp");

							DrawConditionHeaderContextMenu(
								a_name,
								a_type,
								a_data,
								e.group.conditions,
								a_exists,
								a_ignoreNode,
								a_updateFunc);

							DrawConditionTable(
								a_name,
								a_type,
								a_data,
								e.group.conditions,
								a_exists,
								true,
								a_ignoreNode,
								a_updateFunc);

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
							case Data::NodeOverrideConditionType::Node:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kAllowBipedNone);

								m_condParamEditor.SetNext<ConditionParamItem::CMENode>(
									e.s0,
									a_name);
								m_condParamEditor.SetNext<ConditionParamItem::BipedSlot>(
									e.bipedSlot);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								tdesc = UIL::LS(CommonStrings::Node);
								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::CMENode);

								break;
							case Data::NodeOverrideConditionType::Form:

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
							case Data::NodeOverrideConditionType::Keyword:

								m_condParamEditor.SetTempFlags(UIConditionParamEditorTempFlags::kNoClearKeyword);

								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								tdesc = UIL::LS(CommonStrings::Keyword);
								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Keyword);

								break;
							case Data::NodeOverrideConditionType::BipedSlot:

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
							case Data::NodeOverrideConditionType::Type:

								m_condParamEditor.SetNext<ConditionParamItem::EquipmentSlotExtra>(
									e.typeSlot);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::EquipmentSlotExtra);
								tdesc = UIL::LS(CommonStrings::Type);

								break;
							case Data::NodeOverrideConditionType::Race:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = UIL::LS(CommonStrings::Race);

								break;
							case Data::NodeOverrideConditionType::Actor:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = UIL::LS(CommonStrings::Actor);

								break;
							case Data::NodeOverrideConditionType::NPC:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = UIL::LS(CommonStrings::NPC);

								break;
							case Data::NodeOverrideConditionType::Furniture:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Furniture);
								tdesc = UIL::LS(CommonStrings::Furniture);

								break;
							case Data::NodeOverrideConditionType::Extra:

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

							case Data::NodeOverrideConditionType::Location:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.keyword.get_id());

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr);
								tdesc = UIL::LS(CommonStrings::Location);

								break;
							case Data::NodeOverrideConditionType::Worldspace:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = UIL::LS(CommonStrings::Worldspace);

								break;
							case Data::NodeOverrideConditionType::Package:

								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);
								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::PackageType>(
									e.procedureType);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::PackageType);
								tdesc = UIL::LS(CommonStrings::Package);

								break;

							case Data::NodeOverrideConditionType::Weather:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::WeatherClass>(
									e.weatherClass);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::WeatherClass);
								tdesc = UIL::LS(CommonStrings::Weather);

								break;

							case Data::NodeOverrideConditionType::Global:

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

							case Data::NodeOverrideConditionType::Mounting:
							case Data::NodeOverrideConditionType::Mounted:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Keyword>(
									e.form2.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Race>(
									e.keyword.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetFormKeywordExtraDesc(nullptr, true);

								tdesc = e.flags.bf().type == Data::NodeOverrideConditionType::Mounting ?
								            UIL::LS(UIWidgetCommonStrings::Mounting) :
								            UIL::LS(UIWidgetCommonStrings::Mounted);

								break;

							case Data::NodeOverrideConditionType::Idle:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = UIL::LS(CommonStrings::Idle);

								break;

							case Data::NodeOverrideConditionType::Skeleton:
								{
									m_condParamEditor.SetNext<ConditionParamItem::Extra>(
										e);

									auto& buffer = m_condParamEditor.GetDescBuffer();

									if (e.flags.test(Data::NodeOverrideConditionFlags::kExtraFlag1))
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

							case Data::NodeOverrideConditionType::Faction:

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

							case Data::NodeOverrideConditionType::Effect:

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

							case Data::NodeOverrideConditionType::Variable:

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

							case Data::NodeOverrideConditionType::Perk:

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

							case Data::NodeOverrideConditionType::Cell:

								m_condParamEditor.SetNext<ConditionParamItem::Form>(
									e.form.get_id());
								m_condParamEditor.SetNext<ConditionParamItem::LightingTemplateInheritanceFlags>(
									e.lightingTemplateInheritanceFlags);
								m_condParamEditor.SetNext<ConditionParamItem::Extra>(
									e);

								vdesc = m_condParamEditor.GetItemDesc(ConditionParamItem::Form);
								tdesc = UIL::LS(CommonStrings::Cell);

								break;

							case Data::NodeOverrideConditionType::Hand:

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
											e.flags.test(Data::NodeOverrideConditionFlags::kMatchLeftHand) ? "L, " : "",
											d,
											e.formType);
									}
									else
									{
										stl::snprintf(
											b,
											"%sT:%hhu",
											e.flags.test(Data::NodeOverrideConditionFlags::kMatchLeftHand) ? "L, " : "",
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
								vdesc = "N/A";
							}

							ImGui::TextUnformatted(tdesc);

							ImGui::TableSetColumnIndex(2);

							bool r = ImGui::Selectable(
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
								a_updateFunc();
							}
						}

						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

						ImGui::TableSetColumnIndex(3);

						if (ImGui::CheckboxFlagsT(
								"##oper_and",
								stl::underlying(std::addressof(e.flags.value)),
								stl::underlying(Data::NodeOverrideConditionFlags::kAnd)))
						{
							a_updateFunc();
						}

						ImGui::TableSetColumnIndex(4);

						if (ImGui::CheckboxFlagsT(
								"##oper_not",
								stl::underlying(std::addressof(e.flags.value)),
								stl::underlying(Data::NodeOverrideConditionFlags::kNot)))
						{
							a_updateFunc();
						}

						ImGui::PopStyleVar();

						++it;
						i++;
					}

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		void UINodeOverrideConditionWidget::DrawConditionTree(
			const stl::fixed_string&                 a_name,
			NodeOverrideDataType                     a_type,
			entryNodeOverrideData_t&                 a_data,
			Data::configNodeOverrideConditionList_t& a_entry,
			const bool                               a_exists,
			const bool                               a_ignoreNode,
			update_func_t                            a_updateFunc)
		{
			ImGui::PushID("ovr_cond_tree");

			const auto result = DrawConditionHeaderContextMenu(
				a_name,
				a_type,
				a_data,
				a_entry,
				a_exists,
				a_ignoreNode,
				a_updateFunc);

			bool empty = a_entry.empty();

			if (!empty)
			{
				if (result == NodeOverrideCommonAction::Insert ||
				    result == NodeOverrideCommonAction::PasteOver)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			UICommon::PushDisabled(empty);

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					UIL::LS(CommonStrings::Conditions)))
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawConditionTable(
						a_name,
						a_type,
						a_data,
						a_entry,
						a_exists,
						false,
						a_ignoreNode,
						a_updateFunc);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);

			ImGui::PopID();
		}

		bool UINodeOverrideConditionWidget::DrawConditionParamExtra(
			void*       a_p1,
			const void* a_p2)
		{
			auto match = static_cast<Data::configNodeOverrideCondition_t*>(a_p1);

			ImGui::PushID("match_param_extra");

			bool result = false;

			switch (match->flags.bf().type)
			{
			case Data::NodeOverrideConditionType::BipedSlot:

				DrawConditionParamExtra_BipedSlot(match, result);

				break;

			case Data::NodeOverrideConditionType::Type:
				if (match->flags.test_any(Data::NodeOverrideConditionFlags::kMatchAll))
				{
					const bool disabled =
						!match->flags.test(Data::NodeOverrideConditionFlags::kExtraFlag1);

					UICommon::PushDisabled(disabled);

					result |= ImGui::CheckboxFlagsT(
						"!##t_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kNegateMatch3));

					UICommon::PopDisabled(disabled);

					ImGui::SameLine();

					result |= ImGui::CheckboxFlagsT(
						UIL::LS(UINodeOverrideEditorWidgetStrings::IsBolt, "t_2"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kExtraFlag1));
				}

				[[fallthrough]];

			case Data::NodeOverrideConditionType::Keyword:
			case Data::NodeOverrideConditionType::Form:

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(CommonStrings::Equipped, "1"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kMatchEquipped));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(CommonStrings::Or, "2"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kMatchCategoryOperOR));

				ImGui::SameLine();

				result |= ImGui::CheckboxFlagsT(
					UIL::LS(CommonStrings::Displayed, "3"),
					stl::underlying(std::addressof(match->flags.value)),
					stl::underlying(Data::NodeOverrideConditionFlags::kMatchSlots));

				UITipsInterface::DrawTip(UITip::EquippedConditionsGearPositioning);

				if (!match->flags.test_any(Data::NodeOverrideConditionFlags::kMatchAll))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
					ImGui::TextWrapped("%s", UIL::LS(UINodeOverrideEditorWidgetStrings::CondMatchWarn));
					ImGui::PopStyleColor();
				}

				break;

			case Data::NodeOverrideConditionType::Node:

				{
					bool disabled = match->bipedSlot != BIPED_OBJECT::kNone;

					UICommon::PushDisabled(disabled);

					result |= ImGui::CheckboxFlagsT(
						UIL::LS(UIWidgetCommonStrings::IgnoreScabbards, "1"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Data::NodeOverrideConditionFlags::kExtraFlag0));

					UICommon::PopDisabled(disabled);
				}

				break;

			case Data::NodeOverrideConditionType::Furniture:

				DrawConditionParamExtra_Furniture(match, result);

				break;

			case Data::NodeOverrideConditionType::Location:

				DrawConditionParamExtra_Location(match, result);

				break;

			case Data::NodeOverrideConditionType::Worldspace:

				DrawConditionParamExtra_Worldspace(match, result);

				break;

			case Data::NodeOverrideConditionType::Race:

				DrawConditionParamExtra_Race(match, result);

				break;

			case Data::NodeOverrideConditionType::Idle:

				DrawConditionParamExtra_Idle(match, result);

				break;

			case Data::NodeOverrideConditionType::Skeleton:

				DrawConditionParamExtra_Skeleton(match, result);

				break;

			case Data::NodeOverrideConditionType::Mounting:

				DrawConditionParamExtra_Mounting(match, result);

				break;

			case Data::NodeOverrideConditionType::Cell:

				DrawConditionParamExtra_Cell(match, result);

				break;

			case Data::NodeOverrideConditionType::Hand:

				DrawConditionParamExtra_Hand(match, m_condParamEditor, result);

				break;

			case Data::NodeOverrideConditionType::NPC:

				DrawConditionParamExtra_NPC(match, result);

				break;

			case Data::NodeOverrideConditionType::Extra:

				DrawConditionParamExtra_Extra(match, result);

				break;
			}

			ImGui::PopID();

			return result;
		}

		bool UINodeOverrideConditionWidget::DrawConditionItemExtra(
			ConditionParamItem           a_item,
			ConditionParamItemExtraArgs& a_args)
		{
			bool result = false;

			ImGui::PushID("match_item_extra");

			DrawConditionItemExtra_All(a_item, a_args, result);

			ImGui::PopID();

			return result;
		}

		void UINodeOverrideConditionWidget::OnConditionItemChange(
			ConditionParamItem                    a_item,
			const ConditionParamItemOnChangeArgs& a_args)
		{
			OnConditionItemChangeImpl(a_item, a_args);
		}

		void UINodeOverrideConditionWidget::UpdateMatchParamAllowedTypes(
			Data::NodeOverrideConditionType a_type)
		{
			UpdateMatchParamAllowedTypesImpl(a_type, m_condParamEditor);
		}
	}
}