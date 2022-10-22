#pragma once

#include "EffectShaderData.h"
#include "ObjectEntryBase.h"
#include "ObjectEntrySlot.h"
#include "ObjectManagerData.h"

namespace IED
{
	class ActorObjectHolder;

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

		SKMP_FORCEINLINE static void UpdateEffectsOnDisplay(
			const EffectShaderData&       a_data,
			const ObjectEntryBase::State& a_state,
			float                         a_step);

		SKMP_FORCEINLINE static void UpdateEffectsOnEquipped(
			Actor*                  a_actor,
			const EffectShaderData& a_data,
			float                   a_step);

		static void ProcessNiObjectTree(
			NiAVObject*                    a_object,
			const EffectShaderData::Entry& a_entry);

		SKMP_FORCEINLINE static void UpdateObjectEffects(
			Actor*                   a_actor,
			const ObjectEntryCustom& a_entry,
			float                    a_step);

		SKMP_FORCEINLINE static void UpdateObjectEffects(
			Actor*                 a_actor,
			const ObjectEntrySlot& a_entry,
			float                  a_step);

	private:
		mutable PerfTimerInt m_timer{ 1000000LL };
		mutable long long    m_currentTime{ 0LL };

		bool m_enabled{ false };
		bool m_parallel{ false };
	};

}