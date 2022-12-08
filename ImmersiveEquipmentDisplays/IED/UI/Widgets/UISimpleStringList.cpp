#include "pch.h"

#include "UISimpleStringList.h"

#include "IED/UI/UIClipboard.h"

#include "UIPopupToggleButtonWidget.h"
#include "UIWidgetsCommon.h"

namespace IED
{
	namespace UI
	{
		bool UISimpleStringListWidget::DrawStringListTree(
			const char*                    a_id,
			Localization::StringID         a_title,
			Data::configFixedStringList_t& a_data,
			ImGuiTreeNodeFlags             a_treeFlags)
		{
			ImGui::PushID(a_id);

			bool result = false;

			const auto ctxresult = DrawContextMenu(a_data);

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			switch (ctxresult)
			{
			case SimpleStringListContextResult::Add:
			case SimpleStringListContextResult::Paste:
				if (!a_data.empty())
				{
					ImGui::SetNextItemOpen(true);
				}
				[[fallthrough]];
			case SimpleStringListContextResult::Clear:
				result = true;
				break;
			}

			bool disabled = a_data.empty();

			UICommon::PushDisabled(disabled);

			if (ImGui::TreeNodeEx(
					"tree",
						a_treeFlags,
					"%s",
					UIL::LS(a_title)))
			{
				if (!a_data.empty())
				{
					ImGui::Spacing();

					result |= DrawStringListTree(a_data);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(disabled);

			ImGui::PopID();

			return result;
		}

		SimpleStringListContextResult UISimpleStringListWidget::DrawContextMenu(
			Data::configFixedStringList_t& a_data)
		{
			SimpleStringListContextResult result{ SimpleStringListContextResult::None };

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Add, "1"))
				{
					if (DrawDescriptionPopup())
					{
						if (auto& data = GetDescriptionPopupBuffer(); !data.empty())
						{
							a_data.emplace_back(data);
							result = SimpleStringListContextResult::Add;
						}
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Clear, "2"),
						nullptr,
						false,
						!a_data.empty()))
				{
					a_data.clear();

					result = SimpleStringListContextResult::Clear;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "3")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configFixedStringList_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Paste, "4"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;

						result = SimpleStringListContextResult::Paste;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		bool UISimpleStringListWidget::DrawStringListTree(
			Data::configFixedStringList_t& a_data)
		{
			bool result = false;

			ImGui::PushID("list");
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 1.0f });

			std::uint32_t i = 0;

			auto it = a_data.begin();

			while (it != a_data.end())
			{
				ImGui::PushID(i);

				if (ImGui::Button("X##ctl_del"))
				{
					it     = a_data.erase(it);
					result = true;
				}

				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

				if (ImGui::ArrowButton("ctl_up", ImGuiDir_Up))
				{
					if (IterSwap(a_data, it, SwapDirection::Up))
					{
						result = true;
					}
				}

				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

				if (ImGui::ArrowButton("ctl_dn", ImGuiDir_Down))
				{
					if (IterSwap(a_data, it, SwapDirection::Down))
					{
						result = true;
					}
				}

				if (it != a_data.end())
				{
					ImGui::SameLine();
					ImGui::TextUnformatted(it->c_str());

					++it;
					i++;
				}

				ImGui::PopID();
			}

			ImGui::PopStyleVar();
			ImGui::PopID();

			return result;
		}
	}
}