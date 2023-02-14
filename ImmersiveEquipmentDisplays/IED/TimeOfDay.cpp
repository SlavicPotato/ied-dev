#include "pch.h"

#include "TimeOfDay.h"

#include "SPtrHolder.h"

namespace IED
{
	namespace Data
	{
		ClimateTimingData GetClimateTimingData(const RE::Sky* a_sky) noexcept
		{
			if (const auto* const climate = a_sky->currentClimate)
			{
				return ClimateTimingData(
					static_cast<float>(climate->timing.sunrise.begin) * (1.0f / 6.0f),
					static_cast<float>(climate->timing.sunrise.end) * (1.0f / 6.0f),
					static_cast<float>(climate->timing.sunset.begin) * (1.0f / 6.0f),
					static_cast<float>(climate->timing.sunset.end) * (1.0f / 6.0f));
			}

			auto& sh = SPtrHolder::GetSingleton();

			if (sh.HasTimespan())
			{
				return ClimateTimingData(
					*sh.fTimeSpanSunriseStart,
					*sh.fTimeSpanSunriseEnd,
					*sh.fTimeSpanSunsetStart,
					*sh.fTimeSpanSunsetEnd);
			}

			return ClimateTimingData{ 6.0f, 10.5f, 15.5f, 20.5f };
		}

		namespace detail
		{
			static constexpr TimeOfDay get_time_of_day(const RE::Sky* a_sky, const ClimateTimingData& a_data) noexcept
			{
				const auto hour = a_sky->currentGameHour;

				if (hour < a_data.sr_begin)
				{
					return TimeOfDay::kNight;
				}
				else if (hour < a_data.sr_end)
				{
					return TimeOfDay::kSunrise;
				}
				else if (hour < a_data.ss_begin)
				{
					return TimeOfDay::kDay;
				}
				else if (hour < a_data.ss_end)
				{
					return TimeOfDay::kSunset;
				}
				else
				{
					return TimeOfDay::kNight;
				}
			}

			static constexpr bool is_daytime(const RE::Sky* a_sky, const ClimateTimingData& a_data) noexcept
			{
				const auto hour = a_sky->currentGameHour;

				if (hour < a_data.sr_begin + ((a_data.sr_end - a_data.sr_begin) / 2.0f - 0.25f))
				{
					return false;
				}
				else if (hour < a_data.ss_begin + ((a_data.ss_end - a_data.ss_begin) / 2.0f + 0.25f))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		TimeOfDay GetTimeOfDay(const RE::Sky* a_sky) noexcept
		{
			if (!a_sky)
			{
				return TimeOfDay::kNone;
			}

			const auto timing = GetClimateTimingData(a_sky);

			return detail::get_time_of_day(a_sky, timing);
		}

		bool IsDaytime(const RE::Sky* a_sky) noexcept
		{
			if (!a_sky)
			{
				return true;
			}

			const auto timing = GetClimateTimingData(a_sky);

			return detail::is_daytime(a_sky, timing);
		}

		std::pair<TimeOfDay, bool> GetTimeOfDay2(const RE::Sky* a_sky) noexcept
		{
			if (!a_sky)
			{
				return { TimeOfDay::kNone, true };
			}

			const auto timing = GetClimateTimingData(a_sky);

			return { detail::get_time_of_day(a_sky, timing), detail::is_daytime(a_sky, timing) };
		}
	}
}