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
	public:
		struct PhysicsUpdateData
		{
			float timeTick;
			float timeStep;
			float maxTime;
			float timeAccum;
		};

		enum class Flags : std::uint8_t
		{
			kNone = 0,

			kEnableShaders = 1u << 0,
			kEnablePhysics = 1u << 1,

			kParallelProcessing = 1u << 2,

			kEnableMask =
				kEnableShaders |
				kEnablePhysics
		};

		[[nodiscard]] constexpr bool ShaderProcessingEnabled() const noexcept
		{
			return m_flags.test(Flags::kEnableShaders);
		}

		[[nodiscard]] constexpr bool PhysicsProcessingEnabled() const noexcept
		{
			return m_flags.test(Flags::kEnablePhysics);
		}

		constexpr void SetPhysicsProcessingEnabled(bool a_switch) noexcept
		{
			m_flags.set(Flags::kEnablePhysics, a_switch);
		}

		constexpr void SetShaderProcessingEnabled(bool a_switch) noexcept
		{
			m_flags.set(Flags::kEnableShaders, a_switch);
		}

	protected:
		[[nodiscard]] constexpr bool AnyProcessingEnabled() const noexcept
		{
			return m_flags.test_any(Flags::kEnableMask);
		}

		void PreparePhysicsUpdateData(
			float                             a_interval,
			std::optional<PhysicsUpdateData>& a_data) noexcept;

		void RunEffectUpdates(
			const float                              a_interval,
			const Game::Unk2f6b948::TimeMultipliers& a_stepMuls,
			const std::optional<PhysicsUpdateData>&  a_physUpdData,
			const ActorObjectHolder&                 a_holder) noexcept;

		static void UpdateShaders(
			Actor*                   a_actor,
			const float              a_step,
			const ActorObjectHolder& a_holder) noexcept;

		static void UpdatePhysics(
			const float              a_stepMul,
			const PhysicsUpdateData& a_physUpdData,
			const ActorObjectHolder& a_holder) noexcept;

		SKMP_FORCEINLINE static void UpdateShadersOnDisplay(
			const EffectShaderData&       a_data,
			const ObjectEntryBase::State& a_state,
			float                         a_step) noexcept;

		SKMP_FORCEINLINE static void UpdateShadersOnEquipped(
			Actor*                  a_actor,
			const EffectShaderData& a_data,
			float                   a_step) noexcept;

		static void ProcessNiObjectTree(
			NiAVObject*                    a_object,
			const EffectShaderData::Entry& a_entry) noexcept;

		SKMP_FORCEINLINE static void UpdateObjectShaders(
			Actor*                   a_actor,
			const ObjectEntryCustom& a_entry,
			float                    a_step) noexcept;

		SKMP_FORCEINLINE static void UpdateObjectShaders(
			Actor*                 a_actor,
			const ObjectEntrySlot& a_entry,
			float                  a_step) noexcept;

	private:
		float m_timeAccum{ 0.0f };
		float m_averageInterval{ 1.0f / 60.0f };

		stl::flag<Flags> m_flags{ Flags::kNone };
	};

	DEFINE_ENUM_CLASS_BITWISE(EffectController::Flags);

}
