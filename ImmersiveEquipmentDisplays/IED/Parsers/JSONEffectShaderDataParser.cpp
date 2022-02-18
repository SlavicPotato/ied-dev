#include "pch.h"

#include "JSONConfigColorRGBAParser.h"
#include "JSONEffectShaderDataParser.h"
#include "JSONEffectShaderTextureParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configEffectShaderData_t>::Parse(
			const Json::Value&              a_in,
			Data::configEffectShaderData_t& a_out) const
		{
			a_out.flags = a_in.get("flags", stl::underlying(Data::EffectShaderDataFlags::kNone)).asUInt();

			if (auto& inset = a_in["tn"])
			{
				for (auto& e : inset)
				{
					a_out.targetNodes.emplace(e.asString());
				}
			}

			Parser<Data::configEffectShaderTexture_t> texparser(m_state);

			texparser.Parse(a_in["btex"], a_out.baseTexture);
			texparser.Parse(a_in["ptex"], a_out.paletteTexture);
			texparser.Parse(a_in["otex"], a_out.blockOutTexture);

			Parser<Data::configColorRGBA_t> rgbaparser(m_state);

			rgbaparser.Parse(a_in["fcol"], a_out.fillColor);
			rgbaparser.Parse(a_in["rcol"], a_out.rimColor);

			return true;
		}

		template <>
		void Parser<Data::configEffectShaderData_t>::Create(
			const Data::configEffectShaderData_t& a_data,
			Json::Value&                          a_out) const
		{
			a_out["flags"] = a_data.flags.underlying();

			auto& outset = (a_out["tn"] = Json::Value(Json::ValueType::arrayValue));

			for (auto& e : a_data.targetNodes)
			{
				outset.append(*e);
			}

			Parser<Data::configEffectShaderTexture_t> texparser(m_state);

			texparser.Create(a_data.baseTexture, a_out["btex"]);
			texparser.Create(a_data.paletteTexture, a_out["ptex"]);
			texparser.Create(a_data.blockOutTexture, a_out["otex"]);

			Parser<Data::configColorRGBA_t> rgbaparser(m_state);

			rgbaparser.Create(a_data.fillColor, a_out["fcol"]);
			rgbaparser.Create(a_data.rimColor, a_out["rcol"]);
		}

		template <>
		void Parser<Data::configEffectShaderData_t>::GetDefault(
			Data::configEffectShaderData_t& a_out) const
		{}
	}
}