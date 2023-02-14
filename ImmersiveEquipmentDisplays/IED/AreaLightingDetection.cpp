#include "pch.h"

#include "AreaLightingDetection.h"

#include "SPtrHolder.h"
#include "TimeOfDay.h"

namespace IED
{
	namespace ALD
	{
		namespace detail
		{
			static BGSLightingTemplate* get_room_lighting_template(
				const RE::Sky* a_sky) noexcept
			{
				if (a_sky)
				{
					if (a_sky->currentRoom)
					{
						NiPointer<TESObjectREFR> ref;
						if (a_sky->currentRoom.Lookup(ref))
						{
							if (const auto extraRoomRef = ref->extraData.Get<ExtraRoomRefData>())
							{
								if (const auto data = extraRoomRef->data)
								{
									return data->lightingTemplate;
								}
							}
						}
					}
				}

				return nullptr;
			}

			static bool is_nighttime(const RE::Sky* a_sky) noexcept
			{
				const auto timing = Data::GetClimateTimingData(a_sky);

				const auto hour = a_sky->currentGameHour;

				return hour > timing.ss_end - (timing.ss_end - timing.ss_begin) * 0.125f ||
				       hour < timing.sr_begin + (timing.sr_end - timing.sr_begin) * 0.125f;
			}

			static constexpr bool is_nighttime() noexcept
			{
				const auto* const calendar = RE::Calendar::GetSingleton();

				if (!calendar)
				{
					return false;
				}

				if (const auto* const gameHour = calendar->gameHour)
				{
					return gameHour->value > 20.0f || gameHour->value < 6.0f;
				}
				else
				{
					return false;
				}
			}

			static constexpr bool is_color_dark(const RE::Color& a_color) noexcept
			{
				return static_cast<float>(static_cast<std::uint32_t>(a_color.red) + a_color.green + a_color.blue) < 0.35f * 255.0f;
			}

			static constexpr bool is_ambient_lighting_dark(const BGSLightingTemplate* a_templ) noexcept
			{
				return is_color_dark(a_templ->data.ambient);
			}

			static constexpr bool is_ambient_lighting_dark(const RE::INTERIOR_DATA* a_interior) noexcept
			{
				return a_interior ? is_color_dark(a_interior->ambient) : false;
			}

			static bool get_sun_angle(const RE::Sky* a_sky, float& a_out) noexcept
			{
				NiPoint3 pos(NiPoint3::noinit_arg_t{});

				if (a_sky->GetSunPosition(pos))
				{
					a_out = std::atan2f(pos.x, pos.z);

					return true;
				}
				else
				{
					return false;
				}
			}
		}

		bool IsExteriorDark(const RE::Sky* a_sky) noexcept
		{
			if (!a_sky)
			{
				return false;
			}

			const bool isNightTime = detail::is_nighttime(a_sky);

			const auto& col = a_sky->skyColor[3];

			return (col.r + col.g + col.b) < (isNightTime ? (SPtrHolder::GetSingleton().fTorchLightLevelNight ? *SPtrHolder::GetSingleton().fTorchLightLevelNight : 1.2f) : 0.6f);
		}

		bool IsActorInDarkInterior(const Actor* a_actor, const RE::Sky* a_sky) noexcept
		{
			const auto* const cell = a_actor->parentCell;

			if (!cell || !cell->IsInterior())
			{
				return false;
			}

			if (a_actor == *g_thePlayer)
			{
				if (const auto* const templ = detail::get_room_lighting_template(a_sky))
				{
					return detail::is_ambient_lighting_dark(templ);
				}
			}

			const auto* const intData = cell->cellData.interior;

			if (!intData || intData->lightingTemplateInheritanceFlags.test(RE::INTERIOR_DATA::Inherit::kAmbientColor))
			{
				const auto* const templ = cell->lightingTemplate;

				if (templ)
				{
					return detail::is_ambient_lighting_dark(templ);
				}
			}

			return detail::is_ambient_lighting_dark(intData);
		}

		bool IsSunAngleLessThan(const RE::Sky* a_sky, float a_angle) noexcept
		{
			if (a_sky)
			{
				float currentAngle;

				if (detail::get_sun_angle(a_sky, currentAngle))
				{
					return std::fabsf(currentAngle) < a_angle;
				}
			}

			return false;
		}

		float GetRoundedSunAngle(const RE::Sky* a_sky, float a_nearest) noexcept
		{
			assert(a_nearest != 0.0f);

			if (a_sky)
			{
				float currentAngle;

				if (detail::get_sun_angle(a_sky, currentAngle))
				{
					return std::roundf(currentAngle / a_nearest) * a_nearest;
				}
			}

			return 0.0f;
		}

		float GetSunAngle(const RE::Sky* a_sky) noexcept
		{
			if (a_sky)
			{
				float currentAngle;

				if (detail::get_sun_angle(a_sky, currentAngle))
				{
					return currentAngle;
				}
			}

			return 0.0f;
		}

		BGSLightingTemplate* GetRoomLightingTemplate(const RE::Sky* a_sky) noexcept
		{
			const auto* const cell = (*g_thePlayer)->parentCell;
			return cell && cell->IsInterior() ? detail::get_room_lighting_template(a_sky) : nullptr;
		}

	}

}