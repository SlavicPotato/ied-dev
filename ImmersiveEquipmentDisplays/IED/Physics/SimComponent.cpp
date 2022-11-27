#include "pch.h"

#include "SimComponent.h"

#include "Common/VectorMath.h"

#include "IED/EngineExtensions.h"

namespace IED
{
	using namespace DirectX;

	static constexpr XMVECTOR s_vec10{ 10.0f, 10.0f, 10.0f, 0.0f };
	static constexpr XMVECTOR s_vec10Neg{ -10.0f, -10.0f, -10.0f, 0.0f };

	PHYSimComponent::PHYSimComponent(
		NiAVObject*                            a_object,
		const NiTransform&                     a_initialTransform,
		const Data::configNodePhysicsValues_t& a_conf) noexcept :
		m_conf(a_conf),
		m_tag(ILUID{}()),
		m_initialTransform(Bullet::btTransformEx(a_initialTransform)),
		m_object(a_object)
	{
		m_objectLocalTransform = m_initialTransform;

		m_parentWorldTransform = GetCurrentParentWorldTransform();
		m_oldParentPos         = m_parentWorldTransform.getOrigin();

		m_oldWorldPos = CalculateTarget();

		ProcessConfig();
	}

	PHYSimComponent::~PHYSimComponent() noexcept
	{
		assert(!EngineExtensions::ShouldDefer3DTask());

		m_initialTransform.writeNiTransform(m_object->m_localTransform);
	}

	void PHYSimComponent::ReadTransforms(float a_step) noexcept
	{
		m_parentWorldTransform = GetCurrentParentWorldTransform();

		const auto pos = m_parentWorldTransform.getOrigin();

		m_parentVelocity = (pos - m_oldParentPos) /= a_step;
		m_oldParentPos   = pos;
	}

	void PHYSimComponent::WriteTransforms() noexcept
	{
		m_objectLocalTransform.writeNiTransform(m_object->m_localTransform);
	}

	SKMP_NOINLINE void PHYSimComponent::Reset() noexcept
	{
		m_initialTransform.writeNiTransform(m_object->m_localTransform);
		m_objectLocalTransform = m_initialTransform;

		/*NiAVObject::ControllerUpdateContext ctx{ 0, 0 };
		m_object->UpdateWorldData(std::addressof(ctx));*/

		m_parentWorldTransform = GetCurrentParentWorldTransform();
		m_oldParentPos         = m_parentWorldTransform.getOrigin();

		m_oldWorldPos = CalculateTarget();

		m_virtld.setZero();
		m_velocity.setZero();
		m_parentVelocity.setZero();
		m_rotParams.Zero();
	}

	inline static auto mkQuat(
		const btVector3& a_axis,
		btScalar         a_angle) noexcept
	{
		return btQuaternion(XMQuaternionRotationNormal(a_axis.get128(), a_angle));
	}

	void PHYSimComponent::UpdateMotion(float a_step) noexcept
	{
		UpdateMotion(_mm_set_ps1(a_step));
	}

	void PHYSimComponent::UpdateMotion(const btVector3& a_step) noexcept
	{
		const auto target = CalculateTarget();

		auto diff = target - m_oldWorldPos;

		if (diff.length2() > m_maxDiff2)
		{
			Reset();
			return;
		}

		auto force = diff * m_conf.stiffness;
		force += (diff *= diff.absolute()) *= m_conf.stiffness2;

		if (m_hasSpringSlack)
		{
			const auto m = stl::normalize_clamp(
				m_virtld.length(),
				m_conf.springSlackOffset,
				m_conf.springSlackMag);

			force *= m * m;
		}

		force -= m_gravForce;

		const auto& parentRot = m_parentWorldTransform.getBasis();

		const auto res = m_resistanceOn ? (1.0f - 1.0f / (m_velocity.length() * 0.0075f + 1.0f)) * m_conf.resistance + 1.0f : 1.0f;

		m_velocity -= (m_velocity * a_step) *= (m_conf.damping * res);
		m_velocity += (force / m_mass) *= a_step;

		if (m_applyForce)
		{
			const auto d = m_applyForce->first - m_oldWorldPos.get128();

			m_velocity -= (m_velocity * a_step) *= std::max(20.0f - m_conf.damping * res, 0.0f);
			m_velocity += (d * a_step.get128()) * m_applyForce->second;

			m_applyForce.reset();
		}

		LimitVelocity();

		const auto invRot = parentRot.transpose();
		m_virtld          = invRot * ((m_oldWorldPos + (m_velocity * a_step)) -= target);

		if ((m_conf.valueFlags & Data::ConfigNodePhysicsFlags::kEnableSphereConstraint) == Data::ConfigNodePhysicsFlags::kEnableSphereConstraint)
		{
			ConstrainMotionSphere(parentRot, invRot, target.get128(), a_step.get128());
		}

		if ((m_conf.valueFlags & Data::ConfigNodePhysicsFlags::kEnableBoxConstraint) == Data::ConfigNodePhysicsFlags::kEnableBoxConstraint)
		{
			ConstrainMotionBox(parentRot, invRot, target.get128(), a_step.get128());
		}

		m_oldWorldPos = (parentRot * m_virtld) += target;

		const auto ld = (m_virtld * m_conf.linear) += invRot * m_gravityCorrection;

		auto& locOrigin = m_objectLocalTransform.getOrigin();

		locOrigin = m_initialTransform.getOrigin() + ld;

#if !defined(IED_PERF_BUILD)

		// paranoia
		if (XMVector3IsInfinite(locOrigin.get128()) ||
		    XMVector3IsNaN(locOrigin.get128()))
		{
			Reset();
			return;
		}

#endif

		m_rotParams.m_axis.setX((m_virtld.z() + m_conf.rotGravityCorrection) * m_conf.rotational[2]);
		m_rotParams.m_axis.setY(m_virtld.x() * m_conf.rotational[0]);
		m_rotParams.m_axis.setZ(m_virtld.y() * m_conf.rotational[1]);

		const auto av = m_rotParams.m_axis.get128();

		const auto l2 = XMVector3LengthSq(av);

		if (XMVectorGetX(l2) > _EPSILON * _EPSILON)
		{
			const auto l = XMVectorSqrt(l2);

			m_rotParams.m_axis = XMVectorDivide(av, l);

			if (m_hasRotAdjust)
			{
				m_rotParams.m_axis =
					(m_rotParams.m_axis * m_rotAdjustParamsX) +
					(m_conf.rotAdjust.cross(m_rotParams.m_axis) * m_rotAdjustParamsY) +
					(m_conf.rotAdjust * m_conf.rotAdjust.dotv(m_rotParams.m_axis)) * m_rotAdjustParamsZ;
			}

			m_rotParams.m_angle = XMVectorGetX(l) * std::numbers::pi_v<btScalar> / 180.0f;
		}
		else
		{
			m_rotParams.Zero();
		}

		m_objectLocalTransform.getBasis() =
			m_initialTransform.getBasis() *
			btMatrix3x3(mkQuat(m_rotParams.m_axis, m_rotParams.m_angle));
	}

	void PHYSimComponent::UpdateConfig(
		const Data::configNodePhysicsValues_t& a_conf) noexcept
	{
		m_conf = a_conf;

		ProcessConfig();
	}

	Bullet::btTransformEx PHYSimComponent::GetCurrentParentWorldTransform() const noexcept
	{
		if (auto parent = m_object->m_parent)
		{
			return parent->m_worldTransform;
		}
		else
		{
			// should never happen
			return m_object->m_worldTransform;
		}
	}

	void PHYSimComponent::ProcessConfig() noexcept
	{
		m_hasRotAdjust = !m_conf.rotAdjust.fuzzyZero();

		if (m_hasRotAdjust)
		{
			const auto tmp = VectorMath::XMVectorConvertToRadians(m_conf.rotAdjust.get128());

			XMVECTOR x, y;

			XMVectorSinCos(
				std::addressof(y),
				std::addressof(x),
				XMVector3Length(tmp));

			m_rotAdjustParamsX = x;
			m_rotAdjustParamsY = y;
			m_rotAdjustParamsZ = g_XMOne.v - x;

			m_conf.rotAdjust = XMVector3Normalize(tmp);
		};

		m_gravityCorrection.setZ(std::clamp(m_conf.gravityCorrection, -8192.f, 8192.f));

		m_resistanceOn = m_conf.resistance > 0.0f;

		if (m_resistanceOn)
		{
			m_conf.resistance = std::min(m_conf.resistance, 250.0f);
		}

		m_conf.linear     = XMVectorClamp(m_conf.linear.get128(), g_XMZero.v, s_vec10);
		m_conf.rotational = XMVectorClamp(m_conf.rotational.get128(), s_vec10Neg, s_vec10);

		m_conf.mass = std::clamp(m_conf.mass, 0.001f, 10000.0f);
		m_mass      = _mm_set_ps1(m_conf.mass);

		m_conf.maxVelocity = std::clamp(m_conf.maxVelocity, 1.0f, 20000.0f);
		m_maxVelocity      = _mm_set_ps1(m_conf.maxVelocity);
		m_maxVelocity2     = m_conf.maxVelocity * m_conf.maxVelocity;

		m_conf.maxOffsetParamsBox[0] = std::clamp(m_conf.maxOffsetParamsBox[0], 0.0f, 1.0f);
		m_conf.maxOffsetParamsBox[1] = std::clamp(m_conf.maxOffsetParamsBox[1], 0.0f, 20000.0f);
		m_conf.maxOffsetParamsBox[2] = std::clamp(m_conf.maxOffsetParamsBox[2], 0.0f, 1.0f);
		m_conf.maxOffsetParamsBox[3] = std::clamp(m_conf.maxOffsetParamsBox[3], 0.0f, 200.0f) * 2880.0f;

		m_conf.maxOffsetParamsSphere[0] = std::clamp(m_conf.maxOffsetParamsSphere[0], 0.0f, 4.0f);
		m_conf.maxOffsetParamsSphere[1] = std::clamp(m_conf.maxOffsetParamsSphere[1], 0.0f, 20000.0f);
		m_conf.maxOffsetParamsSphere[2] = std::clamp(m_conf.maxOffsetParamsSphere[2], 0.0f, 1.0f);
		m_conf.maxOffsetParamsSphere[3] = std::clamp(m_conf.maxOffsetParamsSphere[3], 0.0f, 200.0f) * 2880.0f;

		m_conf.maxOffsetN.setMin(g_XMNegativeZero.v);
		m_conf.maxOffsetP.setMax(g_XMZero.v);
		m_conf.maxOffsetBoxFriction = std::clamp(m_conf.maxOffsetBoxFriction, 0.0f, 1.0f);

		m_conf.maxOffsetSphereRadius   = std::max(m_conf.maxOffsetSphereRadius, 0.0f);
		m_conf.maxOffsetSphereFriction = std::clamp(m_conf.maxOffsetSphereFriction, 0.0f, 1.0f);

		m_conf.gravityBias = std::clamp(m_conf.gravityBias, 0.0f, 20000.0f);
		m_gravForce.setZ(m_conf.gravityBias * m_conf.mass);

		m_conf.springSlackOffset = std::max(m_conf.springSlackOffset, 0.0f);
		m_conf.springSlackMag    = std::max(m_conf.springSlackMag, 0.0f);

		m_hasSpringSlack = m_conf.springSlackOffset > 0.0f || m_conf.springSlackMag > 0.0f;

		m_conf.springSlackMag += m_conf.springSlackOffset;

		m_conf.stiffness  = std::clamp(m_conf.stiffness, 0.0f, 20000.0f);
		m_conf.stiffness2 = std::clamp(m_conf.stiffness2, 0.0f, 20000.0f);
	}

	btVector3 PHYSimComponent::CalculateTarget() noexcept
	{
		return m_parentWorldTransform * m_conf.cogOffset;
	}

	void PHYSimComponent::LimitVelocity() noexcept
	{
		const XMVECTOR v = m_velocity.get128();

		const auto l2 = XMVector3LengthSq(v);

		if (XMVectorGetX(l2) > m_maxVelocity2)
		{
			m_velocity = XMVectorMultiply(
				XMVectorDivide(v, XMVectorSqrt(l2)),
				m_maxVelocity);
		}
	}

	void PHYSimComponent::ConstrainMotionBox(
		const btMatrix3x3& a_parentRot,
		const btMatrix3x3& a_invRot,
		const XMVECTOR     a_target,
		const XMVECTOR     a_step) noexcept
	{
		btVector3 depth{ g_XMZero.v };

		bool skip = true;

		auto v = m_virtld.x();

		if (v > m_conf.maxOffsetP.x())
		{
			depth.setX(v - m_conf.maxOffsetP.x());
			skip = false;
		}
		else if (v < m_conf.maxOffsetN.x())
		{
			depth.setX(v - m_conf.maxOffsetN.x());
			skip = false;
		}

		v = m_virtld.y();

		if (v > m_conf.maxOffsetP.y())
		{
			depth.setY(v - m_conf.maxOffsetP.y());
			skip = false;
		}
		else if (v < m_conf.maxOffsetN.y())
		{
			depth.setY(v - m_conf.maxOffsetN.y());
			skip = false;
		}

		v = m_virtld.z();

		if (v > m_conf.maxOffsetP.z())
		{
			depth.setZ(v - m_conf.maxOffsetP.z());
			skip = false;
		}
		else if (v < m_conf.maxOffsetN.z())
		{
			depth.setZ(v - m_conf.maxOffsetN.z());
			skip = false;
		}

		if (skip)
		{
			return;
		}

		const auto n = XMVector3Normalize((a_parentRot * depth).get128());

		const XMVECTOR deltav = (m_velocity - m_parentVelocity).get128();
		const auto     vdotn  = XMVector3Dot(deltav, n);

		auto       impulse = XMVectorGetX(vdotn);
		const auto mag     = XMVectorGetX(XMVector3Length(depth.get128()));

		const auto magThreshold = XMVectorGetX(a_step) * 60.0f;

		if (mag > magThreshold)
		{
			impulse += (XMVectorGetX(a_step) * m_conf.maxOffsetParamsBox[3]) *
			           std::clamp(mag - magThreshold, 0.0f, m_conf.maxOffsetParamsBox[1]);
		}

		if (impulse <= 0.0f)
		{
			return;
		}

		m_velocity -= (deltav - n * vdotn) * m_conf.maxOffsetBoxFriction;

		const auto J = (1.0f + m_conf.maxOffsetParamsBox[2]) * impulse;

		m_velocity -= n * (J * m_conf.maxOffsetParamsBox[0]);

		m_virtld = a_invRot * ((m_oldWorldPos + (m_velocity.get128() * a_step)) -= a_target);
	}

	void PHYSimComponent::ConstrainMotionSphere(
		const btMatrix3x3& a_parentRot,
		const btMatrix3x3& a_invRot,
		const XMVECTOR     a_target,
		const XMVECTOR     a_step) noexcept
	{
		const auto diff = m_virtld - m_conf.maxOffsetSphereOffset;

		const auto difflen = XMVectorGetX(XMVector3Length(diff.get128()));

		const auto radius = m_conf.maxOffsetSphereRadius;

		if (difflen <= radius)
		{
			return;
		}

		const auto n = XMVector3Normalize((a_parentRot * diff).get128());

		const XMVECTOR deltav = (m_velocity - m_parentVelocity).get128();
		const auto     vdotn  = XMVector3Dot(deltav, n);

		auto       impulse = XMVectorGetX(vdotn);
		const auto mag     = difflen - radius;

		const auto magThreshold = XMVectorGetX(a_step) * 60.0f;

		if (mag > magThreshold)
		{
			impulse += XMVectorGetX(a_step) * m_conf.maxOffsetParamsSphere[3] *
			           std::clamp(mag - magThreshold, 0.0f, m_conf.maxOffsetParamsSphere[1]);
		}

		if (impulse <= 0.0f)
		{
			return;
		}

		m_velocity -= (deltav - n * vdotn) * m_conf.maxOffsetSphereFriction;

		const auto J = (1.0f + m_conf.maxOffsetParamsSphere[2]) * impulse;

		m_velocity -= n * (J * m_conf.maxOffsetParamsSphere[0]);

		m_virtld = a_invRot * ((m_oldWorldPos + (m_velocity.get128() * a_step)) -= a_target);
	}

	float PHYSimComponent::m_maxDiff2{
		1024.0f * 1024.0f
	};
}