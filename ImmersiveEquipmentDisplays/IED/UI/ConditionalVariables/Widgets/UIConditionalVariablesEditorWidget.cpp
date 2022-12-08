#include "pch.h"

#include "IED/Controller/Controller.h"

#include "UIConditionalVariablesEditorWidget.h"

#include "IED/UI/UIAllowedModelTypes.h"
#include "IED/UI/UIClipboard.h"
#include "IED/UI/UIFormBrowserCommonFilters.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"

#include "UIConditionalVariablesEditorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		UIConditionalVariablesEditorWidget::UIConditionalVariablesEditorWidget(
			Controller& a_controller) :
			UILastEquippedWidget(a_controller),
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
			if (Tree(UIL::LS(CommonStrings::Filter, "item_filter"), false))
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
				if (UIL::LCG_BM(CommonStrings::Rename, "1"))
				{
					if (DrawDescriptionPopup())
					{
						result.desc   = GetDescriptionPopupBuffer();
						result.action = CondVarEntryAction::kRename;

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Delete, "2")))
				{
					result.action = CondVarEntryAction::kDelete;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "A")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configConditionalVariablesEntryListValue_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "B"),
						nullptr,
						false,
						static_cast<bool>(clipData)))
				{
					if (clipData)
					{
						a_data.second = clipData->second;

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
			if (UIL::LCG_BM(a_insert ? CommonStrings::Insert : CommonStrings::Add, "cv_ctx_new"))
			{
				if (UIL::LCG_BM(CommonStrings::Item, "1"))
				{
					if (DrawDescriptionPopup())
					{
						a_result.action = CondVarEntryAction::kAdd;
						a_result.desc   = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_BM(CommonStrings::Group, "2"))
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
						UIL::LS(CommonStrings::Paste, "3"),
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

			auto& vec = a_data.getvec();

			for (auto it = vec.begin(); it != vec.end();)
			{
				auto& e = *it;

				if (!m_itemFilter.Test(*e->first))
				{
					++it;
					continue;
				}

				ImGui::PushID(e->first.c_str());

				const auto result = DrawEntryHeaderContextMenu(a_holder, *e);

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
							auto tmp = std::make_unique<Data::configConditionalVariablesEntry_t>(std::move(e->second));

							it = a_data.emplace_vec(
										   a_data.erase(it),
										   std::move(newName),
										   std::move(*tmp))
							         .first;

							OnCondVarEntryChange(
								{ a_holder,
							      CondVarEntryChangeAction::kReset });
						}
					}

					break;

				case CondVarEntryAction::kSwap:

					if (IterSwap(vec, it, result.dir))
					{
						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });
					}

					break;

				case CondVarEntryAction::kPasteOver:

					ImGui::SetNextItemOpen(true);

					break;
				}

				if (it != vec.end())
				{
					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					DrawEntryTree(a_holder, *(*it));

					++it;
				}

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		void UIConditionalVariablesEditorWidget::DrawCurrentVariableValue(
			Data::configConditionalVariablesEntryListValue_t& a_data)
		{
		}

		bool UIConditionalVariablesEditorWidget::DrawVariableValue(
			ConditionalVariableType                     a_type,
			Data::configConditionalVariableValueData_t& a_data)
		{
			bool result;

			ImGui::PushID("value");

			switch (a_type)
			{
			case ConditionalVariableType::kInt32:

				result = ImGui::InputScalar(
					UIL::LS(CommonStrings::Value, "1"),
					ImGuiDataType_U32,
					a_data.value.primitives,
					nullptr,
					nullptr,
					"%d",
					ImGuiInputTextFlags_CharsDecimal |
						ImGuiInputTextFlags_EnterReturnsTrue);

				break;

			case ConditionalVariableType::kFloat:

				result = ImGui::InputScalar(
					UIL::LS(CommonStrings::Value, "1"),
					ImGuiDataType_Float,
					a_data.value.primitives,
					nullptr,
					nullptr,
					"%f",
					ImGuiInputTextFlags_CharsDecimal |
						ImGuiInputTextFlags_EnterReturnsTrue);

				break;

			case ConditionalVariableType::kForm:

				result = DrawVariableForm(a_type, a_data);

				break;

			default:

				result = false;

				break;
			}

			ImGui::PopID();

			return result;
		}

		bool UIConditionalVariablesEditorWidget::DrawVariableForm(
			ConditionalVariableType                     a_type,
			Data::configConditionalVariableValueData_t& a_data)
		{
			bool result = false;

			result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::LastEquipped, "0"),
				stl::underlying(std::addressof(a_data.flags.value)),
				stl::underlying(Data::ConditionalVariableValueDataFlags::kLastEquipped));

			if (a_data.flags.test(Data::ConditionalVariableValueDataFlags::kLastEquipped))
			{
				DrawLastEquippedPanel(a_data.lastEquipped, [&] {
					result = true;
				});
			}
			else
			{
				auto& fp = m_condParamEditor.GetFormPicker();

				fp.SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::ModelTypes));
				fp.SetFormBrowserEnabled(true);

				result |= fp.DrawFormPicker("1", static_cast<Localization::StringID>(CommonStrings::Form), a_data.value.form);
			}

			return result;
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
							UIL::LS(CommonStrings::Continue, "ctl_0"),
							stl::underlying(std::addressof(a_var.flags.value)),
							stl::underlying(Data::ConditionalVariableFlags::kContinue)))
					{
						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });
					}
					UITipsInterface::DrawTip(UITip::EquipmentOverrideGroupContinue);

					ImGui::Spacing();

					DrawVariableListTree(a_holder, a_entry, a_var.group);

					ImGui::PopID();
				}
				else
				{
					ImGui::Spacing();

					if (DrawVariableValue(a_entry.defaultValue.value.type, a_var.value))
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

				if (UIL::LCG_BM(CommonStrings::Rename, "1"))
				{
					if (DrawDescriptionPopup())
					{
						a_var.desc = GetDescriptionPopupBuffer();

						ClearDescriptionPopupBuffer();

						OnCondVarEntryChange({ a_holder });
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Delete, "2")))
				{
					result.action = CondVarEntryAction::kDelete;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "A")))
				{
					UIClipboard::Set(a_var);
				}

				auto clipData = UIClipboard::Get<Data::configConditionalVariable_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "B"),
						nullptr,
						false,
						static_cast<bool>(clipData)))
				{
					if (clipData)
					{
						a_var = *clipData;

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
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen,
					"%s",
					UIL::LS(CommonStrings::Overrides)))
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

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "A")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configConditionalVariablesList_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "C"),
						nullptr,
						false,
						static_cast<bool>(clipData)))
				{
					if (clipData)
					{
						a_data = *clipData;

						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });

						result.action = CondVarEntryAction::kPasteOver;
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Clear, "D"),
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

				ImGui::PushItemWidth(ImGui::GetFontSize() * -6.0f);

				if (a_data.second.vars.empty())
				{
					if (UIVariableTypeSelectorWidget::DrawVariableTypeSelectorWidget(
							a_data.second.defaultValue.value.type))
					{
						static_cast<conditionalVariableValue_t&>(a_data.second.defaultValue.value) = {};

						OnCondVarEntryChange(
							{ a_holder,
						      CondVarEntryChangeAction::kReset });
					}
				}
				else
				{
					ImGui::Text(
						"%s: %s",
						UIL::LS(CommonStrings::Type),
						UIVariableTypeSelectorWidget::variable_type_to_desc(
							a_data.second.defaultValue.value.type));
				}

				if (DrawVariableValue(
						a_data.second.defaultValue.value.type,
						a_data.second.defaultValue))
				{
					OnCondVarEntryChange(
						{ a_holder,
					      CondVarEntryChangeAction::kReset });
				}

				ImGui::PopItemWidth();

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
			if (UIL::LCG_BM(CommonStrings::Add, "1"))
			{
				UIVariableTypeSelectorWidget::DrawVariableTypeSelectorWidget(m_tmpType);

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

			const auto clipData = UIClipboard::Get<Data::configConditionalVariablesEntryListValue_t>();

			if (ImGui::MenuItem(
					UIL::LS(CommonStrings::Paste, "2"),
					nullptr,
					false,
					static_cast<bool>(clipData)))

			{
				if (clipData)
				{
					if (auto data = GetCurrentData())
					{
						auto& v = data->get().data;

						bool update;

						if (v.contains(clipData->first))
						{
							update = v.emplace(
										  *clipData->first + " (1)",
										  clipData->second)
							             .second;
						}
						else
						{
							update = v.emplace(*clipData).second;
						}

						if (update)
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