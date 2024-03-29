#include "pch.h"

#include "ProcessParams.h"

namespace IED
{
	void ProcessParams::SuspendReferenceEffectShaders() noexcept
	{
		if (!state.flags.test(ProcessStateUpdateFlags::kEffectShadersSuspended))
		{
			if (auto pl = Game::ProcessLists::GetSingleton())
			{
				pl->SuspendReferenceEffectShaders(handle);

				state.flags.set(ProcessStateUpdateFlags::kEffectShadersSuspended);
			}
		}
	}

}
