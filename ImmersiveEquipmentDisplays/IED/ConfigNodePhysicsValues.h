#pragma once

#include "ConfigLUIDTag.h"

namespace IED
{
	namespace Data
	{
		enum class ConfigNodePhysicsFlags : std::uint32_t
		{
			kNone = 0,

			kDisabled = 1u << 0,

			kEnableBoxConstraint    = 1u << 16,
			kEnableSphereConstraint = 1u << 17,

			kConstraintMask =
				kEnableSphereConstraint |
				kEnableBoxConstraint
		};

		DEFINE_ENUM_CLASS_BITWISE(ConfigNodePhysicsFlags);

		struct configNodePhysicsValues_t :
			configLUIDTagAG_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			inline void clear() noexcept
			{
				*this = {};
			}

			stl::flag<ConfigNodePhysicsFlags> valueFlags{ ConfigNodePhysicsFlags::kNone };

			btVector3 maxOffsetSphereOffset{ DirectX::g_XMZero.v };
			btVector3 maxOffsetN{ -20.0f, -20.0f, -20.0f };
			btVector3 maxOffsetP{ 20.0f, 20.0f, 20.0f };
			btVector3 cogOffset{ DirectX::g_XMZero.v };
			btVector3 linear{ DirectX::g_XMZero.v };
			btVector3 rotational{ DirectX::g_XMOne3.v };
			btVector3 rotAdjust{ DirectX::g_XMZero.v };
			btVector4 maxOffsetParamsSphere{ 0.1f, 20000.0f, 0.0f, 1.0f };
			btVector4 maxOffsetParamsBox{ 0.1f, 20000.0f, 0.0f, 1.0f };

			float stiffness{ 2.0f };
			float stiffness2{ 1.0f };
			float springSlackOffset{ 0.0f };
			float springSlackMag{ 0.0f };
			float damping{ 0.95f };
			float maxOffsetSphereRadius{ 20.0f };
			float gravityBias{ 1200.0f };
			float gravityCorrection{ 0.0f };
			float rotGravityCorrection{ 0.0f };
			float resistance{ 0.0f };
			float mass{ 1.0f };
			float maxVelocity{ 20000.0f };

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& valueFlags.value;

				a_ar& maxOffsetSphereOffset.mVec128.m128_f32;
				a_ar& maxOffsetN.mVec128.m128_f32;
				a_ar& maxOffsetP.mVec128.m128_f32;
				a_ar& cogOffset.mVec128.m128_f32;
				a_ar& linear.mVec128.m128_f32;
				a_ar& rotational.mVec128.m128_f32;
				a_ar& rotAdjust.mVec128.m128_f32;
				a_ar& maxOffsetParamsSphere.mVec128.m128_f32;
				a_ar& maxOffsetParamsBox.mVec128.m128_f32;

				a_ar& stiffness;
				a_ar& stiffness2;
				a_ar& springSlackOffset;
				a_ar& springSlackMag;
				a_ar& damping;
				a_ar& maxOffsetSphereRadius;
				a_ar& gravityBias;
				a_ar& gravityCorrection;
				a_ar& rotGravityCorrection;
				a_ar& resistance;
				a_ar& mass;
				a_ar& maxVelocity;
			}
		};
	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configNodePhysicsValues_t,
	::IED::Data::configNodePhysicsValues_t::Serialization::DataVersion1);
