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
			function =
				a_data.flags.test(Data::EffectShaderFunctionFlags::kOpt1) ?
					PulseFunction ::Cosine :
                    PulseFunction::Sine;

			flags = a_data.pulseFlags;
			speed = std::clamp(a_data.speed, 0.01f, 100.0f);
		}

		void Pulse::UpdateConfigInitImpl(
			const Data::configEffectShaderFunction_t& a_data)
		{
			mod = std::clamp(a_data.initmod * PI_X_2, 0.0f, PI_X_2);
		}

		void Pulse::Run(
			BSEffectShaderData* a_data,
			float               a_step)
		{
			mod = stl::fmod(mod + a_step * speed, PI_X_2);

			auto v = ((function == PulseFunction::Cosine ?
			               std::cosf(mod) :
                           std::sinf(mod)) +
			          1.0f) *
			         0.5f;

			auto f = flags;

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