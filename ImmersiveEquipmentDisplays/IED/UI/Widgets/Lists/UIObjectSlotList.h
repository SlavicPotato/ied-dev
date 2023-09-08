#pragma once

#include "IED/UI/UILocalizationInterface.h"

#include "IED/ConfigCommon.h"

#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/UI/Widgets/UIObjectTypeSelectorWidget.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"

#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"

#include "IED/Data.h"

namespace IED
{
	namespace UI
	{
		enum class UIObjectSlotListContextAction
		{
			None,
			Add,
			Clear,
			Paste,
			Swap
		};

		template <class T>
		struct UIObjectSlotListContextResult
		{
			UIObjectSlotListContextAction action{ UIObjectSlotListContextAction::None };
			T                             object{ T::kMax };
			SwapDirection                 dir;
		};

		template <Data::concepts::is_valid_slot_id T>
		class UIObjectSlotList
		{
		public:
			UIObjectSlotList() = default;

			bool DrawObjectSlotListTree(
				const char*           a_strid,
				stl::boost_vector<T>& a_data,
				std::function<bool()> a_extra = [] { return false; });

		private:
			UIObjectSlotListContextAction    DrawHeaderContextMenu(stl::boost_vector<T>& a_data);
			UIObjectSlotListContextResult<T> DrawEntryContextMenu();

			bool DrawTable(stl::boost_vector<T>& a_data);

			T m_newObject{ T::kMax };
		};

		template <Data::concepts::is_valid_slot_id T>
		bool UIObjectSlotList<T>::DrawObjectSlotListTree(
			const char*           a_strid,
			stl::boost_vector<T>& a_data,
			std::function<bool()> a_extra)
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

			constexpr auto treeLabelSID =
				stl::is_any_same_v<T, Data::ObjectType, Data::ObjectTypeExtra> ?
					UIWidgetCommonStrings::EquipmentTypes :
					UIWidgetCommonStrings::EquipmentSlots;

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(treeLabelSID)))
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

		template <Data::concepts::is_valid_slot_id T>
		UIObjectSlotListContextAction UIObjectSlotList<T>::DrawHeaderContextMenu(
			stl::boost_vector<T>& a_data)
		{
			UIObjectSlotListContextAction result{
				UIObjectSlotListContextAction::None
			};

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_newObject = T::kMax;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Add, "1"))
				{
					if (UIObjectSlotSelectorWidget::DrawObjectSlotSelector("##0", m_newObject))
					{
						if (m_newObject < T::kMax)
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

				using data_type = stl::strip_type<decltype(a_data)>;

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "3")))
				{
					UIClipboard::Set(a_data);
				}

				const auto clipData = UIClipboard::Get<data_type>();

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

		template <Data::concepts::is_valid_slot_id T>
		UIObjectSlotListContextResult<T> UIObjectSlotList<T>::DrawEntryContextMenu()
		{
			UIObjectSlotListContextResult<T> result;

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_newObject = T::kMax;
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

		template <Data::concepts::is_valid_slot_id T>
		bool UIObjectSlotList<T>::DrawTable(stl::boost_vector<T>& a_data)
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

						if (hr.object < T::kMax)
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
						if (const auto slotDesc = Data::GetObjectName(*it))
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