#include "pch.h"

#include "ProcessParams.h"

namespace IED
{
	void processState_t::UpdateEffectShaders(Game::ObjectRefHandle a_handle)
	{
		if (!flags.test(ProcessStateUpdateFlags::kEffectShadersReset))
		{
			if (auto pl = Game::ProcessLists::GetSingleton())
			{
				pl->ResetEffectShaders(a_handle);
			}

			flags.set(ProcessStateUpdateFlags::kEffectShadersReset);
		}
	}
}
