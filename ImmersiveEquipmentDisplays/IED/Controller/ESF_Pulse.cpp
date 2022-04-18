#include "pch.h"

#include "ESF_Pulse.h"

#include <ext/stl_math.h>

namespace IED
{
	namespace ESF
	{
		void Pulse::UpdateConfigImpl(
			const Data::configEffectShaderFunction_t& a_data)
		{
			function    = a_data.fbf.type;
			flags       = a_data.pulseFlags;
			ftp         = std::clamp(a_data.speed, 0.01f, 300.0f) * PI2;
			inverse     = a_data.flags.test(Data::EffectShaderFunctionFlags::kAdditiveInverse);
			exponential = a_data.flags.test(Data::EffectShaderFunctionFlags::kExponential);

			if (exponential)
			{
				exponent = std::clamp(a_data.exponent, -20.0f, 20.0f);
				if (stl::is_equal(exponent, 0.0f))
				{
					exponential = false;
				}
				else
				{
					maxexpr = std::powf(2.0f, exponent) - 1.0f;
				}
			}

			if ((flags & Data::EffectShaderPulseFlags::uScale) == Data::EffectShaderPulseFlags::uScale)
			{
				float min = std::clamp(a_data.uMinMax[0], 0.0f, 1000.0f);
				float max = std::clamp(a_data.uMinMax[1], 0.0f, 1000.0f);

				ummd = max - min;
				if (ummd <= 0.0f)
				{
					flags &= ~Data::EffectShaderPulseFlags::uScale;
				}
				else
				{
					umin = min;
				}
			}

			if ((flags & Data::EffectShaderPulseFlags::vScale) == Data::EffectShaderPulseFlags::vScale)
			{
				float min = std::clamp(a_data.vMinMax[0], 0.0f, 1000.0f);
				float max = std::clamp(a_data.vMinMax[1], 0.0f, 1000.0f);

				vmmd = max - min;
				if (vmmd <= 0.0f)
				{
					flags &= ~Data::EffectShaderPulseFlags::vScale;
				}
				else
				{
					vmin = min;
				}
			}

			float min = std::clamp(a_data.range[0], 0.0f, 1.0f);
			float max = std::clamp(a_data.range[1], 0.0f, 1.0f);

			rmmd = max - min;
			if (rmmd >= 0.0f && rmmd < 1.0f)
			{
				nonstdrange = true;
				rmin        = min;
			}
		}

		void Pulse::UpdateConfigInitImpl(
			const Data::configEffectShaderFunction_t& a_data)
		{
			pos = std::clamp(a_data.initpos * PI2, 0.0f, PI2);
		}

		void Pulse::Run(
			BSEffectShaderData* a_data,
			float               a_step)
		{
			float v;

			pos = stl::fmod(pos + a_step * ftp, PI2);

			switch (function)
			{
			case Data::EffectShaderWaveform::Sine:
				v = (std::sinf(pos) + 1.0f) * 0.5f;
				break;
			case Data::EffectShaderWaveform::Cosine:
				v = (std::cosf(pos) + 1.0f) * 0.5f;
				break;
			case Data::EffectShaderWaveform::Square:
				v = pos < PI ? 1.0f : 0.0f;
				break;
			case Data::EffectShaderWaveform::Triangle:
				v = std::fabsf(pos - PI) / PI;
				break;
			case Data::EffectShaderWaveform::Sawtooth:
				v = pos / PI2;
				break;
			default:
				v = pos = 0.0f;
				break;
			}

			if (exponential)
			{
				v = (std::powf(1.0f + v, exponent) - 1.0f) / maxexpr;
			}

			if (inverse)
			{
				v = 1.0f - v;
			}

			auto f = flags;

			if ((f & Data::EffectShaderPulseFlags::uScale) == Data::EffectShaderPulseFlags::uScale)
			{
				a_data->uScale = umin + v * ummd;
			}

			if ((f & Data::EffectShaderPulseFlags::vScale) == Data::EffectShaderPulseFlags::vScale)
			{
				a_data->vScale = vmin + v * vmmd;
			}

			if (nonstdrange)
			{
				v = rmin + v * rmmd;
			}

			if ((f & Data::EffectShaderPulseFlags::kFillR) == Data::EffectShaderPulseFlags::kFillR)
			{
				a_data->fillColor.r = v;
			}

			if ((f & Data::EffectShaderPulseFlags::kFillG) == Data::EffectShaderPulseFlags::kFillG)
			{
				a_data->fillColor.g = v;
			}

			if ((f & Data::EffectShaderPulseFlags::kFillB) == Data::EffectShaderPulseFlags::kFillB)
			{
				a_data->fillColor.b = v;
			}

			if ((f & Data::EffectShaderPulseFlags::kFillA) == Data::EffectShaderPulseFlags::kFillA)
			{
				a_data->fillColor.a = v;
			}

			if ((f & Data::EffectShaderPulseFlags::kRimR) == Data::EffectShaderPulseFlags::kRimR)
			{
				a_data->rimColor.r = v;
			}

			if ((f & Data::EffectShaderPulseFlags::kRimG) == Data::EffectShaderPulseFlags::kRimG)
			{
				a_data->rimColor.g = v;
			}

			if ((f & Data::EffectShaderPulseFlags::kRimB) == Data::EffectShaderPulseFlags::kRimB)
			{
				a_data->rimColor.b = v;
			}

			if ((f & Data::EffectShaderPulseFlags::kRimA) == Data::EffectShaderPulseFlags::kRimA)
			{
				a_data->rimColor.a = v;
			}
		}
	}
}