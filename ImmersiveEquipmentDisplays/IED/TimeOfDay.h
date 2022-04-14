#pragma once

#include <ext/Sky.h>
#include <ext/TESClimate.h>

namespace IED
{
	namespace Data
	{
		enum class TimeOfDay : std::uint32_t
		{
			kNone = 0,

			kDay     = 1,
			kSunset  = 2,
			kNight   = 3,
			kSunrise = 4
		};

		inline static constexpr TimeOfDay GetTimeOfDay(RE::Sky* a_sky) noexcept
		{
			if (!a_sky)
			{
				return TimeOfDay::kNone;
			}

			auto climate = a_sky->currentClimate;
			if (!climate)
			{
				return TimeOfDay::kNone;
			}

			auto hour = a_sky->currentGameHour;

			if (hour < static_cast<float>(climate->timing.sunrise.begin) / 6.0f)
			{
				return TimeOfDay::kNight;
			}
			else if (hour < static_cast<float>(climate->timing.sunrise.end) / 6.0f)
			{
				return TimeOfDay::kSunrise;
			}
			else if (hour < static_cast<float>(climate->timing.sunset.begin) / 6.0f)
			{
				return TimeOfDay::kDay;
			}
			else if (hour < static_cast<float>(climate->timing.sunset.end) / 6.0f)
			{
				return TimeOfDay::kSunset;
			}
			else
			{
				return TimeOfDay::kNight;
			}
		}

	}

}