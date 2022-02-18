#include "pch.h"

#include "JSONConfigColorRGBAParser.h"
#include "JSONEffectShaderDataParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configEffectShaderTexture_t>::Parse(
			const Json::Value&                 a_in,
			Data::configEffectShaderTexture_t& a_out) const
		{
			a_out.flags = a_in.get("flags", stl::underlying(Data::EffectShaderDataFlags::kNone)).asUInt();
			a_out.path  = a_in["path"].asString();

			return true;
		}

		template <>
		void Parser<Data::configEffectShaderTexture_t>::Create(
			const Data::configEffectShaderTexture_t& a_data,
			Json::Value&                             a_out) const
		{
			a_out["flags"] = a_data.flags.underlying();
			a_out["btex"]  = a_data.path;
		}

		template <>
		void Parser<Data::configEffectShaderTexture_t>::GetDefault(
			Data::configEffectShaderTexture_t& a_out) const
		{}
	}
}