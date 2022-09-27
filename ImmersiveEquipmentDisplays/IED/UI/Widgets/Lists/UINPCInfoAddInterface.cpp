#include "pch.h"

#include "../../UIFormBrowser.h"
#include "UINPCInfoAddInterface.h"

#include "IED/UI/PopupQueue/UIPopupQueue.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UINPCInfoAddInterface::UINPCInfoAddInterface(
			Controller& a_controller) :
			m_controller(a_controller)
		{
		}

		void UINPCInfoAddInterface::DrawNPCInfoAdd(Game::FormID a_current)
		{
			ImGui::PushID("npc_info_add");

			auto& formBrowser = m_controller.UIGetFormBrowser();

			if (ImGui::Button(LS(CommonStrings::Find, "ctl")))
			{
				if (formBrowser.Open(false))
				{
					formBrowser.SetTabFilter({ TESNPC::kTypeID });
					formBrowser.SetHighlightForm(a_current);
				}
			}

			if (formBrowser.DrawImpl())
			{
				auto& e = formBrowser.GetSelectedEntry();

				m_controller.QueueUpdateNPCInfo(
					e->formid,
					[this, form = e->formid](bool a_result) {
						if (a_result)
						{
							OnNPCInfoAdded(form);
						}
						else
						{
							auto& queue = m_controller.UIGetPopupQueue();
							queue.push(
								UIPopupType::Message,
								LS(CommonStrings::Error),
								"%s [%.8X]",
								LS(UIWidgetCommonStrings::NPCNotFound),
								form.get());
						}
					});
			}

			ImGui::PopID();
		}

		void UINPCInfoAddInterface::QueueUpdateNPCInfo(Game::FormID a_handle)
		{
			m_controller.QueueUpdateNPCInfo(a_handle);
		}
	}
}