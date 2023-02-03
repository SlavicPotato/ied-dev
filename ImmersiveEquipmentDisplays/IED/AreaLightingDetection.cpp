#include "pch.h"

#include "AreaLightingDetection.h"

namespace IED
{
	namespace detail
	{
		static auto get_int_lighting_template(
			const PlayerCharacter* a_player,
			const RE::Sky*         a_sky) noexcept
		{
			// binding conditions on npcs to this sucks, needs a better solution
			if (a_sky->currentRoom)
			{
				NiPointer<TESObjectREFR> ref;
				if (a_sky->currentRoom.Lookup(ref))
				{
					if (const auto extraRoomRef = ref->extraData.Get<ExtraRoomRefData>())
					{
						if (const auto data = extraRoomRef->data; data && data->lightingTemplate)
						{
							return data->lightingTemplate;
						}
					}
				}
			}

			return a_player->parentCell->lightingTemplate;
		}

		static constexpr bool is_nighttime(const RE::Sky* a_sky, const RE::TESClimate* a_climate) noexcept
		{
			const auto hour = a_sky->currentGameHour;

			const auto srBegin = static_cast<float>(a_climate->timing.sunrise.begin) * (1.0f / 6.0f);
			const auto srEnd   = static_cast<float>(a_climate->timing.sunrise.end) * (1.0f / 6.0f);

			const auto ssBegin = static_cast<float>(a_climate->timing.sunset.begin) * (1.0f / 6.0f);
			const auto ssEnd   = static_cast<float>(a_climate->timing.sunset.end) * (1.0f / 6.0f);

			return hour > ssEnd - (ssEnd - ssBegin) * 0.125f ||
			       hour < srBegin + (srEnd - srBegin) * 0.125f;
		}

		static constexpr bool is_nighttime() noexcept
		{
			const auto calendar = RE::Calendar::GetSingleton();

			if (!calendar)
			{
				return false;
			}

			if (auto gameHour = calendar->gameHour)
			{
				return gameHour->value > 20.0f || gameHour->value < 6.0f;
			}
			else
			{
				return false;
			}
		}
	}

	bool IsAreaDark(const RE::Sky* a_sky) noexcept
	{
		const auto player = *g_thePlayer;

		if (player->IsInInterior())
		{
			const auto* const templ = detail::get_int_lighting_template(player, a_sky);

			if (!templ)
			{
				return false;
			}

			const auto& col = templ->data.ambient;

			return static_cast<float>(static_cast<std::uint32_t>(col.red) + col.green + col.blue) < 0.35f * 255.0f;
		}
		else
		{
			bool isNightTime;

			if (auto climate = a_sky->currentClimate)
			{
				isNightTime = detail::is_nighttime(a_sky, climate);
			}
			else
			{
				isNightTime = detail::is_nighttime();
			}

			const auto& col = a_sky->skyColor[3];

			return (col.r + col.g + col.b) < (isNightTime ? 1.2f : 0.6f);
		}
	}
}