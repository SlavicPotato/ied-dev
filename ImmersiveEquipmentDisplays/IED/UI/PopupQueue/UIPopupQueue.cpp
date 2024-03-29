#include "pch.h"

#include "UIPopupQueue.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		void UIPopupQueue::run()
		{
			if (m_queue.empty())
			{
				return;
			}

			auto& e = m_queue.front();

			ImGui::PushID("popup_queue");

			if (!ImGui::IsPopupOpen(e.m_key.c_str()))
			{
				ImGui::OpenPopup(e.m_key.c_str());
			}

			ModalStatus res{ ModalStatus::kNoAction };

			switch (e.m_type)
			{
			case UIPopupType::Confirm:
				res = ConfirmDialog(
					e.m_key.c_str(),
					e.m_funcDraw,
					e.m_textWrapSize,
					e.m_buf.get());
				break;
			case UIPopupType::Input:
				res = TextInputDialog(
					e.m_key.c_str(),
					e.m_buf.get(),
					ImGuiInputTextFlags_EnterReturnsTrue,
					e.m_funcDraw,
					e.m_textWrapSize,
					e.a_allowEmpty,
					e.m_input);
				break;
			case UIPopupType::MultilineInput:
				res = TextInputMultilineDialog(
					e.m_key.c_str(),
					e.m_buf.get(),
					ImGuiInputTextFlags_EnterReturnsTrue,
					e.m_funcDraw,
					e.m_textWrapSize,
					e.a_allowEmpty,
					e.m_input);
				break;
			case UIPopupType::Message:
				res = MessageDialog(
					e.m_key.c_str(),
					e.m_funcDraw,
					e.m_textWrapSize,
					e.m_buf.get());
				break;
			case UIPopupType::Custom:
				res = CustomDialog(
					e.m_key.c_str(),
					e.m_funcDraw);
				break;
			}

			if (res == ModalStatus::kAccept)
			{
				if (e.m_func)
				{
					e.m_func(e);
				}

				m_queue.pop();
			}
			else if (res == ModalStatus::kReject)
			{
				m_queue.pop();
			}

			ImGui::PopID();
		}
	}
}