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

	bool GlobalParams::is_daytime() const noexcept
	{
		if (!isDaytime)
		{
			isDaytime.emplace(Data::IsSunAboveHorizon(RE::TES::GetSingleton()->sky));
		}

		return *isDaytime;
	}

	float GlobalParams::get_sun_angle() const noexcept
	{
		if (!sunAngle)
		{
			sunAngle.emplace(ALD::GetRoundedSunAngle(RE::TES::GetSingleton()->sky));
		}

		return *sunAngle;
	}
}