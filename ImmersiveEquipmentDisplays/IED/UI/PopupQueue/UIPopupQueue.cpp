#include "pch.h"

#include "UIPopupQueue.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		UIPopupQueue::UIPopupQueue(
			Localization::ILocalization& a_localization) :
			UICommonModals(a_localization),
			UILocalizationInterface(a_localization)
		{
		}

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
					e.m_buf);
				break;
			case UIPopupType::Input:
				res = TextInputDialog(
					e.m_key.c_str(),
					e.m_buf,
					ImGuiInputTextFlags_EnterReturnsTrue,
					e.m_funcDraw,
					e.m_input,
					sizeof(e.m_input));
				break;
			case UIPopupType::Message:
				res = MessageDialog(
					e.m_key.c_str(),
					e.m_funcDraw,
					e.m_buf);
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