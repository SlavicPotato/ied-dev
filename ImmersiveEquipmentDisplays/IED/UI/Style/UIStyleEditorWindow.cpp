#include "pch.h"

#include "UIStyleEditorWindow.h"

#include "IED/UI/UIClipboard.h"

#include "Drivers/UI.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIStyleEditorWindow::UIStyleEditorWindow(Controller& a_controller) :
			UIProfileEditorBase<StyleProfile>(
				UIProfileStrings::TitleStyle,
				"ied_pe_ste",
				UIProfileEditorBaseFlags::kDisableSaveUnmodified),
			m_controller(a_controller)
		{
		}

		UIStyleEditorWindow::~UIStyleEditorWindow() noexcept
		{
			Drivers::UI::GetStyleProfileManager().RemoveSink(this);
		}

		void UIStyleEditorWindow::Initialize()
		{
			InitializeProfileBase();
		}

		void UIStyleEditorWindow::DrawProfileEditorMenuBarItems()
		{
			if (ImGui::BeginMenu(UIL::LS(CommonStrings::Actions, "peb_1")))
			{
				if (m_state.selected)
				{
					auto& data = GetProfileManager().Data();

					auto it = data.find(*m_state.selected);
					if (it != data.end())
					{
						DrawActionMenuItems(it->first, it->second);
					}
				}

				ImGui::EndMenu();
			}
		}

		void UIStyleEditorWindow::OnOpen()
		{
			SelectCurrentStyle();
		}

		void UIStyleEditorWindow::DrawItem(StyleProfile& a_profile)
		{
			if (ImGui::BeginChild("editor_child"))
			{
				if (DrawStyleEditor(a_profile.Data()))
				{
					a_profile.MarkModified();
					Drivers::UI::SetStyle(a_profile.Name(), true);
				}
			}

			ImGui::EndChild();
		}

		ProfileManager<StyleProfile>& UIStyleEditorWindow::GetProfileManager() const
		{
			return Drivers::UI::GetStyleProfileManager();
		}

		UIPopupQueue& UIStyleEditorWindow::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		void UIStyleEditorWindow::OnItemSelected(
			const stl::fixed_string& a_item)
		{
			Drivers::UI::SetStyle(a_item, true);

			auto& settings = m_controller.GetSettings();
			settings.set(settings.data.ui.stylePreset, a_item);
		}

		void UIStyleEditorWindow::OnProfileRename(
			const stl::fixed_string& a_oldName,
			const stl::fixed_string& a_newName)
		{
			if (Drivers::UI::GetCurrentStyle() == a_oldName)
			{
				Drivers::UI::SetStyle(a_newName);
			}
		}

		void UIStyleEditorWindow::OnProfileDelete(
			const stl::fixed_string& a_name)
		{
			if (Drivers::UI::GetCurrentStyle() == a_name)
			{
				Drivers::UI::SetStyle(stl::fixed_string(), true);
			}
		}

		void UIStyleEditorWindow::OnProfileSave(
			const stl::fixed_string& a_name,
			StyleProfile&            a_profile)
		{
		}

		void UIStyleEditorWindow::OnProfileReload(
			const StyleProfile& a_profile)
		{
			if (Drivers::UI::GetCurrentStyle() == a_profile.Name())
			{
				Drivers::UI::SetStyle(a_profile.Name(), true);
			}
		}

		WindowLayoutData UIStyleEditorWindow::GetWindowDimensions() const
		{
			return { 0.0, 600.0f, 800.0f, true };
		}

		void UIStyleEditorWindow::SelectCurrentStyle()
		{
			SetSelected(Drivers::UI::GetCurrentStyle());
		}

		void UIStyleEditorWindow::DrawActionMenuItems(
			const stl::fixed_string& a_name,
			StyleProfile&            a_data)
		{
			if (UIL::LCG_MI(CommonStrings::Copy, "1"))
			{
				UIClipboard::Set(a_data.Data());
			}

			auto clipData = UIClipboard::Get<Data::ImGuiStyleHolder>();

			if (ImGui::MenuItem(
					UIL::LS(CommonStrings::PasteOver, "2"),
					nullptr,
					false,
					clipData != nullptr))
			{
				if (clipData)
				{
					a_data.Data() = *clipData;
					a_data.MarkModified();

					Drivers::UI::SetStyle(a_name, true);
				}
			}
		}
	}
}