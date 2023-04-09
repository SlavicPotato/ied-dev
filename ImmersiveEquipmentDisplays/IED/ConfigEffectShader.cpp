#include "pch.h"

#include "ConfigEffectShader.h"

#include <ext/BSGraphics.h>
#include <ext/Model.h>

namespace IED
{
	namespace Data
	{
		bool configEffectShaderData_t::create_shader_data(
			RE::BSTSmartPointer<BSEffectShaderData>& a_out) const
		{
			auto result = RE::make_smart<BSEffectShaderData>();

			load_texture(baseTexture, true, result->baseTexture);

			if (!result->baseTexture)
			{
				return false;
			}

			load_texture(paletteTexture, false, result->paletteTexture);
			load_texture(blockOutTexture, false, result->blockOutTexture);

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

			if (result->paletteTexture)
			{
				result->grayscaleToColor = flags.test(EffectShaderDataFlags::kGrayscaleToColor);
				result->grayscaleToAlpha = flags.test(EffectShaderDataFlags::kGrayscaleToAlpha);
			}

			result->ignoreTextureAlpha      = flags.test(EffectShaderDataFlags::kIgnoreTextureAlpha);
			result->baseTextureProjectedUVs = flags.test(EffectShaderDataFlags::kBaseTextureProjectedUVs);
			result->ignoreBaseGeomTexAlpha  = flags.test(EffectShaderDataFlags::kIgnoreBaseGeomTexAlpha);
			result->lighting                = flags.test(EffectShaderDataFlags::kLighting);
			result->alpha                   = flags.test(EffectShaderDataFlags::kAlpha);

			a_out = std::move(result);

			return true;
		}

		void configEffectShaderData_t::load_texture(
			const configEffectShaderTexture_t& a_in,
			bool                               a_force_default,
			NiPointer<NiTexture>&              a_out)
		{
			if (a_in.fbf.selected == EffectShaderSelectedTexture::Custom &&
			    !a_in.path.empty())
			{
				char path_buffer[MAX_PATH];

				const auto path = Util::Model::MakePath(
					"textures",
					a_in.path.c_str(),
					path_buffer);

				LoadTexture(
					path,
					std::uint8_t(1),
					a_out,
					false);
			}

			if (!a_out)
			{
				if (auto gstate = BSGraphics::State::GetSingleton())
				{
					switch (a_in.fbf.selected)
					{
					case EffectShaderSelectedTexture::White:
						a_out = gstate->defaultTextureWhite;
						break;
					case EffectShaderSelectedTexture::Grey:
						a_out = gstate->defaultTextureGrey;
						break;
					case EffectShaderSelectedTexture::Black:
						a_out = gstate->unk058;
						break;
					}

					if (!a_out && a_force_default)
					{
						a_out = gstate->defaultTextureWhite;
					}
				}
			}
		}
	}
}