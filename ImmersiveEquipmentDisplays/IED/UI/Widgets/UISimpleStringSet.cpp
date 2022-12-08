#include "pch.h"

#include "UISimpleStringSet.h"

#include "IED/UI/UIClipboard.h"

#include "UIPopupToggleButtonWidget.h"

namespace IED
{
	namespace UI
	{
		bool UISimpleStringSetWidget::DrawStringSetTree(
			const char*                   a_id,
			Localization::StringID        a_title,
			Data::configFixedStringSet_t& a_data,
			ImGuiTreeNodeFlags            a_treeFlags)
		{
			ImGui::PushID(a_id);

			bool result = false;

			const auto ctxresult = DrawContextMenu(a_data);

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			switch (ctxresult)
			{
			case SimpleStringSetContextResult::Add:
			case SimpleStringSetContextResult::Paste:
				if (!a_data.empty())
				{
					ImGui::SetNextItemOpen(true);
				}
				[[fallthrough]];
			case SimpleStringSetContextResult::Clear:
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

					result |= DrawStringSetTree(a_data);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(disabled);

			ImGui::PopID();

			return result;
		}

		SimpleStringSetContextResult UISimpleStringSetWidget::DrawContextMenu(
			Data::configFixedStringSet_t& a_data)
		{
			SimpleStringSetContextResult result{ SimpleStringSetContextResult::None };

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
							if (a_data.emplace(data).second)
							{
								result = SimpleStringSetContextResult::Add;
							}
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

					result = SimpleStringSetContextResult::Clear;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "3")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configFixedStringSet_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Paste, "4"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;

						result = SimpleStringSetContextResult::Paste;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		bool UISimpleStringSetWidget::DrawStringSetTree(
			Data::configFixedStringSet_t& a_data)
		{
			bool result = false;

			ImGui::PushID("list");
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 1.0f });

			std::uint32_t i = 0;

			auto it = a_data.begin();

			while (it != a_data.end())
			{
				ImGui::PushID(i);

				if (ImGui::Button("X"))
				{
					it     = a_data.erase(it);
					result = true;
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