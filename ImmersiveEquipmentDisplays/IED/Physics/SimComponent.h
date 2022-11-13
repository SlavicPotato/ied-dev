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

			btVector3 m_axis{ m_baseRotAxis };
			btScalar  m_angle{ 0.0f };
			//btScalar m_axisLength;
		private:
			static inline const auto m_baseRotAxis = btVector3(1.0f, 0.0f, 0.0f);
		};

	public:
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

		[[nodiscard]] inline constexpr auto& get_conf() const noexcept
		{
			return m_conf;
		}

		[[nodiscard]] inline constexpr bool operator==(const PHYSimComponent& a_rhs) const noexcept
		{
			return m_conf == a_rhs.m_conf;
		}

	private:
		void ProcessConfig() noexcept;

		SKMP_FORCEINLINE btVector3 CalculateTarget();

		SKMP_FORCEINLINE void LimitVelocity() noexcept;

		SKMP_FORCEINLINE void ConstrainMotionBox(
			const btMatrix3x3& a_parentRot,
			const btMatrix3x3& a_invRot,
			const btVector3&   a_target,
			const btVector3&   a_timeStep) noexcept;

		SKMP_FORCEINLINE void ConstrainMotionSphere(
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
	};
}