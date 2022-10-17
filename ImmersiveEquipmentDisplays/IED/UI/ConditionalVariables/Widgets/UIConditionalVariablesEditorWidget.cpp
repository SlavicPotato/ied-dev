#include "pch.h"

#include "IED/Controller/Controller.h"

#include "UIConditionalVariablesEditorWidget.h"

#include "IED/UI/UIClipboard.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"

#include "UIConditionalVariablesEditorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		UIConditionalVariablesEditorWidget::UIConditionalVariablesEditorWidget(
			Controller& a_controller) :
			UIEquipmentOverrideConditionsWidget(a_controller),
			UIVariableTypeSelectorWidget(a_controller),
			m_itemFilter(true)
		{
		}

		void UIConditionalVariablesEditorWidget::DrawCondVarEditor(
			Data::configConditionalVariablesHolder_t& a_data)
		{
			ImGui::PushID("cvar_editor");

			ImGui::PushID("extra_header");

			DrawMainHeaderControlsExtra(a_data);

			ImGui::PopID();

			DrawItemFilter();

			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::BeginChild(
					"cvar_editor_panel",
					{ -1.0f, 0.0f },
					false,
					ImGuiWindowFlags_HorizontalScrollbar))
			{
				DrawEntryList(a_data, a_data.data);
			}

			ImGui::EndChild();

			ImGui::PopID();
		}

		void UIConditionalVariablesEditorWidget::DrawItemFilter()
		{
			if (Tree(LS(CommonStrings::Filter, "item_filter"), false))
			{
				ImGui::PushItemWidth(ImGui::GetFontSize() * -8.0f);

				ImGui::Spacing();

				m_itemFilter.Draw();

				ImGui::Spacing();

				ImGui::PopItemWidth();

				ImGui::TreePop();
			}
		}

		CondVarEntryResult UIConditionalVariablesEditorWidget::DrawEntryHeaderContextMenu(
			Data::configConditionalVariablesHolder_t&         a_holder,
			Data::configConditionalVariablesEntryListValue_t& a_data)
		{
			CondVarEntryResult result;

			ImGui::PushID("header_controls");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Rename, "1"))
				{
					if (DrawDescriptionPopup())
					{
						result.desc   = GetDescriptionPopupBuffer();
						result.action = CondVarEntryAction::kRename;

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Delete, "2")))
				{
					result.action = CondVarEntryAction::kDelete;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "A")))
				{
					UIClipboard::Set(a_data);
				}

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "B"),
						nullptr,
						false,
						static_cast<bool>(UIClipboard::Get<Data::configConditionalVariablesEntryListValue_t>())))
				{
					if (auto data = UIClipboard::Get<Data::configConditionalVariablesEntryListValue_t>())
					{
						a_data = *data;

						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });

						result.action = CondVarEntryAction::kPasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		void UIConditionalVariablesEditorWidget::DrawNewContextItem(
			bool                a_insert,
			CondVarEntryResult& a_result)
		{
			if (LCG_BM(a_insert ? CommonStrings::Insert : CommonStrings::Add, "cv_ctx_new"))
			{
				if (LCG_BM(CommonStrings::Item, "1"))
				{
					if (DrawDescriptionPopup())
					{
						a_result.action = CondVarEntryAction::kAdd;
						a_result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				if (LCG_BM(CommonStrings::Group, "2"))
				{
					if (DrawDescriptionPopup())
					{
						a_result.action = CondVarEntryAction::kAddGroup;
						a_result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

				if (ImGui::MenuItem(
						LS(CommonStrings::Paste, "3"),
						nullptr,
						false,
						static_cast<bool>(UIClipboard::Get<Data::configConditionalVariable_t>())))
				{
					a_result.action = CondVarEntryAction::kPaste;
				}

				ImGui::EndMenu();
			}
		}

		void UIConditionalVariablesEditorWidget::DrawEntryList(
			Data::configConditionalVariablesHolder_t&    a_holder,
			Data::configConditionalVariablesEntryList_t& a_data)
		{
			ImGui::PushID("elist");

			for (auto it = a_data.begin(); it != a_data.end();)
			{
				if (!m_itemFilter.Test(*it->first))
				{
					++it;
					continue;
				}

				ImGui::PushID(it->first.c_str());

				auto result = DrawEntryHeaderContextMenu(a_holder, *it);

				switch (result.action)
				{
				case CondVarEntryAction::kDelete:

					it = a_data.erase(it);

					OnCondVarEntryChange(
						{ a_holder,
					      CondVarEntryChangeAction::kReset });

					break;

				case CondVarEntryAction::kRename:

					{
						stl::fixed_string newName = result.desc;

						if (!a_data.contains(newName))
						{
							auto tmp = std::move(it->second);

							a_data.erase(it);

							it = a_data.emplace(std::move(newName), std::move(tmp)).first;

							OnCondVarEntryChange(
								{ a_holder,
							      CondVarEntryChangeAction::kReset });
						}
					}

					break;

				case CondVarEntryAction::kPasteOver:

					ImGui::SetNextItemOpen(true);

					break;
				}

				if (it != a_data.end())
				{
					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					DrawEntryTree(a_holder, *it);

					++it;
				}

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		bool UIConditionalVariablesEditorWidget::DrawVariableValue(
			ConditionalVariableType     a_type,
			conditionalVariableValue_t& a_value)
		{
			switch (a_type)
			{
			case ConditionalVariableType::kInt32:

				return ImGui::InputScalar(
					LS(CommonStrings::Value, "2"),
					ImGuiDataType_U32,
					a_value.bytes,
					nullptr,
					nullptr,
					"%d",
					ImGuiInputTextFlags_CharsDecimal |
						ImGuiInputTextFlags_EnterReturnsTrue);

				break;

			case ConditionalVariableType::kFloat:

				return ImGui::InputScalar(
					LS(CommonStrings::Value, "2"),
					ImGuiDataType_Float,
					a_value.bytes,
					nullptr,
					nullptr,
					"%f",
					ImGuiInputTextFlags_CharsDecimal |
						ImGuiInputTextFlags_EnterReturnsTrue);

				break;

			default:

				return false;
			}
		}

		void UIConditionalVariablesEditorWidget::DrawVariableTree(
			Data::configConditionalVariablesHolder_t& a_holder,
			Data::configConditionalVariablesEntry_t&  a_entry,
			Data::configConditionalVariable_t&        a_var)
		{
			if (ImGui::TreeNodeEx(
					"tree_var",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					a_var.desc.c_str()))
			{
				ImGui::Spacing();

				DrawEquipmentOverrideConditionTree(
					a_var.conditions,
					[&] {
						OnCondVarEntryChange(
							{ a_holder,
					          CondVarEntryChangeAction::kReset });
					});

				if (a_var.flags.test(Data::ConditionalVariableFlags::kIsGroup))
				{
					ImGui::PushID("group");

					if (ImGui::CheckboxFlagsT(
							LS(CommonStrings::Continue, "ctl_0"),
							stl::underlying(std::addressof(a_var.flags.value)),
							stl::underlying(Data::ConditionalVariableFlags::kContinue)))
					{
						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });
					}
					DrawTip(UITip::EquipmentOverrideGroupContinue);

					ImGui::Spacing();

					DrawVariableListTree(a_holder, a_entry, a_var.group);

					ImGui::PopID();
				}
				else
				{
					ImGui::Spacing();

					if (DrawVariableValue(a_entry.defaultValue.type, a_var.value))
					{
						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });
					}
				}

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		CondVarEntryResult UIConditionalVariablesEditorWidget::DrawVariableHeaderControls(
			Data::configConditionalVariablesHolder_t& a_holder,
			Data::configConditionalVariablesList_t&   a_data,
			Data::configConditionalVariable_t&        a_var)
		{
			CondVarEntryResult result;

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = CondVarEntryAction::kSwap;
				result.dir    = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = CondVarEntryAction::kSwap;
				result.dir    = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				DrawNewContextItem(true, result);

				if (LCG_BM(CommonStrings::Rename, "1"))
				{
					if (DrawDescriptionPopup())
					{
						a_var.desc = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();

						OnCondVarEntryChange({ a_holder });
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Delete, "2")))
				{
					result.action = CondVarEntryAction::kDelete;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "A")))
				{
					UIClipboard::Set(a_var);
				}

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "B"),
						nullptr,
						false,
						static_cast<bool>(UIClipboard::Get<Data::configConditionalVariable_t>())))
				{
					if (auto data = UIClipboard::Get<Data::configConditionalVariable_t>())
					{
						a_var = *data;

						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });

						result.action = CondVarEntryAction::kPasteOver;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		void UIConditionalVariablesEditorWidget::DrawVariableListTree(
			Data::configConditionalVariablesHolder_t& a_holder,
			Data::configConditionalVariablesEntry_t&  a_entry,
			Data::configConditionalVariablesList_t&   a_data)
		{
			ImGui::PushID("varlist_tree");

			auto result = DrawVariableListHeaderContextMenu(a_holder, a_data);

			switch (result.action)
			{
			case CondVarEntryAction::kAdd:
			case CondVarEntryAction::kAddGroup:
			case CondVarEntryAction::kPaste:
			case CondVarEntryAction::kPasteOver:

				ImGui::SetNextItemOpen(true);

				break;
			}

			const bool disabled = a_data.empty();

			if (disabled)
			{
				ImGui::SetNextItemOpen(false);
			}

			UICommon::PushDisabled(disabled);

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(CommonStrings::Overrides)))
			{
				ImGui::Spacing();

				DrawVariableList(a_holder, a_entry, a_data);

				ImGui::Spacing();

				ImGui::TreePop();
			}

			UICommon::PopDisabled(disabled);

			ImGui::PopID();
		}

		CondVarEntryResult UIConditionalVariablesEditorWidget::DrawVariableListHeaderContextMenu(
			Data::configConditionalVariablesHolder_t& a_holder,
			Data::configConditionalVariablesList_t&   a_data)
		{
			CondVarEntryResult result;

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				DrawNewContextItem(false, result);

				switch (result.action)
				{
				case CondVarEntryAction::kAdd:
				case CondVarEntryAction::kAddGroup:
					{
						auto& r = a_data.emplace_back(result.desc);

						if (result.action == CondVarEntryAction::kAddGroup)
						{
							r.flags.set(Data::ConditionalVariableFlags::kIsGroup);
						}

						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });
					}

					break;

				case CondVarEntryAction::kPaste:

					if (auto clipData = UIClipboard::Get<Data::configConditionalVariable_t>())
					{
						if (auto data = GetCurrentData())
						{
							a_data.emplace_back(*clipData);

							OnCondVarEntryChange(
								{ *data,
							      CondVarEntryChangeAction::kReset });
						}
					}

					break;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "A")))
				{
					UIClipboard::Set(a_data);
				}

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "C"),
						nullptr,
						false,
						static_cast<bool>(UIClipboard::Get<Data::configConditionalVariablesList_t>())))
				{
					if (auto data = UIClipboard::Get<Data::configConditionalVariablesList_t>())
					{
						a_data = *data;

						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });

						result.action = CondVarEntryAction::kPasteOver;
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem(
						LS(CommonStrings::Clear, "D"),
						nullptr,
						false,
						!a_data.empty()))
				{
					a_data.clear();

					OnCondVarEntryChange(
						{ a_holder,
					      CondVarEntryChangeAction::kReset });

					result.action = CondVarEntryAction::kClear;
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		void UIConditionalVariablesEditorWidget::DrawVariableList(
			Data::configConditionalVariablesHolder_t& a_holder,
			Data::configConditionalVariablesEntry_t&  a_entry,
			Data::configConditionalVariablesList_t&   a_data)
		{
			ImGui::PushID("var_list");

			std::uint32_t i = 0;

			for (auto it = a_data.begin(); it != a_data.end();)
			{
				ImGui::PushID(i);

				auto result = DrawVariableHeaderControls(a_holder, a_data, *it);

				switch (result.action)
				{
				case CondVarEntryAction::kAdd:
				case CondVarEntryAction::kAddGroup:

					it = a_data.emplace(it, result.desc);

					if (result.action == CondVarEntryAction::kAddGroup)
					{
						it->flags.set(Data::ConditionalVariableFlags::kIsGroup);
					}

					OnCondVarEntryChange(
						{ a_holder,
					      CondVarEntryChangeAction::kReset });

					ImGui::SetNextItemOpen(true);

					break;

				case CondVarEntryAction::kDelete:

					it = a_data.erase(it);

					OnCondVarEntryChange(
						{ a_holder,
					      CondVarEntryChangeAction::kReset });

					break;

				case CondVarEntryAction::kSwap:

					if (IterSwap(a_data, it, result.dir))
					{
						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });
					}

					break;

				case CondVarEntryAction::kPaste:

					if (auto data = UIClipboard::Get<Data::configConditionalVariable_t>())
					{
						it = a_data.emplace(it, *data);

						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });
					}

					[[fallthrough]];

				case CondVarEntryAction::kPasteOver:

					ImGui::SetNextItemOpen(true);

					break;
				}

				if (it != a_data.end())
				{
					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					DrawVariableTree(a_holder, a_entry, *it);

					i++;
					++it;
				}

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		void UIConditionalVariablesEditorWidget::DrawEntryTree(
			Data::configConditionalVariablesHolder_t&         a_holder,
			Data::configConditionalVariablesEntryListValue_t& a_data)
		{
			ImGui::PushID("content");

			if (ImGui::TreeNodeEx(
					"tree_entry",
					ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					a_data.first.c_str()))
			{
				ImGui::Spacing();

				if (a_data.second.vars.empty())
				{
					if (DrawVariableTypeSelectorWidget(
							a_data.second.defaultValue.type))
					{
						std::memset(
							a_data.second.defaultValue.bytes,
							0x0,
							sizeof(a_data.second.defaultValue.bytes));

						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });
					}
				}
				else
				{
					ImGui::Text(
						"%s: %s",
						LS(CommonStrings::Type),
						variable_type_to_desc(
							a_data.second.defaultValue.type));
				}

				if (DrawVariableValue(
						a_data.second.defaultValue.type,
						a_data.second.defaultValue))
				{
					OnCondVarEntryChange(
						{ a_holder,
					      CondVarEntryChangeAction::kReset });
				}

				ImGui::Spacing();

				DrawVariableListTree(
					a_holder,
					a_data.second,
					a_data.second.vars);

				ImGui::Spacing();

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		void UIConditionalVariablesEditorWidget::DrawMainHeaderControlsExtra(
			Data::configConditionalVariablesHolder_t& a_data)
		{
		}

		void UIConditionalVariablesEditorWidget::EditorDrawMenuBarItems()
		{
			if (LCG_BM(CommonStrings::Add, "1"))
			{
				DrawVariableTypeSelectorWidget(m_tmpType);

				if (DrawDescriptionPopup())
				{
					if (auto data = GetCurrentData())
					{
						auto r = data->get().data.try_emplace(
							GetDescriptionPopupBuffer(),
							m_tmpType);

						if (r.second)
						{
							OnCondVarEntryChange(
								{ *data,
							      CondVarEntryChangeAction::kReset });
						}
					}

					ClearDescriptionPopupBuffer();
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem(
					LS(CommonStrings::Paste, "2"),
					nullptr,
					false,
					static_cast<bool>(UIClipboard::Get<Data::configConditionalVariablesEntryListValue_t>())))

			{
				if (auto clipData = UIClipboard::Get<Data::configConditionalVariablesEntryListValue_t>())
				{
					if (auto data = GetCurrentData())
					{
						auto r = data->get().data.emplace(*clipData);

						if (r.second)
						{
							OnCondVarEntryChange(
								{ *data,
							      CondVarEntryChangeAction::kReset });
						}
					}
				}
			}
		}
	}
}