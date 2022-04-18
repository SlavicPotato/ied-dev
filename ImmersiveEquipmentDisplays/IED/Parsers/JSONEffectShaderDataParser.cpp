#include "pch.h"

#include "JSONConfigColorRGBAParser.h"
#include "JSONEffectShaderDataParser.h"
#include "JSONEffectShaderFunctionListParser.h"
#include "JSONEffectShaderTextureParser.h"

namespace IED
{
	static constexpr void clamp_alpha_func(NiProperty::AlphaFunction& a_data) noexcept
	{
		switch (a_data)
		{
		case NiProperty::AlphaFunction::kOne:
		case NiProperty::AlphaFunction::kZero:
		case NiProperty::AlphaFunction::kSrcColor:
		case NiProperty::AlphaFunction::kInvSrcColor:
		case NiProperty::AlphaFunction::kDestColor:
		case NiProperty::AlphaFunction::kInvDestColor:
		case NiProperty::AlphaFunction::kSrcAlpha:
		case NiProperty::AlphaFunction::kInvSrcAlpha:
		case NiProperty::AlphaFunction::kDestAlpha:
		case NiProperty::AlphaFunction::kInvDestAlpha:
		case NiProperty::AlphaFunction::kSrcAlphaSat:
			break;
		default:
			a_data = NiProperty::AlphaFunction::kSrcAlpha;
			break;
		}
	}

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

			a_out.zTestFunc = static_cast<DepthStencilDepthMode>(
				a_in.get("dsm", stl::underlying(DepthStencilDepthMode::kTest)).asInt());

			a_out.srcBlend = static_cast<NiAlphaProperty::AlphaFunction>(
				a_in.get("sb", stl::underlying(NiAlphaProperty::AlphaFunction::kSrcAlpha)).asInt());

			a_out.destBlend = static_cast<NiAlphaProperty::AlphaFunction>(
				a_in.get("db", stl::underlying(NiAlphaProperty::AlphaFunction::kInvSrcAlpha)).asInt());

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

			switch (a_out.zTestFunc)
			{
			case DepthStencilDepthMode::kDisabled:
			case DepthStencilDepthMode::kTest:
			case DepthStencilDepthMode::kWrite:
			case DepthStencilDepthMode::kTestWrite:
			case DepthStencilDepthMode::kTestEqual:
			case DepthStencilDepthMode::kTestGreaterEqual:
			case DepthStencilDepthMode::kTestGreater:
				break;
			default:
				a_out.zTestFunc = DepthStencilDepthMode::kTest;
				break;
			}

			clamp_alpha_func(a_out.srcBlend);
			clamp_alpha_func(a_out.destBlend);

			a_out.baseFillScale = a_in.get("bfs", 1.0f).asFloat();
			a_out.baseFillAlpha = a_in.get("bfa", 1.0f).asFloat();
			a_out.baseRimAlpha  = a_in.get("bra", 1.0f).asFloat();

			a_out.uvo.uOffset = a_in.get("uo", 0.0f).asFloat();
			a_out.uvo.vOffset = a_in.get("vo", 0.0f).asFloat();

			a_out.uvp.uScale = a_in.get("us", 1.0f).asFloat();
			a_out.uvp.vScale = a_in.get("vs", 1.0f).asFloat();

			a_out.edgeExponent  = a_in.get("ee", 1.0f).asFloat();
			a_out.boundDiameter = a_in.get("bd", 0.0f).asFloat();

			Parser<Data::configEffectShaderFunctionList_t> flparser(m_state);

			if (!flparser.Parse(a_in["fn"], a_out.functions))
			{
				return false;
			}

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

			a_out["tcm"] = stl::underlying(a_data.textureClampMode);
			a_out["dsm"] = stl::underlying(a_data.zTestFunc);
			a_out["sb"]  = stl::underlying(a_data.srcBlend);
			a_out["db"]  = stl::underlying(a_data.destBlend);

			a_out["bfs"] = a_data.baseFillScale;
			a_out["bfa"] = a_data.baseFillAlpha;
			a_out["bra"] = a_data.baseRimAlpha;

			a_out["uo"] = a_data.uvo.uOffset;
			a_out["vo"] = a_data.uvo.vOffset;

			a_out["us"] = a_data.uvp.uScale;
			a_out["vs"] = a_data.uvp.vScale;

			a_out["ee"] = a_data.edgeExponent;
			a_out["bd"] = a_data.boundDiameter;

			Parser<Data::configEffectShaderFunctionList_t> flparser(m_state);

			flparser.Create(a_data.functions, a_out["fn"]);
		}

	}
}