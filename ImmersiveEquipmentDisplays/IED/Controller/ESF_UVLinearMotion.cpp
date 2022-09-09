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
			auto angle = std::clamp(a_data.angle, -PI2, PI2);
			auto speed = std::clamp(a_data.speed, 0.01f, 100.0f);

			DirectX::XMScalarSinCos(
				std::addressof(angleSin),
				std::addressof(angleCos),
				angle);

			angleSin *= speed;
			angleCos *= speed;
		}

		void UVLinearMotion::Run(
			BSEffectShaderData* a_data,
			float               a_step)
		{
			a_data->uOffset = uOffset = stl::fmod_1(uOffset + angleSin * a_step);
			a_data->vOffset = vOffset = stl::fmod_1(vOffset + angleCos * a_step);
		}
	}
}