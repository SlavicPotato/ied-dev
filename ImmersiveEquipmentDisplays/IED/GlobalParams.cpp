#include "pch.h"

#include "GlobalParams.h"

#include "AreaLightingDetection.h"

namespace IED
{
	bool GlobalParams::is_exterior_dark() const noexcept
	{
		if (!isExteriorDark)
		{
			isExteriorDark.emplace(ALD::IsExteriorDark(RE::TES::GetSingleton()->sky));
		}

		return *isExteriorDark;
	}

	bool GlobalParams::is_sun_angle_less_than_60() const noexcept
	{
		return ALD::IsSunAngleLessThan(
			RE::TES::GetSingleton()->sky,
			60.0f * std::numbers::pi_v<float> / 180.0f);
	}
}