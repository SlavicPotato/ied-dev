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
					auto tmp = e.asString();
					if (!tmp.empty())
					{
						a_out.targetNodes.emplace(std::move(tmp));
					}
				}
			}

			Parser<Data::configEffectShaderTexture_t> texparser(m_state);

			texparser.Parse(a_in["btex"], a_out.baseTexture);
			texparser.Parse(a_in["ptex"], a_out.paletteTexture);
			texparser.Parse(a_in["otex"], a_out.blockOutTexture);

			Parser<Data::configColorRGBA_t> rgbaparser(m_state);

			rgbaparser.Parse(a_in["fcol"], a_out.fillColor, 1.0f);
			rgbaparser.Parse(a_in["rcol"], a_out.rimColor, 0.0f);

			a_out.textureClampMode = static_cast<TextureAddressMode>(
				a_in.get("tcm", stl::underlying(TextureAddressMode::kWrapSWrapT)).asInt());

			switch (a_out.textureClampMode)
			{
			case TextureAddressMode::kClampSClampT:
			case TextureAddressMode::kClampSWrapT:
			case TextureAddressMode::kWrapSClampT:
			case TextureAddressMode::kWrapSWrapT:
				break;
			default:
				a_out.textureClampMode = TextureAddressMode::kWrapSWrapT;
				break;
			}

			a_out.baseFillScale = a_in.get("bfs", 1.0f).asFloat();
			a_out.baseFillAlpha = a_in.get("bfa", 1.0f).asFloat();
			a_out.baseRimAlpha  = a_in.get("bra", 1.0f).asFloat();

			a_out.uOffset = a_in.get("uo", 0.0f).asFloat();
			a_out.vOffset = a_in.get("vo", 0.0f).asFloat();

			a_out.uScale = a_in.get("us", 1.0f).asFloat();
			a_out.vScale = a_in.get("vs", 1.0f).asFloat();

			a_out.edgeExponent  = a_in.get("ee", 1.0f).asFloat();
			a_out.boundDiameter = a_in.get("bd", 0.0f).asFloat();

			return true;
		}

		template <>
		void Parser<Data::configEffectShaderData_t>::Create(
			const Data::configEffectShaderData_t& a_data,
			Json::Value&                          a_out) const
		{
			a_out["flags"] = a_data.flags.underlying();

			if (!a_data.targetNodes.empty())
			{
				auto& outset = (a_out["tn"] = Json::Value(Json::ValueType::arrayValue));

				for (auto& e : a_data.targetNodes)
				{
					if (!e.empty())
					{
						outset.append(*e);
					}
				}
			}

			Parser<Data::configEffectShaderTexture_t> texparser(m_state);

			texparser.Create(a_data.baseTexture, a_out["btex"]);
			texparser.Create(a_data.paletteTexture, a_out["ptex"]);
			texparser.Create(a_data.blockOutTexture, a_out["otex"]);

			Parser<Data::configColorRGBA_t> rgbaparser(m_state);

			rgbaparser.Create(a_data.fillColor, a_out["fcol"]);
			rgbaparser.Create(a_data.rimColor, a_out["rcol"]);

			a_out["tcm"] = static_cast<std::int32_t>(a_data.textureClampMode);

			a_out["bfs"] = a_data.baseFillScale;
			a_out["bfa"] = a_data.baseFillAlpha;
			a_out["bra"] = a_data.baseRimAlpha;

			a_out["uo"] = a_data.uOffset;
			a_out["vo"] = a_data.vOffset;

			a_out["us"] = a_data.uScale;
			a_out["vs"] = a_data.vScale;

			a_out["ee"] = a_data.edgeExponent;
			a_out["bd"] = a_data.boundDiameter;
		}

	}
}