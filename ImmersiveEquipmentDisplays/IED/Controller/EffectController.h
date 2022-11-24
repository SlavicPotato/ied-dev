#pragma once

#include "EffectShaderData.h"
#include "ObjectEntryBase.h"
#include "ObjectEntrySlot.h"
#include "ObjectManagerData.h"

#include "IED/Physics/SimComponent.h"

namespace IED
{
	class ActorObjectHolder;

	class EffectController
	{
		struct PhysUpdateData
		{
			float timeTick;
			float timeStep;
			float maxTime;
			float timeAccum;
		};

	public:
		enum class EffectControllerFlags : std::uint8_t
		{
			kNone = 0,

			kEnableShaders = 1u << 0,
			kEnablePhysics = 1u << 1,

			kParallelProcessing = 1u << 2,

			kEnableMask =
				kEnableShaders |
				kEnablePhysics
		};

		void ProcessEffects(const ActorObjectMap& a_map);

		[[nodiscard]] inline constexpr auto EffectControllerGetTime() const noexcept
		{
			return m_currentTime;
		}

		[[nodiscard]] inline constexpr bool ShaderProcessingEnabled() const noexcept
		{
			return m_flags.test(EffectControllerFlags::kEnableShaders);
		}

		[[nodiscard]] inline constexpr bool PhysicsProcessingEnabled() const noexcept
		{
			return m_flags.test(EffectControllerFlags::kEnablePhysics);
		}

		inline constexpr void SetEffectControllerParallelUpdates(bool a_switch) noexcept
		{
			m_flags.set(EffectControllerFlags::kParallelProcessing, a_switch);
		}

		inline constexpr void SetPhysicsProcessingEnabled(bool a_switch) noexcept
		{
			m_flags.set(EffectControllerFlags::kEnablePhysics, a_switch);
		}

		inline constexpr void SetShaderProcessingEnabled(bool a_switch) noexcept
		{
			m_flags.set(EffectControllerFlags::kEnableShaders, a_switch);
		}

	private:
		void ProcessEffectsImpl(const ActorObjectMap& a_map);

		void PreparePhysicsUpdateData(
			float                          a_interval,
			std::optional<PhysUpdateData>& a_data);

		void RunUpdates(
			const float                          a_interval,
			const Game::Unk2f6b948::Steps&       a_stepMuls,
			const std::optional<PhysUpdateData>& a_physUpdData,
			const ActorObjectHolder&             a_holder);

		static void UpdateShaders(
			const float              a_step,
			const ActorObjectHolder& a_holder);

		static void UpdatePhysics(
			const float              a_stepMul,
			const PhysUpdateData&    a_physUpdData,
			const ActorObjectHolder& a_holder) noexcept;

		SKMP_FORCEINLINE static void UpdateShadersOnDisplay(
			const EffectShaderData&       a_data,
			const ObjectEntryBase::State& a_state,
			float                         a_step);

		SKMP_FORCEINLINE static void UpdateShadersOnEquipped(
			Actor*                  a_actor,
			const EffectShaderData& a_data,
			float                   a_step);

		static void ProcessNiObjectTree(
			NiAVObject*                    a_object,
			const EffectShaderData::Entry& a_entry);

		SKMP_FORCEINLINE static void UpdateObjectShaders(
			Actor*                   a_actor,
			const ObjectEntryCustom& a_entry,
			float                    a_step);

		SKMP_FORCEINLINE static void UpdateObjectShaders(
			Actor*                 a_actor,
			const ObjectEntrySlot& a_entry,
			float                  a_step);

		mutable PerfTimerInt m_timer{ 1000000LL };
		mutable long long    m_currentTime{ 0LL };

		float m_timeAccum{ 0.0f };
		float m_averageInterval{ 1.0f / 60.0f };

		stl::flag<EffectControllerFlags> m_flags{ EffectControllerFlags::kNone };
	};

	DEFINE_ENUM_CLASS_BITWISE(EffectController::EffectControllerFlags);

}
