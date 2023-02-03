#include "pch.h"

#include "GlobalParams.h"

#include "AreaLightingDetection.h"

namespace IED
{
	bool GlobalParams::is_area_dark() const noexcept
	{
		if (!isAreaDark)
		{
			isAreaDark.emplace(IsAreaDark(RE::Sky::GetSingleton()));
		}

		return *isAreaDark;
	}
}