#include "pch.h"

#include "UIActorInfoAddInterface.h"

#include "IED/UI/PopupQueue/UIPopupQueue.h"

#include "IED/Controller/Controller.h"

#include "../../UICommon.h"

namespace IED
{
	namespace UI
	{
		UIActorInfoAddInterface::UIActorInfoAddInterface(
			Controller& a_controller) :
			m_formSelector(a_controller, FormInfoFlags::kNone, true, false),
			m_controller(a_controller)
		{
			m_formSelector.SetAllowedTypes({ Actor::kTypeID });
		}

		void UIActorInfoAddInterface::DrawActorInfoAdd()
		{
			ImGui::PushID("actor_info_add");

			if (ImGui::Button(LS(CommonStrings::Add, "ctl")))
			{
				ImGui::OpenPopup("popup");
			}

			if (ImGui::BeginPopup("popup"))
			{
				if (m_formSelector.DrawFormSelector(m_form))
				{
					m_controller.QueueUpdateActorInfo(
						m_form,
						[this, form = m_form](bool a_result) {
							if (a_result)
							{
								OnActorInfoAdded(form);
							}
							else
							{
								auto& queue = m_controller.UIGetPopupQueue();
								queue.push(
									UIPopupType::Message,
									LS(CommonStrings::Error),
									"%s [%.8X]",
									LS(UIWidgetCommonStrings::ActorNotFound),
									form.get());
							}
						});

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();
		}
	}
}