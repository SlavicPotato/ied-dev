#include "pch.h"

#include "ProcessParams.h"

namespace IED
{
	void processParams_t::ResetEffectShaders()
	{
		if (!state.flags.test(ProcessStateUpdateFlags::kEffectShadersReset))
		{
			if (auto pl = Game::ProcessLists::GetSingleton())
			{
				pl->ResetEffectShaders(handle);

				state.flags.set(ProcessStateUpdateFlags::kEffectShadersReset);
			}
		}
	}

}
