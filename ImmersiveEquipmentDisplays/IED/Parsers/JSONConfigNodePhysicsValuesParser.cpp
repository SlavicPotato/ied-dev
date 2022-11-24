#include "pch.h"

#include "JSONConfigNodePhysicsValuesParser.h"

#include "JSONBulletVector3Parser.h"
#include "JSONBulletVector4Parser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodePhysicsValues_t>::Parse(
			const Json::Value&               a_in,
			Data::configNodePhysicsValues_t& a_out) const
		{
			Parser<btVector3> v3parser(m_state);
			Parser<btVector4> v4parser(m_state);

			a_out.valueFlags = static_cast<Data::ConfigNodePhysicsFlags>(
				a_in.get("f", stl::underlying(Data::ConfigNodePhysicsFlags::kNone)).asUInt());

			v3parser.Parse(a_in["so"], a_out.maxOffsetSphereOffset);
			v3parser.Parse(a_in["on"], a_out.maxOffsetN);
			v3parser.Parse(a_in["op"], a_out.maxOffsetP);
			v3parser.Parse(a_in["co"], a_out.cogOffset);
			v3parser.Parse(a_in["l"], a_out.linear);
			v3parser.Parse(a_in["r"], a_out.rotational);
			v3parser.Parse(a_in["ra"], a_out.rotAdjust);

			v4parser.Parse(a_in["ps"], a_out.maxOffsetParamsSphere);
			v4parser.Parse(a_in["pb"], a_out.maxOffsetParamsBox);

			a_out.stiffness               = a_in["s"].asFloat();
			a_out.stiffness2              = a_in["q"].asFloat();
			a_out.springSlackOffset       = a_in["sl"].asFloat();
			a_out.springSlackMag          = a_in["sm"].asFloat();
			a_out.damping                 = a_in["d"].asFloat();
			a_out.maxOffsetSphereRadius   = a_in["sr"].asFloat();
			a_out.maxOffsetSphereFriction = a_in["sf"].asFloat();
			a_out.maxOffsetBoxFriction    = a_in["bf"].asFloat();
			a_out.gravityBias             = a_in["b"].asFloat();
			a_out.gravityCorrection       = a_in["c"].asFloat();
			a_out.rotGravityCorrection    = a_in["rc"].asFloat();
			a_out.resistance              = a_in["g"].asFloat();
			a_out.mass                    = a_in["m"].asFloat();
			a_out.maxVelocity             = a_in["v"].asFloat();

			return true;
		}

		template <>
		void Parser<Data::configNodePhysicsValues_t>::Create(
			const Data::configNodePhysicsValues_t& a_data,
			Json::Value&                           a_out) const
		{
			Parser<btVector3> v3parser(m_state);
			Parser<btVector4> v4parser(m_state);

			a_out["f"] = a_data.valueFlags.underlying();

			v3parser.Create(a_data.maxOffsetSphereOffset, a_out["so"]);
			v3parser.Create(a_data.maxOffsetN, a_out["on"]);
			v3parser.Create(a_data.maxOffsetP, a_out["op"]);
			v3parser.Create(a_data.cogOffset, a_out["co"]);
			v3parser.Create(a_data.linear, a_out["l"]);
			v3parser.Create(a_data.rotational, a_out["r"]);
			v3parser.Create(a_data.rotAdjust, a_out["ra"]);

			v4parser.Create(a_data.maxOffsetParamsSphere, a_out["ps"]);
			v4parser.Create(a_data.maxOffsetParamsBox, a_out["pb"]);

			a_out["s"]  = a_data.stiffness;
			a_out["q"]  = a_data.stiffness2;
			a_out["sl"] = a_data.springSlackOffset;
			a_out["sm"] = a_data.springSlackMag;
			a_out["d"]  = a_data.damping;
			a_out["sr"] = a_data.maxOffsetSphereRadius;
			a_out["sf"] = a_data.maxOffsetSphereFriction;
			a_out["bf"] = a_data.maxOffsetBoxFriction;
			a_out["b"]  = a_data.gravityBias;
			a_out["c"]  = a_data.gravityCorrection;
			a_out["rc"] = a_data.rotGravityCorrection;
			a_out["g"]  = a_data.resistance;
			a_out["m"]  = a_data.mass;
			a_out["v"]  = a_data.maxVelocity;
		}

	}
}