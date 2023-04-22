#include "pch.h"

#include "UIKeyBindEditorWidget.h"

#include "IED/UI/Widgets/UIControlKeySelectorWidget.h"
#include "IED/UI/Widgets/UIKeyBindIDSelectorWidget.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"
#include "IED/UI/Widgets/UIWidgetCommonStrings.h"

#include "IED/UI/UILocalizationInterface.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		void UIKeyBindEditorWidget::DrawKeyBindEditorWidget(
			Data::configKeybindEntryHolder_t& a_data)
		{
			ImGui::PushID("kb_editor");

			DrawList(a_data);

			ImGui::PopID();
		}

		void UIKeyBindEditorWidget::DrawKeyBindActionItems(
			Data::configKeybindEntryHolder_t& a_data)
		{
			if (ImGui::BeginMenu(UIL::LS(CommonStrings::Add, "kbe_1")))
			{
				const bool r = UIKeyBindIDSelectorWidget::DrawKeyBindIDSelector(m_tmpID);

				if (r && !m_tmpID.empty())
				{
					const auto s = a_data.data.try_emplace(std::move(m_tmpID));

					if (s.second)
					{
						OnKeybindAdd(*s.first);
					}
					else
					{
						m_tmpID.clear();
					}

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndMenu();
			}
		}

		void UIKeyBindEditorWidget::DrawList(
			Data::configKeybindEntryHolder_t& a_data)
		{
			ImGui::PushID("list");

			for (auto it = a_data.data.begin(); it != a_data.data.end();)
			{
				ImGui::PushID(it->first.c_str());

				const auto contextResult = DrawEntryTreeContextMenu(*it);

				switch (contextResult)
				{
				case UIKeyBindEditorContextAction::Delete:

					OnKeybindErase(it->first);

					it = a_data.data.erase(it);

					break;
				}

				if (it != a_data.data.end())
				{
					DrawEntryTree(*it);

					++it;
				}

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		auto UIKeyBindEditorWidget::DrawEntryTreeContextMenu(
			Data::configKeybindEntryHolder_t::container_type::value_type& a_data)
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

		void UIKeyBindEditorWidget::DrawEntryTree(
			Data::configKeybindEntryHolder_t::container_type::value_type& a_data)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
			{
				return;
			}

			std::uint32_t state;

			const bool hasState = GetKeybindState(a_data.first, state);

			ImGui::PushID("tree");

			bool result;

			constexpr auto flags = ImGuiTreeNodeFlags_DefaultOpen |
			                       ImGuiTreeNodeFlags_SpanAvailWidth;

			if (hasState)
			{
				result = ImGui::TreeNodeEx(
					"tree",
					flags,
					"%s:[%s] %s:[%u]",
					UIL::LS(CommonStrings::ID),
					a_data.first.c_str(),
					UIL::LS(CommonStrings::State),
					state);
			}
			else
			{
				result = ImGui::TreeNodeEx(
					"tree",
					flags,
					"%s:[%s]",
					UIL::LS(CommonStrings::ID),
					a_data.first.c_str());
			}

			if (result)
			{
				ImGui::Spacing();

				DrawEntry(a_data);

				ImGui::Spacing();

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		void UIKeyBindEditorWidget::DrawEntry(
			Data::configKeybindEntryHolder_t::container_type::value_type& a_data)
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

			if (result)
			{
				OnKeybindChange(a_data);
			}
		}

		void UIKeyBindEditorWidget::OnKeybindErase(
			const Data::configKeybindEntryHolder_t::container_type::key_type& a_key)
		{
		}

		void UIKeyBindEditorWidget::OnKeybindAdd(
			const Data::configKeybindEntryHolder_t::container_type::value_type& a_key)
		{
		}

		void UIKeyBindEditorWidget::OnKeybindChange(
			const Data::configKeybindEntryHolder_t::container_type::value_type& a_data)
		{
		}

		bool UIKeyBindEditorWidget::GetKeybindState(
			const Data::configKeybindEntryHolder_t::container_type::key_type& a_key,
			std::uint32_t&                                                    a_stateOut)
		{
			return false;
		}
	}
}
