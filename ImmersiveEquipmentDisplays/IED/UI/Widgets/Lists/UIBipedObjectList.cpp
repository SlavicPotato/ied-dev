#include "pch.h"

#include "UIBipedObjectList.h"

#include "IED/UI/Widgets/UIBipedObjectSelectorWidget.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"

#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		UIBipedObjectList::UIBipedObjectList(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIBipedObjectList::DrawBipedObjectTree(
			Data::configBipedObjectList_t& a_data,
			const std::function<bool()>&   a_extra)
		{
			ImGui::PushID("bip_obj");

			bool result = false;

			const auto hr = DrawHeaderContextMenu(a_data);

			bool treeDisabled = a_data.empty();

			switch (hr)
			{
			case UIBipedObjectListContextAction::Add:
			case UIBipedObjectListContextAction::Paste:
				if (!treeDisabled)
				{
					ImGui::SetNextItemOpen(true);
				}
				[[fallthrough]];
			case UIBipedObjectListContextAction::Clear:
				result = true;
				break;
			}

			UICommon::PushDisabled(treeDisabled);

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(UIWidgetCommonStrings::BipedSlots)))
			{
				if (!treeDisabled)
				{
					ImGui::Spacing();
					ImGui::Indent();

					ImGui::PushID("extra");
					result |= a_extra();
					ImGui::PopID();

					result |= DrawTable(a_data);

					ImGui::Unindent();
					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(treeDisabled);

			ImGui::PopID();

			return result;
		}

		bool UIBipedObjectList::DrawBipedObjectList(
			Data::configBipedObjectList_t& a_data,
			const std::function<void()>&   a_extra)
		{
		}

		UIBipedObjectListContextAction UIBipedObjectList::DrawHeaderContextMenu(
			Data::configBipedObjectList_t& a_data)
		{
			UIBipedObjectListContextAction result{
				UIBipedObjectListContextAction::None
			};

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_newObject = BIPED_OBJECT::kNone;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Add, "1"))
				{
					if (UIBipedObjectSelectorWidget::DrawBipedObjectSelector("##0", m_newObject))
					{
						if (m_newObject != BIPED_OBJECT::kNone)
						{
							auto it = std::find(a_data.begin(), a_data.end(), m_newObject);
							if (it == a_data.end())
							{
								a_data.emplace_back(m_newObject);

								result = UIBipedObjectListContextAction::Add;
							}
						}

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (LCG_MI(UIWidgetCommonStrings::ClearAll, "2"))
				{
					a_data.clear();
					result = UIBipedObjectListContextAction::Clear;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "3")))
				{
					UIClipboard::Set<Data::configBipedObjectList_t>(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configBipedObjectList_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::PasteOver, "4"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;
						result = UIBipedObjectListContextAction::Paste;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		UIBipedObjectListContextResult UIBipedObjectList::DrawEntryContextMenu()
		{
			UIBipedObjectListContextResult result;

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_newObject = BIPED_OBJECT::kNone;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = UIBipedObjectListContextAction::Swap;
				result.dir    = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = UIBipedObjectListContextAction::Swap;
				result.dir    = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Insert, "1"))
				{
					if (UIBipedObjectSelectorWidget::DrawBipedObjectSelector("##0", m_newObject))
					{
						result.object = m_newObject;
						result.action = UIBipedObjectListContextAction::Add;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (LCG_MI(CommonStrings::Delete, "2"))
				{
					result.action = UIBipedObjectListContextAction::Clear;
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		bool UIBipedObjectList::DrawTable(
			Data::configBipedObjectList_t& a_data)
		{
			if (a_data.empty())
			{
				return false;
			}

			bool result = false;

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.f, 4.f });

			constexpr int NUM_COLUMNS = 2;

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
					(ImGui::GetFontSize() + ImGui::GetStyle().ItemInnerSpacing.x) * 3.0f + 2.0f;

				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, w);
				ImGui::TableSetupColumn(LS(CommonStrings::Biped), ImGuiTableColumnFlags_None, 250.0f);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				auto it = a_data.begin();

				int i = 0;

				while (it != a_data.end())
				{
					ImGui::PushID(i);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					const auto hr = DrawEntryContextMenu();

					switch (hr.action)
					{
					case UIBipedObjectListContextAction::Add:

						if (hr.object != BIPED_OBJECT::kNone)
						{
							if (std::find(a_data.begin(), a_data.end(), hr.object) == a_data.end())
							{
								it     = a_data.emplace(it, hr.object);
								result = true;
							}
						}

						break;
					case UIBipedObjectListContextAction::Clear:

						it     = a_data.erase(it);
						result = true;

						break;

					case UIBipedObjectListContextAction::Swap:

						if (IterSwap(a_data, it, hr.dir))
						{
							result = true;
						}

						break;
					}

					if (it != a_data.end())
					{
						ImGui::TableSetColumnIndex(1);

						ImGui::Text("%s", UIBipedObjectSelectorWidget::GetBipedSlotDesc(*it));

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
