#include "pch.h"

#include "UIPopupInterface.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		UIPopupInterface::UIPopupInterface(
			Controller& a_controller) :
			m_controller(a_controller)
		{
		}

		UIPopupQueue& UIPopupInterface::GetPopupQueue() const noexcept
		{
			return m_controller.UIGetPopupQueue();
		}
	}
}