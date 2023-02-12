#pragma once

#include "TimeOfDay.h"
#include "WeatherClassificationFlags.h"

namespace IED
{
	class GlobalParams
	{
	public:
		[[nodiscard]] constexpr auto get_time_of_day() const noexcept
		{
			if (!timeOfDay)
			{
				timeOfDay.emplace(Data::GetTimeOfDay(RE::TES::GetSingleton()->sky));
			}

			return *timeOfDay;
		}

		[[nodiscard]] constexpr auto get_current_weather() const noexcept
		{
			if (!currentWeather)
			{
				const auto* const sky = RE::TES::GetSingleton()->sky;
				currentWeather.emplace(sky ? sky->GetCurrentWeatherHalfPct() : nullptr);
			}

			return *currentWeather;
		}

		[[nodiscard]] bool is_exterior_dark() const noexcept;
		[[nodiscard]] bool is_sun_angle_less_than_60() const noexcept;

	private:
		mutable std::optional<Data::TimeOfDay>                       timeOfDay;
		mutable std::optional<RE::TESWeather*>                       currentWeather;
		mutable std::optional<bool>                                  isExteriorDark;
		mutable std::optional<bool>                                  isSunAngleLessThan60;
	};
}