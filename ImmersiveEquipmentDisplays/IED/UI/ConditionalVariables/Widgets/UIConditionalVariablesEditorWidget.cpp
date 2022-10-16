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
				DrawEntryList(a_data, a_data.data, false);
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

		CondVarEntryResult UIConditionalVariablesEditorWidget::DrawEntryHeaderControls(
			Data::configConditionalVariablesHolder_t& a_holder,
			Data::configConditionalVariablesEntry_t&  a_data)
		{
			CondVarEntryResult result;

			ImGui::PushID("header_controls");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

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
				result = DrawEntryContextMenu(a_holder, a_data);

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		void UIConditionalVariablesEditorWidget::DrawNewItemContextItems(
			bool                a_insert,
			CondVarEntryResult& a_result)
		{
			if (LCG_BM(CommonStrings::New, "cv_ctx_new"))
			{
				if (LCG_BM(a_insert ? CommonStrings::Insert : CommonStrings::Add, "1"))
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

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(
						LS(CommonStrings::Paste, "2"),
						nullptr,
						false,
						static_cast<bool>(UIClipboard::Get<Data::configConditionalVariablesEntry_t>())))
				{
					a_result.action = CondVarEntryAction::kPaste;
				}

				ImGui::EndMenu();
			}
		}

		CondVarEntryResult UIConditionalVariablesEditorWidget::DrawEntryContextMenu(
			Data::configConditionalVariablesHolder_t& a_holder,
			Data::configConditionalVariablesEntry_t&  a_data)
		{
			CondVarEntryResult result;

			DrawNewItemContextItems(true, result);

			if (ImGui::MenuItem(LS(CommonStrings::Delete, "2")))
			{
				result.action = CondVarEntryAction::kDelete;
			}

			if (LCG_BM(CommonStrings::Rename, "3"))
			{
				if (DrawDescriptionPopup())
				{
					a_data.desc = GetDescriptionPopupBuffer();

					ClearDescriptionPopupBuffer();

					OnCondVarEntryChange({ a_holder });
				}

				ImGui::EndMenu();
			}

			return result;
		}

		void UIConditionalVariablesEditorWidget::DrawEntryList(
			Data::configConditionalVariablesHolder_t&    a_holder,
			Data::configConditionalVariablesEntryList_t& a_data,
			bool                                         a_isNested)
		{
			ImGui::PushID("elist");

			std::uint32_t i = 0;

			for (auto it = a_data.begin(); it != a_data.end();)
			{
				if (!a_isNested && !m_itemFilter.Test(*it->desc))
				{
					++it;
					continue;
				}

				ImGui::PushID(i);

				auto result = DrawEntryHeaderControls(a_holder, *it);

				switch (result.action)
				{
				case CondVarEntryAction::kAdd:
				case CondVarEntryAction::kAddGroup:

					if (!result.desc.empty())
					{
						it = a_data.emplace(it, result.desc);

						if (result.action == CondVarEntryAction::kAddGroup)
						{
							it->flags.set(Data::ConditionalVariablesEntryFlags::kIsGroup);
						}

						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });
					}

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

					if (auto clipData = UIClipboard::Get<Data::configConditionalVariablesEntry_t>())
					{
						it = a_data.emplace(it, *clipData);

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

					DrawEntryTree(a_holder, *it, a_isNested);

					++it;
					i++;
				}

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		bool UIConditionalVariablesEditorWidget::DrawVariable(
			Data::configConditionalVariable_t& a_var)
		{
			bool result = false;

			char buf[64];
			stl::snprintf(buf, "%s", a_var.name.c_str());

			if (ImGui::InputText(
					LS(CommonStrings::Name, "1"),
					buf,
					sizeof(buf),
					ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (buf[0] != 0x0)
				{
					a_var.name = buf;
					result |= true;
				}
			}

			if (DrawVariableTypeSelectorWidget(a_var.storage.type))
			{
				std::memset(
					a_var.storage.bytes,
					0x0,
					sizeof(a_var.storage.bytes));

				result = true;
			}

			switch (a_var.storage.type)
			{
			case ConditionalVariableType::kInt32:

				result |= ImGui::InputScalar(
					LS(CommonStrings::Value, "2"),
					ImGuiDataType_U32,
					a_var.storage.bytes,
					nullptr,
					nullptr,
					"%d",
					ImGuiInputTextFlags_CharsDecimal |
						ImGuiInputTextFlags_EnterReturnsTrue);

				break;

			case ConditionalVariableType::kFloat:

				result |= ImGui::InputScalar(
					LS(CommonStrings::Value, "2"),
					ImGuiDataType_Float,
					a_var.storage.bytes,
					nullptr,
					nullptr,
					"%f",
					ImGuiInputTextFlags_CharsDecimal |
					ImGuiInputTextFlags_EnterReturnsTrue);

				break;
			}

			return result;
		}

		void UIConditionalVariablesEditorWidget::DrawVariableTree(
			Data::configConditionalVariablesHolder_t& a_holder,
			Data::configConditionalVariablesList_t&   a_data,
			Data::configConditionalVariable_t&        a_var)
		{
			if (ImGui::TreeNodeEx(
					"tree_var",
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					a_var.name.c_str()))
			{
				ImGui::Spacing();

				if (DrawVariable(a_var))
				{
					OnCondVarEntryChange(
						{ a_holder,
					      CondVarEntryChangeAction::kReset });
				}

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		CondVarEntryResult UIConditionalVariablesEditorWidget::DrawVariableHeaderContextMenu(
			Data::configConditionalVariablesHolder_t& a_holder,
			Data::configConditionalVariablesList_t&   a_data,
			Data::configConditionalVariable_t&        a_var)
		{
			CondVarEntryResult result;

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Insert, "1"))
				{
					if (LCG_BM(CommonStrings::New, "1"))
					{
						if (DrawDescriptionPopup())
						{
							result.action = CondVarEntryAction::kAdd;
							result.desc   = GetDescriptionPopupBuffer();

							ClearDescriptionPopupBuffer();
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem(
							LS(CommonStrings::Paste, "2"),
							nullptr,
							false,
							static_cast<bool>(UIClipboard::Get<Data::configConditionalVariable_t>())))
					{
						result.action = CondVarEntryAction::kPaste;

						ImGui::CloseCurrentPopup();
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
			Data::configConditionalVariablesList_t&   a_data)
		{
			ImGui::PushID("varlist_tree");

			auto result = DrawVariableListHeaderContextMenu(a_holder, a_data);

			switch (result.action)
			{
			case CondVarEntryAction::kAdd:
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
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					LS(CommonStrings::Variables)))
			{
				ImGui::Spacing();

				DrawVariableList(a_holder, a_data);

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

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::New, "1"))
				{
					if (DrawDescriptionPopup())
					{
						auto it = a_data.end();

						if (CreateNewVariable(
								GetDescriptionPopupBuffer(),
								a_data,
								it))
						{
							OnCondVarEntryChange(
								{ a_holder,
							      CondVarEntryChangeAction::kReset });
						}

						result.action = CondVarEntryAction::kAdd;

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "A")))
				{
					UIClipboard::Set(a_data);
				}

				if (ImGui::MenuItem(
						LS(CommonStrings::Paste, "B"),
						nullptr,
						false,
						static_cast<bool>(UIClipboard::Get<Data::configConditionalVariable_t>())))
				{
					if (auto data = UIClipboard::Get<Data::configConditionalVariable_t>())
					{
						a_data.emplace_back(*data);

						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });

						result.action = CondVarEntryAction::kPaste;
					}
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
			Data::configConditionalVariablesList_t&   a_data)
		{
			ImGui::PushID("var_list");

			std::uint32_t i = 0;

			for (auto it = a_data.begin(); it != a_data.end();)
			{
				ImGui::PushID(i);

				auto result = DrawVariableHeaderContextMenu(a_holder, a_data, *it);

				switch (result.action)
				{
				case CondVarEntryAction::kAdd:

					if (CreateNewVariable(
							result.desc,
							a_data,
							it))
					{
						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });

						ImGui::SetNextItemOpen(true);
					}

					break;

				case CondVarEntryAction::kDelete:

					it = a_data.erase(it);

					OnCondVarEntryChange(
						{ a_holder,
					      CondVarEntryChangeAction::kReset });

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

					DrawVariableTree(a_holder, a_data, *it);

					i++;
					++it;
				}

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		CondVarEntryResult UIConditionalVariablesEditorWidget::DrawGroupHeaderContextMenu()
		{
			CondVarEntryResult result;

			ImGui::PushID("header_controls");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				DrawNewItemContextItems(false, result);

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		void UIConditionalVariablesEditorWidget::DrawEntryTree(
			Data::configConditionalVariablesHolder_t& a_holder,
			Data::configConditionalVariablesEntry_t&  a_data,
			bool                                      a_isNested)
		{
			ImGui::PushID("content");

			if (ImGui::TreeNodeEx(
					"tree_entry",
					ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					a_data.desc.c_str()))
			{
				ImGui::Spacing();

				DrawEquipmentOverrideConditionTree(
					a_data.conditions,
					[&] {
						OnCondVarEntryChange(
							{ a_holder,
					          CondVarEntryChangeAction::kReset });
					});

				if (a_data.flags.test(Data::ConditionalVariablesEntryFlags::kIsGroup))
				{
					DrawGroupEntryList(a_holder, a_data.group);
				}
				else
				{
					DrawVariableListTree(a_holder, a_data.vars);
				}

				ImGui::Spacing();

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		void UIConditionalVariablesEditorWidget::DrawGroupEntryList(
			Data::configConditionalVariablesHolder_t&    a_holder,
			Data::configConditionalVariablesEntryList_t& a_data)
		{
			ImGui::PushID("group");

			auto result = DrawGroupHeaderContextMenu();

			switch (result.action)
			{
			case CondVarEntryAction::kAdd:
			case CondVarEntryAction::kAddGroup:
				{
					auto& r = a_data.emplace_back(result.desc);

					if (result.action == CondVarEntryAction::kAddGroup)
					{
						r.flags.set(Data::ConditionalVariablesEntryFlags::kIsGroup);
					}

					OnCondVarEntryChange(
						{ a_holder,
					      CondVarEntryChangeAction::kReset });

					ImGui::SetNextItemOpen(true);
				}
				break;

			case CondVarEntryAction::kPaste:

				if (auto clipData = UIClipboard::Get<Data::configConditionalVariablesEntry_t>())
				{
					a_data.emplace_back(*clipData);

					OnCondVarEntryChange(
						{ a_holder,
					      CondVarEntryChangeAction::kReset });
				}

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
					LS(CommonStrings::Group)))
			{
				DrawEntryList(a_holder, a_data, true);
				ImGui::TreePop();
			}

			UICommon::PopDisabled(disabled);

			ImGui::PopID();
		}

		bool UIConditionalVariablesEditorWidget::CreateNewVariable(
			const std::string&                                a_name,
			Data::configConditionalVariablesList_t&           a_list,
			Data::configConditionalVariablesList_t::iterator& a_where)
		{
			if (a_name.empty())
			{
				return false;
			}

			stl::fixed_string name = a_name;

			auto itn = std::find_if(
				a_list.cbegin(),
				a_list.cend(),
				[&](auto& a_v) {
					return a_v.name == name;
				});

			if (itn == a_list.cend())
			{
				a_where = a_list.emplace(
					a_where,
					ConditionalVariableType::kInt32,
					std::move(name));

				return true;
			}
			else
			{
				return false;
			}
		}

		void UIConditionalVariablesEditorWidget::DrawMainHeaderControlsExtra(
			Data::configConditionalVariablesHolder_t& a_data)
		{
		}

		void UIConditionalVariablesEditorWidget::EditorDrawMenuBarItems()
		{
			CondVarEntryResult result;

			DrawNewItemContextItems(false, result);

			switch (result.action)
			{
			case CondVarEntryAction::kAdd:
			case CondVarEntryAction::kAddGroup:
				{
					if (auto data = GetCurrentData())
					{
						auto& r = data->get().data.emplace_back(result.desc);

						if (result.action == CondVarEntryAction::kAddGroup)
						{
							r.flags.set(Data::ConditionalVariablesEntryFlags::kIsGroup);
						}

						OnCondVarEntryChange(
							{ *data,
						      CondVarEntryChangeAction::kReset });
					}
				}

				break;

			case CondVarEntryAction::kPaste:

				if (auto clipData = UIClipboard::Get<Data::configConditionalVariablesEntry_t>())
				{
					if (auto data = GetCurrentData())
					{
						data->get().data.emplace_back(*clipData);

						OnCondVarEntryChange(
							{ *data,
						      CondVarEntryChangeAction::kReset });
					}
				}

				break;
			}
		}
	}
}