#pragma once

#include "IED/ConfigNodePhysicsValues.h"

#include "Common/BulletExtensions.h"

namespace IED
{
	class PHYSimComponent
	{
		struct rotationParams_t
		{
		public:
			inline void Zero() noexcept
			{
				*this = {};
			}

			btVector3 m_axis{ DirectX::g_XMIdentityR0.v };
			btScalar  m_angle{ 0.0f };
		};

	public:
#if defined(IED_USE_MIMALLOC_SIMCOMPONENT)

		SKMP_ALIGNED_REDEFINE_NEW_MI(16);

#endif

		PHYSimComponent(
			NiAVObject*                            a_object,
			const NiTransform&                     a_initialTransform,
			const Data::configNodePhysicsValues_t& a_conf) noexcept;

		~PHYSimComponent() noexcept;

		void ReadTransforms() noexcept;
		void WriteTransforms() noexcept;

		void Reset() noexcept;

		void UpdateMotion(float a_step) noexcept;
		void UpdateMotion(const btVector3& a_step) noexcept;
		void UpdateConfig(const Data::configNodePhysicsValues_t& a_conf) noexcept;

		[[nodiscard]] inline constexpr bool operator==(const PHYSimComponent& a_rhs) const noexcept
		{
			return m_conf == a_rhs.m_conf;
		}
		
		[[nodiscard]] inline constexpr bool operator==(const luid_tag& a_rhs) const noexcept
		{
			return m_conf == a_rhs;
		}

		inline static constexpr void SetMaxDiff(float a_value) noexcept
		{
			a_value    = std::clamp(a_value, 128.0f, 32768.0f);
			m_maxDiff2 = a_value * a_value;
		}

		[[nodiscard]] inline constexpr auto& GetObjectLocalTransform() const noexcept
		{
			return m_objectLocalTransform;
		}
		
		[[nodiscard]] inline constexpr auto& GetObjectInitialTransform() const noexcept
		{
			return m_initialTransform;
		}

		[[nodiscard]] inline constexpr auto& GetCachedParentWorldTransform() const noexcept
		{
			return m_parentWorldTransform;
		}
		
		[[nodiscard]] Bullet::btTransformEx GetCurrentParentWorldTransform() const noexcept;
		
		[[nodiscard]] inline constexpr auto& GetVirtualPos() const noexcept
		{
			return m_virtld;
		}
		
		[[nodiscard]] inline constexpr auto& GetRotationAxis() const noexcept
		{
			return m_rotParams.m_axis;
		}
		
		[[nodiscard]] inline constexpr auto& GetConfig() const noexcept
		{
			return m_conf;
		}
		
		[[nodiscard]] inline constexpr auto& GetObject() const noexcept
		{
			return m_object;
		}

	private:
		void ProcessConfig() noexcept;

		SKMP_FORCEINLINE btVector3 CalculateTarget() noexcept;

		void LimitVelocity() noexcept;

		void ConstrainMotionBox(
			const btMatrix3x3& a_parentRot,
			const btMatrix3x3& a_invRot,
			const btVector3&   a_target,
			const btVector3&   a_timeStep) noexcept;

		void ConstrainMotionSphere(
			const btMatrix3x3& a_parentRot,
			const btMatrix3x3& a_invRot,
			const btVector3&   a_target,
			const btVector3&   a_timeStep) noexcept;

		Data::configNodePhysicsValues_t m_conf;

		btVector3 m_oldWorldPos;
		btVector3 m_virtld{ DirectX::g_XMZero.v };
		btVector3 m_velocity{ DirectX::g_XMZero.v };

		btVector3 m_gravityCorrection{ DirectX::g_XMZero.v };
		btVector3 m_gravForce{ DirectX::g_XMZero.v };

		DirectX::XMVECTOR m_maxVelocity{ DirectX::g_XMZero.v };

		btVector3 m_rotAdjustParamsX;
		btVector3 m_rotAdjustParamsY;
		btVector3 m_rotAdjustParamsZ;

		btVector3 m_mass;

		rotationParams_t m_rotParams;

		Bullet::btTransformEx m_initialTransform;

		//Bullet::btTransformEx m_objectWorldTransform;
		//btVector3             m_objectWorldPos{ DirectX::g_XMZero.v };
		Bullet::btTransformEx m_objectLocalTransform;
		Bullet::btTransformEx m_parentWorldTransform;

		NiPointer<NiAVObject> m_object;

		btScalar m_maxVelocity2{ 0.0f };

		bool m_hasRotAdjust{ false };
		bool m_resistanceOn{ false };
		bool m_hasSpringSlack{ false };
		bool m_hasFriction{ false };
		bool m_initialIdentity{ false };

		// global settings

		static float m_maxDiff2;
	};
}