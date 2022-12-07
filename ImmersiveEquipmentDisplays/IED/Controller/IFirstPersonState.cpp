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
		if (const auto* playerCamera = PlayerCamera::GetSingleton())
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
		const bool in_fp_state = is_in_first_person_state();

		if (m_ifpvState)
		{
			return in_fp_state || m_ifpvState->value != 0.f;
		}
		else
		{
			return in_fp_state;
		}
	}
}