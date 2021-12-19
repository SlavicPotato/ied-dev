#include "pch.h"

#include "UINotificationInterface.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		UINotificationInterface::UINotificationInterface(
			Controller& a_controller) :
			m_controller(a_controller)
		{
		}

		UIPopupQueue& UINotificationInterface::GetPopupQueue() const noexcept
		{
			return m_controller.UIGetPopupQueue();
		}
	}
}