#pragma once

namespace IED
{
	namespace Data
	{
		enum class TimeOfDay : std::uint32_t
		{
			kNone = 0,

			kDay     = 1u << 0,
			kSunset  = 1u << 1,
			kNight   = 1u << 2,
			kSunrise = 1u << 3,

			kAll = kDay |
			       kSunset |
			       kNight |
			       kSunrise
		};

		DEFINE_ENUM_CLASS_BITWISE(TimeOfDay);

		struct ClimateTimingData
		{
			float sr_begin;
			float sr_end;
			float ss_begin;
			float ss_end;
		};

		ClimateTimingData GetClimateTimingData(const RE::Sky* a_sky) noexcept;

		TimeOfDay                  GetTimeOfDay(const RE::Sky* a_sky) noexcept;
		bool                       IsSunAboveHorizon(const RE::Sky* a_sky) noexcept;
		std::pair<TimeOfDay, bool> GetTimeOfDay2(const RE::Sky* a_sky) noexcept;

	}

}