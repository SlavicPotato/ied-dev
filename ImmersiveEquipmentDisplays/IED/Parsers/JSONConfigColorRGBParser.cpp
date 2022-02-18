#include "pch.h"

#include "JSONConfigColorRGBParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configColorRGB_t>::Parse(
			const Json::Value&      a_in,
			Data::configColorRGB_t& a_out) const
		{
			a_out.r = std::clamp(a_in.get("r", 0.0f).asFloat(), 0.0f, 1.0f);
			a_out.g = std::clamp(a_in.get("g", 0.0f).asFloat(), 0.0f, 1.0f);
			a_out.b = std::clamp(a_in.get("b", 0.0f).asFloat(), 0.0f, 1.0f);

			return true;
		}

		template <>
		void Parser<Data::configColorRGB_t>::Create(
			const Data::configColorRGB_t& a_data,
			Json::Value&                  a_out) const
		{
			a_out["r"] = a_data.r;
			a_out["g"] = a_data.g;
			a_out["b"] = a_data.b;
		}

		template <>
		void Parser<Data::configColorRGB_t>::GetDefault(
			Data::configColorRGB_t& a_out) const
		{}
	}
}