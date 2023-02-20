#include "pch.h"

#include "UIPopupToggleButtonWidget.h"

#include "UIControlKeySelectorWidget.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		bool UIControlKeySelectorWidget::DrawKeySelector(
			const char*                      a_strid,
			Localization::StringID           a_label,
			const UIData::controlDescData_t& a_data,
			std::uint32_t&                   a_key,
			bool                             a_allowNone)
		{
			char        buf[16];
			const char* preview;

			bool result = false;

			ImGui::PushID(a_strid);

			const auto contextResult = DrawContextMenu(a_key, a_allowNone);

			if (contextResult == UIControlKeySelectorContextAction::ManualChange)
			{
				result = true;
			}

			auto it = a_data.find(a_key);
			if (it != a_data.end())
			{
				preview = it->second;
			}
			else
			{
				stl::snprintf(buf, "0x%X", a_key);
				preview = buf;
			}

			if (ImGui::BeginCombo(
					UIL::LS(a_label, "1"),
					preview,
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : a_data.getvec())
				{
					if (!a_allowNone && !e->first)
					{
						continue;
					}

					ImGui::PushID(e);

					bool selected = e->first == a_key;
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
						{
							ImGui::SetScrollHereY();
						}
					}

					if (ImGui::Selectable(e->second, selected))
					{
						a_key  = e->first;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			ImGui::PopID();

			return result;
		}

		auto UIControlKeySelectorWidget::DrawContextMenu(
			std::uint32_t& a_key,
			bool           a_allowNone)
			-> UIControlKeySelectorContextAction
		{
			UIControlKeySelectorContextAction result{ UIControlKeySelectorContextAction::None };

			ImGui::PushID("context_area");

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::BeginMenu(
						UIL::LS(CommonStrings::Set, "1")))
				{
					char buffer[16];
					
					stl::snprintf(buffer, "0x%X", a_key);

					ImGui::AlignTextToFramePadding();
					ImGui::Text("0x");
					ImGui::SameLine();

					static_assert(sizeof(buffer) >= 11);

					if (ImGui::InputText(
							UIL::LS(CommonStrings::Scancode, "1"),
							buffer + 2,
							9,
							ImGuiInputTextFlags_CharsHexadecimal |
								ImGuiInputTextFlags_CharsUppercase |
								ImGuiInputTextFlags_EnterReturnsTrue))
					{
						if (const auto v = stl::ston<unsigned long>(buffer, 0))
						{
							if (*v || a_allowNone)
							{
								a_key = *v;
								ImGui::CloseCurrentPopup();

								result = UIControlKeySelectorContextAction::ManualChange;
							}
						}
					}

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}
	}
}
