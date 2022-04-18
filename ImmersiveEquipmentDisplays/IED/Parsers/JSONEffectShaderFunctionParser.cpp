#include "pch.h"

#include "JSONEffectShaderFunctionParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configEffectShaderFunction_t>::Parse(
			const Json::Value&                  a_in,
			Data::configEffectShaderFunction_t& a_out,
			const std::uint32_t                 a_version) const
		{
			a_out.flags = a_in.get(
								  "flags",
								  stl::underlying(Data::EffectShaderFunctionFlags::kNone))
			                  .asUInt();

			a_out.type = static_cast<Data::EffectShaderFunctionType>(
				a_in.get(
						"type",
						stl::underlying(Data::EffectShaderFunctionType::None))
					.asUInt());

			a_out.f32a     = a_in.get("f32a", 0.0f).asFloat();
			a_out.f32b     = a_in.get("f32b", 0.0f).asFloat();
			a_out.f32c     = a_in.get("f32c", 0.0f).asFloat();
			a_out.fde.f32d = a_in.get("f32d", 0.0f).asFloat();
			a_out.fde.f32e = a_in.get("f32e", 0.0f).asFloat();
			a_out.ffg.f32f = a_in.get("f32f", 0.0f).asFloat();
			a_out.ffg.f32g = a_in.get("f32g", 0.0f).asFloat();
			a_out.fhi.f32h = a_in.get("f32h", 0.0f).asFloat();
			a_out.fhi.f32i = a_in.get("f32i", 0.0f).asFloat();
			a_out.u32a     = a_in.get("u32a", 0).asUInt();

			return true;
		}

		template <>
		void Parser<Data::configEffectShaderFunction_t>::Create(
			const Data::configEffectShaderFunction_t& a_data,
			Json::Value&                              a_out) const
		{
			a_out["flags"] = a_data.flags.underlying();
			a_out["type"]  = stl::underlying(a_data.type);

			a_out["f32a"] = a_data.f32a;
			a_out["f32b"] = a_data.f32b;
			a_out["f32c"] = a_data.f32c;
			a_out["f32d"] = a_data.fde.f32d;
			a_out["f32e"] = a_data.fde.f32e;
			a_out["f32f"] = a_data.ffg.f32f;
			a_out["f32g"] = a_data.ffg.f32g;
			a_out["f32h"] = a_data.fhi.f32h;
			a_out["f32i"] = a_data.fhi.f32i;
			a_out["u32a"] = a_data.u32a;
		}

	}
}