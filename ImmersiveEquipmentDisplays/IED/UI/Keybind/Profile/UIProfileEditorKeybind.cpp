#include "pch.h"

#include "UIProfileEditorKeybind.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIProfileEditorKeybind::UIProfileEditorKeybind(
			Controller& a_controller) :
			UIProfileEditorBase<KeybindProfile>(
				UIProfileStrings::TitleKeybind,
				"ied_pe_kbnd"),
			m_controller(a_controller)
		{
		}

		UIProfileEditorKeybind::~UIProfileEditorKeybind() noexcept
		{
			GlobalProfileManager::GetSingleton<KeybindProfile>().RemoveSink(this);
		}

		void UIProfileEditorKeybind::Initialize()
		{
			InitializeProfileBase();
		}

		void UIProfileEditorKeybind::DrawProfileEditorMenuBarItems()
		{
			if (ImGui::BeginMenu(UIL::LS(CommonStrings::Actions, "peb_1")))
			{
				if (m_state.selected)
				{
					auto& data = GetProfileManager().Data();

					auto it = data.find(*m_state.selected);
					if (it != data.end())
					{
						DrawKeyBindActionItems(it->second.Data());
					}
				}

				ImGui::EndMenu();
			}
		}

		void UIProfileEditorKeybind::DrawItem(KeybindProfile& a_profile)
		{
			if (m_state.selected)
			{
				auto& data = GetProfileManager().Data();

				if (auto it = data.find(*m_state.selected); it != data.end())
				{
					DrawKeyBindEditorWidget(it->second.Data());
				}
			}
		}

		ProfileManager<KeybindProfile>& UIProfileEditorKeybind::GetProfileManager() const
		{
			return GlobalProfileManager::GetSingleton<KeybindProfile>();
		}

		UIPopupQueue& UIProfileEditorKeybind::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		WindowLayoutData UIProfileEditorKeybind::GetWindowDimensions() const
		{
			return { 0.0f, 600.0f, 400.0f, true };
		}
	}
}