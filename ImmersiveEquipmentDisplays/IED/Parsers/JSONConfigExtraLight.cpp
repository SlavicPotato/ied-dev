#include "pch.h"

#include "JSONConfigExtraLight.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraLight_t>::Parse(
			const Json::Value&        a_in,
			Data::configExtraLight_t& a_out) const
		{
			a_out.data.flags           = a_in.get("flags", stl::underlying(Data::ExtraLightData::DEFAULT_FLAGS)).asUInt();
			a_out.data.shadowDepthBias = a_in.get("sdb", 0.0f).asFloat();
			a_out.data.fieldOfView     = a_in.get("fov", 0.0f).asFloat();

			return true;
		}

		template <>
		void Parser<Data::configExtraLight_t>::Create(
			const Data::configExtraLight_t& a_data,
			Json::Value&                    a_out) const
		{
			a_out["flags"] = a_data.data.flags.underlying();
			a_out["sdb"]   = a_data.data.shadowDepthBias;
			a_out["fov"]   = a_data.data.fieldOfView;
		}

	}
}