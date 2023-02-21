#include "pch.h"

#include "UIObjectSlotList.h"

#include "IED/UI/Widgets/UIObjectTypeSelectorWidget.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"

#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"

#include "IED/Data.h"

namespace IED
{
	namespace UI
	{
		bool UIObjectSlotList::DrawObjectSlotListTree(
			const char*                   a_strid,
			Data::configObjectSlotList_t& a_data,
			std::function<bool()>         a_extra)
		{
			ImGui::PushID(a_strid);

			bool result = false;

			const auto hr = DrawHeaderContextMenu(a_data);

			const bool treeDisabled = a_data.empty();

			switch (hr)
			{
			case UIObjectSlotListContextAction::Add:
			case UIObjectSlotListContextAction::Paste:
				if (!treeDisabled)
				{
					ImGui::SetNextItemOpen(true);
				}
				[[fallthrough]];
			case UIObjectSlotListContextAction::Clear:
				result = true;
				break;
			}

			UICommon::PushDisabled(treeDisabled);

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(UIWidgetCommonStrings::EquipmentSlots)))
			{
				if (!treeDisabled)
				{
					ImGui::Spacing();

					ImGui::PushID("extra");
					result |= a_extra();
					ImGui::PopID();

					result |= DrawTable(a_data);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(treeDisabled);

			ImGui::PopID();

			return result;
		}

		UIObjectSlotListContextAction UIObjectSlotList::DrawHeaderContextMenu(
			Data::configObjectSlotList_t& a_data)
		{
			UIObjectSlotListContextAction result{
				UIObjectSlotListContextAction::None
			};

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_newObject = Data::ObjectSlot::kMax;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Add, "1"))
				{
					if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector("##0", m_newObject))
					{
						if (m_newObject < Data::ObjectSlot::kMax)
						{
							auto it = std::find(a_data.begin(), a_data.end(), m_newObject);
							if (it == a_data.end())
							{
								a_data.emplace_back(m_newObject);

								result = UIObjectSlotListContextAction::Add;
							}
						}

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_MI(UIWidgetCommonStrings::ClearAll, "2"))
				{
					a_data.clear();
					result = UIObjectSlotListContextAction::Clear;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "3")))
				{
					UIClipboard::Set<Data::configObjectSlotList_t>(a_data);
				}

				const auto clipData = UIClipboard::Get<Data::configObjectSlotList_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::PasteOver, "4"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;
						result = UIObjectSlotListContextAction::Paste;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		UIObjectSlotListContextResult UIObjectSlotList::DrawEntryContextMenu()
		{
			UIObjectSlotListContextResult result;

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_newObject = Data::ObjectSlot::kMax;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = UIObjectSlotListContextAction::Swap;
				result.dir    = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = UIObjectSlotListContextAction::Swap;
				result.dir    = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Insert, "1"))
				{
					if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector("##0", m_newObject))
					{
						result.object = m_newObject;
						result.action = UIObjectSlotListContextAction::Add;

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_MI(CommonStrings::Delete, "2"))
				{
					result.action = UIObjectSlotListContextAction::Clear;
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		bool UIObjectSlotList::DrawTable(
			Data::configObjectSlotList_t& a_data)
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
				const auto w =
					(ImGui::GetFontSize() + ImGui::GetStyle().ItemInnerSpacing.x) * 3.0f + 2.0f;

				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, w);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Slot), ImGuiTableColumnFlags_None, 250.0f);

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
					case UIObjectSlotListContextAction::Add:

						if (hr.object < Data::ObjectSlot::kMax)
						{
							if (std::find(a_data.begin(), a_data.end(), hr.object) == a_data.end())
							{
								it     = a_data.emplace(it, hr.object);
								result = true;
							}
						}

						break;
					case UIObjectSlotListContextAction::Clear:

						it     = a_data.erase(it);
						result = true;

						break;

					case UIObjectSlotListContextAction::Swap:

						if (IterSwap(a_data, it, hr.dir))
						{
							result = true;
						}

						break;
					}

					if (it != a_data.end())
					{
						if (const auto slotDesc = Data::GetSlotName(*it))
						{
							ImGui::TableSetColumnIndex(1);
							ImGui::Text(slotDesc);
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
