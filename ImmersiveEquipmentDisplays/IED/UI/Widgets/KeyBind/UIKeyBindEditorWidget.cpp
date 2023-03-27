#include "pch.h"

#include "UIKeyBindEditorWidget.h"

#include "IED/UI/Widgets/UIControlKeySelectorWidget.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"
#include "IED/UI/Widgets/UIWidgetCommonStrings.h"

#include "IED/UI/UILocalizationInterface.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		bool UIKeyBindEditorWidget::DrawKeyBindEditorWidget(
			KB::KeyToggleStateEntryHolder& a_data)
		{
			ImGui::PushID("kb_editor");

			bool result = DrawList(a_data);

			ImGui::PopID();

			return result;
		}

		bool UIKeyBindEditorWidget::DrawList(
			KB::KeyToggleStateEntryHolder& a_data)
		{
			bool result = false;

			ImGui::PushID("list");

			for (auto it = a_data.entries.begin(); it != a_data.entries.end();)
			{
				ImGui::PushID(it->first.c_str());

				const auto contextResult = DrawEntryTreeContextMenu(*it);

				switch (contextResult)
				{
				case UIKeyBindEditorContextAction::Delete:

					it     = a_data.entries.erase(it);
					result = true;

					break;
				}

				if (it != a_data.entries.end())
				{
					result |= DrawEntryTree(*it);

					++it;
				}

				ImGui::PopID();
			}

			ImGui::PopID();

			return result;
		}

		auto UIKeyBindEditorWidget::DrawEntryTreeContextMenu(
			KB::KeyToggleStateEntryHolder::container_type::value_type& a_data)
			-> UIKeyBindEditorContextAction
		{
			UIKeyBindEditorContextAction result{ UIKeyBindEditorContextAction ::None };

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 2.0f });

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Delete, "1")))
				{
					result = UIKeyBindEditorContextAction::Delete;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(
						UIL::LS(UIWidgetCommonStrings::CopyIDToClipboard, "2")))
				{
					ImGui::SetClipboardText(a_data.first.c_str());
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		bool UIKeyBindEditorWidget::DrawEntryTree(
			KB::KeyToggleStateEntryHolder::container_type::value_type& a_data)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
			{
				return false;
			}

			bool result = false;

			ImGui::PushID("tree");

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s:[%s] %s:[%hhu]",
					UIL::LS(CommonStrings::ID),
					a_data.first.c_str(),
					UIL::LS(CommonStrings::State),
					a_data.second.GetState()))
			{
				ImGui::Spacing();
				ImGui::Indent();

				result |= DrawEntry(a_data);

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::TreePop();
			}

			ImGui::PopID();

			return result;
		}

		bool UIKeyBindEditorWidget::DrawEntry(
			KB::KeyToggleStateEntryHolder::container_type::value_type& a_data)
		{
			bool result = false;

			ImGui::PushID("entry");

			result |= UIControlKeySelectorWidget::DrawKeySelector(
				"1",
				static_cast<Localization::StringID>(CommonStrings::ComboKey),
				UIData::g_comboControlMap,
				a_data.second.comboKey,
				true);

			result |= UIControlKeySelectorWidget::DrawKeySelector(
				"2",
				static_cast<Localization::StringID>(CommonStrings::Key),
				UIData::g_controlMap,
				a_data.second.key,
				true);

			constexpr std::uint32_t step     = 1;
			constexpr std::uint32_t stepFast = 3;

			if (ImGui::InputScalar(
				UIL::LS(CommonStrings::States, "3"),
				ImGuiDataType_U32,
				std::addressof(a_data.second.numStates),
				std::addressof(step),
				std::addressof(stepFast),
				"%u",
				ImGuiInputTextFlags_None))
			{
				a_data.second.numStates = std::max(a_data.second.numStates, 1u);

				result = true;
			}

			ImGui::PopID();

			return result;
		}
	}
}
