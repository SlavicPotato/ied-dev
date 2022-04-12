#include "pch.h"

#include "ESF_UVLinearMotion.h"

#include <ext/stl_math.h>

namespace IED
{
	namespace ESF
	{
		void UVLinearMotion::UpdateConfigImpl(
			const Data::configEffectShaderFunction_t& a_data)
		{
			constexpr auto pi2 = std::numbers::pi_v<float> * 2.0f;

			auto angle = std::clamp(a_data.angle, -pi2, pi2);
			auto speed = std::clamp(a_data.speed, 0.01f, 100.0f);

			angleSin = std::sinf(angle) * speed;
			angleCos = std::cosf(angle) * speed;
		}

		void UVLinearMotion::Run(
			BSEffectShaderData* a_data,
			float               a_step)
		{
			uvOffsetX = stl::fmod_1(uvOffsetX + angleSin * a_step);
			uvOffsetY = stl::fmod_1(uvOffsetY + angleCos * a_step);

			a_data->uOffset = uvOffsetX;
			a_data->vOffset = uvOffsetY;
		}
	}
}