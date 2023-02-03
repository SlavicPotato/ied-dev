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
				timeOfDay.emplace(Data::GetTimeOfDay(RE::Sky::GetSingleton()));
			}

			return *timeOfDay;
		}

		[[nodiscard]] constexpr auto get_current_weather() const noexcept
		{
			if (!currentWeather)
			{
				const auto* const sky = RE::Sky::GetSingleton();
				currentWeather.emplace(sky->GetCurrentWeatherHalfPct());
			}

			return *currentWeather;
		}

		[[nodiscard]] bool is_area_dark() const noexcept;

	private:
		mutable std::optional<Data::TimeOfDay>                       timeOfDay;
		mutable std::optional<RE::TESWeather*>                       currentWeather;
		mutable std::optional<bool>                                  isAreaDark;
	};
}