#pragma once

namespace IED
{
	enum class WeatherClassificationFlags : std::uint32_t
	{
		//kAll = static_cast<std::underlying_type_t<WeatherClassificationFlags>>(-1),

		kNone = 0,

		kPleasant = 1u << 0,
		kCloudy   = 1u << 1,
		kRainy    = 1u << 2,
		kSnow     = 1u << 3,

		kAll = kPleasant |
		       kCloudy |
		       kRainy |
		       kSnow
	};

	DEFINE_ENUM_CLASS_BITWISE(WeatherClassificationFlags);
}