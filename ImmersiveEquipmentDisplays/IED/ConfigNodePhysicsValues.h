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
			configLUIDTagGI_t
		{
		private:
			friend class boost::serialization::access;

		public:

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			void clear()
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
			float maxOffsetSphereFriction{ 0.025f };
			float maxOffsetBoxFriction{ 0.025f };

			float gravityBias{ 1200.0f };
			float gravityCorrection{ 0.0f };
			float rotGravityCorrection{ 0.0f };
			float resistance{ 0.0f };

			float mass{ 1.0f };
			float maxVelocity{ 20000.0f };

		private:
			template <class Archive>
			void serialize_btVector3(Archive& a_ar, const btVector3& a_member) const
			{
				a_ar& a_member.mVec128.m128_f32[0];
				a_ar& a_member.mVec128.m128_f32[1];
				a_ar& a_member.mVec128.m128_f32[2];
			}

			template <class Archive>
			void deserialize_btVector3(Archive& a_ar, btVector3& a_member)
			{
				a_ar& a_member.mVec128.m128_f32[0];
				a_ar& a_member.mVec128.m128_f32[1];
				a_ar& a_member.mVec128.m128_f32[2];
				a_member.mVec128.m128_f32[3] = 0.0f;
			}

			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& valueFlags.value;

				serialize_btVector3(a_ar, maxOffsetSphereOffset);
				serialize_btVector3(a_ar, maxOffsetN);
				serialize_btVector3(a_ar, maxOffsetP);
				serialize_btVector3(a_ar, cogOffset);
				serialize_btVector3(a_ar, linear);
				serialize_btVector3(a_ar, rotational);
				serialize_btVector3(a_ar, rotAdjust);

				a_ar& maxOffsetParamsSphere.mVec128.m128_f32;
				a_ar& maxOffsetParamsBox.mVec128.m128_f32;

				a_ar& stiffness;
				a_ar& stiffness2;
				a_ar& springSlackOffset;
				a_ar& springSlackMag;
				a_ar& damping;
				a_ar& maxOffsetSphereRadius;
				a_ar& maxOffsetSphereFriction;
				a_ar& maxOffsetBoxFriction;
				a_ar& gravityBias;
				a_ar& gravityCorrection;
				a_ar& rotGravityCorrection;
				a_ar& resistance;
				a_ar& mass;
				a_ar& maxVelocity;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& valueFlags.value;

				deserialize_btVector3(a_ar, maxOffsetSphereOffset);
				deserialize_btVector3(a_ar, maxOffsetN);
				deserialize_btVector3(a_ar, maxOffsetP);
				deserialize_btVector3(a_ar, cogOffset);
				deserialize_btVector3(a_ar, linear);
				deserialize_btVector3(a_ar, rotational);
				deserialize_btVector3(a_ar, rotAdjust);

				a_ar& maxOffsetParamsSphere.mVec128.m128_f32;
				a_ar& maxOffsetParamsBox.mVec128.m128_f32;

				a_ar& stiffness;
				a_ar& stiffness2;
				a_ar& springSlackOffset;
				a_ar& springSlackMag;
				a_ar& damping;
				a_ar& maxOffsetSphereRadius;
				a_ar& maxOffsetSphereFriction;
				a_ar& maxOffsetBoxFriction;
				a_ar& gravityBias;
				a_ar& gravityCorrection;
				a_ar& rotGravityCorrection;
				a_ar& resistance;
				a_ar& mass;
				a_ar& maxVelocity;
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};
	}

}

BOOST_CLASS_VERSION(
	::IED::Data::configNodePhysicsValues_t,
	::IED::Data::configNodePhysicsValues_t::Serialization::DataVersion1);
