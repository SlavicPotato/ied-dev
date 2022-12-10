#include "pch.h"

#include "IED/Controller/Controller.h"
#include "UIActorInfoInterface.h"

namespace IED
{
	namespace UI
	{
		UIActorInfoInterface::UIActorInfoInterface(
			Controller& a_controller) :
			m_controller(a_controller)
		{
		}

		const ActorInfoHolder& UIActorInfoInterface::GetActorInfo() const noexcept
		{
			return m_controller.GetActorInfo();
		}

		const NPCInfoHolder& UIActorInfoInterface::GetNPCInfo() const noexcept
		{
			return m_controller.GetNPCInfo();
		}

		std::uint64_t UIActorInfoInterface::GetActorInfoUpdateID() const noexcept
		{
			return m_controller.GetActorInfoUpdateID();
		}

		const std::optional<Game::FormID>& UIActorInfoInterface::GetCrosshairRef() const noexcept
		{
			return m_controller.GetCrosshairRef();
		}

	}
}