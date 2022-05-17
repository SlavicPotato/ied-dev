#include "pch.h"

#include "IFirstPersonState.h"

#include "IED/Data.h"

namespace IED
{
	void IFirstPersonState::InitializeFPStateData()
	{
		assert(Data::IData::GetPluginInfo().IsPopulated());

		m_ifpvState = Data::IData::GetPluginInfo().LookupForm<TESGlobal>("IFPVDetector.esl", 0x801);
	}

	inline static bool is_in_first_person_state() noexcept
	{
		if (auto playerCamera = PlayerCamera::GetSingleton())
		{
			return playerCamera->cameraState ==
			       playerCamera->GetCameraState(PlayerCamera::CameraState::kFirstPerson);
		}
		else
		{
			return false;
		}
	}

	bool IFirstPersonState::IsInFirstPerson() const noexcept
	{
		if (m_ifpvState)
		{
			return is_in_first_person_state() || m_ifpvState->value != 0.f;
		}
		else
		{
			return is_in_first_person_state();
		}
	}
}