#include "pch.h"

#include "UIDescriptionPopup.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		bool UIDescriptionPopupWidget::DrawDescriptionPopup(
			const char* a_label)
		{
			bool ret = ImGui::InputText(
				a_label ?
					a_label :
                    "##desc_input",
				std::addressof(m_buffer),
				ImGuiInputTextFlags_EnterReturnsTrue);

			bool result = ret && !m_buffer.empty();

			if (result)
			{
				ImGui::CloseCurrentPopup();
			}

			return result;
		}

		void UIDescriptionPopupWidget::ClearDescriptionPopupBuffer()
		{
			m_buffer.clear();
		}

		void UIDescriptionPopupWidget::SetDescriptionPopupBuffer(
			const std::string& a_text)
		{
			m_buffer = a_text;
		}

	}
}