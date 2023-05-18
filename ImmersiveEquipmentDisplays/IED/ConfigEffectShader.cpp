#include "pch.h"

#include "ConfigEffectShader.h"

#include <ext/BSGraphics.h>
#include <ext/Model.h>

namespace IED
{
	namespace Data
	{
		RE::BSTSmartPointer<BSEffectShaderData> configEffectShaderData_t::create_shader_data() const
		{
			auto result = RE::make_smart<BSEffectShaderData>();

			result->fillColor = fillColor;
			result->rimColor  = rimColor;

			result->textureClampMode = textureClampMode;
			result->zTestFunc        = zTestFunc;
			result->srcBlend         = srcBlend;
			result->destBlend        = destBlend;
			result->baseFillScale    = baseFillScale;
			result->baseFillAlpha    = baseFillAlpha;
			result->baseRimAlpha     = baseRimAlpha;
			result->uOffset          = uvo.uOffset;
			result->vOffset          = uvo.vOffset;
			result->uScale           = std::clamp(uvp.uScale, 0.0f, 1000.0f);
			result->vScale           = std::clamp(uvp.vScale, 0.0f, 1000.0f);
			result->edgeExponent     = edgeExponent;
			//result->boundDiameter    = boundDiameter;

			result->ignoreTextureAlpha      = flags.test(EffectShaderDataFlags::kIgnoreTextureAlpha);
			result->baseTextureProjectedUVs = flags.test(EffectShaderDataFlags::kBaseTextureProjectedUVs);
			result->ignoreBaseGeomTexAlpha  = flags.test(EffectShaderDataFlags::kIgnoreBaseGeomTexAlpha);
			result->lighting                = flags.test(EffectShaderDataFlags::kLighting);
			result->alpha                   = flags.test(EffectShaderDataFlags::kAlpha);

			return result;
		}

		NiPointer<NiTexture> configEffectShaderTexture_t::load_texture(
			bool a_force_default) const noexcept
		{
			NiPointer<NiTexture> result;

			if (has_custom())
			{
				char path_buffer[MAX_PATH];

				const auto p = Util::Model::MakePath(
					"textures",
					path.c_str(),
					path_buffer);

				LoadTexture(
					p,
					std::uint8_t(1),
					result,
					false);
			}

			if (!result)
			{
				if (auto gstate = BSGraphics::State::GetSingleton())
				{
					switch (flags.bf().selected)
					{
					case EffectShaderSelectedTexture::White:
						result = gstate->defaultTextureWhite;
						break;
					case EffectShaderSelectedTexture::Grey:
						result = gstate->defaultTextureGrey;
						break;
					case EffectShaderSelectedTexture::Black:
						result = gstate->unk058;
						break;
					}

					if (!result && a_force_default)
					{
						result = gstate->defaultTextureWhite;
					}
				}
			}

			return result;
		}
	}
}