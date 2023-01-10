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

		void ReadTransforms(float a_step) noexcept;
		void WriteTransforms() noexcept;

		void Reset() noexcept;

		void UpdateMotion(float a_step) noexcept;
		void UpdateMotion(const btVector3& a_step) noexcept;
		void UpdateConfig(const Data::configNodePhysicsValues_t& a_conf) noexcept;

		[[nodiscard]] inline constexpr bool operator==(const PHYSimComponent& a_rhs) const noexcept
		{
			return m_tag == a_rhs.m_tag;
		}

		[[nodiscard]] inline constexpr bool operator==(const luid_tag& a_rhs) const noexcept
		{
			return m_tag == a_rhs;
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

		[[nodiscard]] const NiTransform& GetCurrentParentWorldTransform() const noexcept;

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

		[[nodiscard]] inline constexpr auto& GetLUID() const noexcept
		{
			return m_tag;
		}

		[[nodiscard]] inline constexpr auto& GetObject() const noexcept
		{
			return m_object;
		}

		[[nodiscard]] inline constexpr auto& GetVelocity() const noexcept
		{
			return m_velocity;
		}

		inline constexpr void XM_CALLCONV ApplyForce(DirectX::XMVECTOR a_target, float a_mag) const noexcept
		{
			m_applyForce.emplace(a_target, a_mag);
		}

	private:
		void ProcessConfig() noexcept;

		SKMP_FORCEINLINE btVector3 CalculateTarget() noexcept;

		void LimitVelocity() noexcept;

		void XM_CALLCONV ConstrainMotionBox(
			const btMatrix3x3&      a_parentRot,
			const btMatrix3x3&      a_invRot,
			const DirectX::XMVECTOR a_target,
			const DirectX::XMVECTOR a_step) noexcept;

		void XM_CALLCONV ConstrainMotionSphere(
			const btMatrix3x3&      a_parentRot,
			const btMatrix3x3&      a_invRot,
			const DirectX::XMVECTOR a_target,
			const DirectX::XMVECTOR a_step) noexcept;

		Data::configNodePhysicsValues_t m_conf;

		Bullet::btTransformEx m_initialTransform;
		Bullet::btTransformEx m_objectLocalTransform;
		Bullet::btTransformEx m_parentWorldTransform;

		btVector3 m_oldWorldPos;
		btVector3 m_virtld{ DirectX::g_XMZero.v };
		btVector3 m_velocity{ DirectX::g_XMZero.v };

		btVector3 m_gravityCorrection{ DirectX::g_XMZero.v };
		btVector3 m_gravForce{ DirectX::g_XMZero.v };

		DirectX::XMVECTOR m_maxVelocity{ DirectX::g_XMZero.v };
		btScalar          m_maxVelocity2{ 0.0f };

		btVector3 m_rotAdjustParamsX;
		btVector3 m_rotAdjustParamsY;
		btVector3 m_rotAdjustParamsZ;

		btVector3 m_mass;

		rotationParams_t m_rotParams;

		btVector3 m_oldParentPos;
		btVector3 m_parentVelocity{ DirectX::g_XMZero.v };

		mutable std::optional<std::pair<DirectX::XMVECTOR, float>> m_applyForce;

		NiPointer<NiAVObject> m_object;

		bool m_hasRotAdjust{ false };
		bool m_resistanceOn{ false };
		bool m_hasSpringSlack{ false };
		bool m_hasFriction{ false };
		bool m_initialIdentity{ false };

		luid_tag m_tag;

		// global settings

		static float m_maxDiff2;
	};
}