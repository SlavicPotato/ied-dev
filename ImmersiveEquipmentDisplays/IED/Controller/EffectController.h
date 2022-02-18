#pragma once

#include "ObjectManagerData.h"

namespace IED
{
	class EffectController
	{
	public:
		void ProcessEffects(const ActorObjectMap& a_map);

		[[nodiscard]] inline constexpr auto EffectControllerGetTime() const noexcept
		{
			return m_currentTime;
		}

	protected:
		static void UpdateEffects(const effectShaderData_t& a_data);

		mutable PerfTimerInt m_timer{ 1000000LL };
		mutable long long    m_currentTime{ 0LL };
	};

}