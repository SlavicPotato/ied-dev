#pragma once

#include "EffectShaderData.h"
#include "ObjectManagerData.h"

namespace IED
{
	class EffectController
	{
	public:
		inline EffectController(bool a_enabled) :
			m_enabled(a_enabled)
		{
		}

		void ProcessEffects(const ActorObjectMap& a_map);

		[[nodiscard]] inline constexpr auto EffectControllerGetTime() const noexcept
		{
			return m_currentTime;
		}

		[[nodiscard]] inline constexpr bool EffectControllerEnabled() const noexcept
		{
			return m_enabled;
		}

		inline constexpr void SetEffectControllerParallelUpdates(bool a_switch)
		{
			m_parallel = a_switch;
		}

	protected:
		static void UpdateActor(
			const Game::Unk2f6b948::Steps& a_steps,
			const ActorObjectHolder&       a_holder);

		SKMP_FORCEINLINE static void UpdateEffects(
			EffectShaderData& a_data,
			float             a_step);

	private:
		mutable PerfTimerInt m_timer{ 1000000LL };
		mutable long long    m_currentTime{ 0LL };

		bool m_enabled{ false };
		bool m_parallel{ false };
	};

}