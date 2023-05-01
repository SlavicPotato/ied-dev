#include "pch.h"

#include "UIKeyBindEditorWindow.h"

#include "IED/UI/Widgets/UIKeyBindIDSelectorWidget.h"
#include "IED/UI/Widgets/UIWidgetCommonStrings.h"

#include "IED/UI/UIMainStrings.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		UIKeyBindEditorWindow::UIKeyBindEditorWindow(
			Controller& a_controller) :
			UIProfileSelectorWidget<
				KeyBindEditorPSParams,
				KeybindProfile>(
				UIProfileSelectorFlags::kEnableApply |
				UIProfileSelectorFlags::kEnableMerge),
			m_controller(a_controller)
		{
		}

		UIKeyBindEditorWindow::~UIKeyBindEditorWindow()
		{
			GlobalProfileManager::GetSingleton<KeybindProfile>().RemoveSink(this);
		}

		void UIKeyBindEditorWindow::Initialize()
		{
			InitializeProfileBase();
		}

		void UIKeyBindEditorWindow::Draw()
		{
			SetWindowDimensions(0.0f, 800.0f, 600.0f, true);

			if (ImGui::Begin(
					UIL::LS<UIMainStrings, 3>(
						UIMainStrings::KeyBinds,
						WINDOW_ID),
					GetOpenState(),
					ImGuiWindowFlags_MenuBar))
			{
				DrawMenuBar();

				const bool disabled = m_controller.IsDefaultConfigForced();

				UICommon::PushDisabled(disabled);

				DrawProfileTree();
				ImGui::Separator();

				DrawEditor();

				UICommon::PopDisabled(disabled);
			}

			ImGui::End();
		}

		void UIKeyBindEditorWindow::DrawEditor()
		{
			if (ImGui::BeginChild("editor_panel", { -1.0f, 0.0f }))
			{
				ImGui::Spacing();

				ImGui::PushItemWidth(ImGui::GetFontSize() * -8.0f);

				auto& config = m_controller.GetActiveConfig();

				DrawKeyBindEditorWidget(config.keybinds);

				ImGui::PopItemWidth();
			}

			ImGui::EndChild();
		}

		void UIKeyBindEditorWindow::DrawProfileTree()
		{
			if (ImGui::TreeNodeEx(
					"tree_prof",
					ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					UIL::LS(CommonStrings::Profile)))
			{
				ImGui::Spacing();

				ImGui::PushItemWidth(ImGui::GetFontSize() * -15.5f);

				const KeyBindEditorPSParams params{
					m_controller.GetActiveConfig().keybinds
				};

				const bool disabled = m_controller.IsDefaultConfigForced();

				UICommon::PushDisabled(disabled);

				DrawProfileSelector(params);

				UICommon::PopDisabled(disabled);

				ImGui::PopItemWidth();

				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		void UIKeyBindEditorWindow::DrawMenuBar()
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(UIL::LS(CommonStrings::File, "1")))
				{
					DrawFileMenu();

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::BeginMenu(UIL::LS(CommonStrings::Actions, "2")))
				{
					DrawActionMenu();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
		}

		void UIKeyBindEditorWindow::DrawFileMenu()
		{
			if (ImGui::MenuItem(UIL::LS(CommonStrings::Exit, "1")))
			{
				SetOpenState(false);
			}
		}

		void UIKeyBindEditorWindow::DrawActionMenu()
		{
			const bool disabled = m_controller.IsDefaultConfigForced();

			UICommon::PushDisabled(disabled);

			auto& config = m_controller.GetActiveConfig();

			DrawKeyBindActionItems(config.keybinds);

			UICommon::PopDisabled(disabled);
		}

		void UIKeyBindEditorWindow::ApplyProfile(
			const KeyBindEditorPSParams& a_data,
			const KeybindProfile&        a_profile)
		{
			auto& config = m_controller.GetActiveConfig();

			config.keybinds = a_profile.Data();

			auto& holder = m_controller.GetKeyBindDataHolder();

			holder->SetFromConfig(a_profile.Data());
		}

		void UIKeyBindEditorWindow::MergeProfile(
			const KeyBindEditorPSParams& a_data,
			const KeybindProfile&        a_profile)
		{
			auto&       config = m_controller.GetActiveConfig();
			const auto& data   = a_profile.Data();

			for (auto& e : data.data)
			{
				config.keybinds.data.insert_or_assign(e.first, e.second);
			}

			config.keybinds.flags.set(data.flags);

			auto& holder = m_controller.GetKeyBindDataHolder();

			holder->MergeFromConfig(config.keybinds);
		}

		UIPopupQueue& UIKeyBindEditorWindow::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		KeybindProfile::base_type UIKeyBindEditorWindow::GetData(
			const KeyBindEditorPSParams& a_params)
		{
			return a_params.data;
		}

		void UIKeyBindEditorWindow::OnKeybindErase(
			const Data::configKeybindEntryHolder_t::container_type::key_type& a_key)
		{
			auto& holder = m_controller.GetKeyBindDataHolder();

			const stl::lock_guard lock(holder->GetLock());

			auto& data = holder->GetData().entries;

			data.erase(a_key);
		}

		void UIKeyBindEditorWindow::OnKeybindAdd(
			const Data::configKeybindEntryHolder_t::container_type::value_type& a_data)
		{
			auto& holder = m_controller.GetKeyBindDataHolder();

			const stl::lock_guard lock(holder->GetLock());

			auto& data = holder->GetData().entries;

			data.try_emplace(a_data.first, a_data.second);
		}

		void UIKeyBindEditorWindow::OnKeybindChange(
			const Data::configKeybindEntryHolder_t::container_type::value_type& a_data)
		{
			auto& holder = m_controller.GetKeyBindDataHolder();

			const stl::lock_guard lock(holder->GetLock());

			auto& data = holder->GetData().entries;

			auto r = data.try_emplace(a_data.first, a_data.second);

			if (!r.second)
			{
				r.first->second = a_data.second;
			}
		}
		bool UIKeyBindEditorWindow::GetKeybindState(
			const Data::configKeybindEntryHolder_t::container_type::key_type& a_key,
			std::uint32_t&                                                    a_stateOut)
		{
			auto& holder = m_controller.GetKeyBindDataHolder();

			return holder->GetKeyState(a_key, a_stateOut);
		}
	}
}