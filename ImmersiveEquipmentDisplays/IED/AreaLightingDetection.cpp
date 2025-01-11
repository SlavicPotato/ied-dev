#include "pch.h"

#include "AreaLightingDetection.h"

#include "SPtrHolder.h"
#include "TimeOfDay.h"

namespace IED
{
	namespace ALD
	{
		static float s_interiorAmbientLightThreshold = 0.45f;

		void SetInteriorAmbientLightThreshold(float a_value) noexcept
		{
			s_interiorAmbientLightThreshold = std::clamp(a_value, 0.0f, 3.0f);
		}

		float GetInteriorAmbientLightThreshold() noexcept
		{
			return s_interiorAmbientLightThreshold;
		}

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

			static constexpr float get_directional_ambient_color_intensity(
				const RE::BGSDirectionalAmbientLightingColors::Directional& a_color) noexcept
			{
				const auto x =
					static_cast<std::uint32_t>(std::max(a_color.x.min.red, a_color.x.max.red)) +
					std::max(a_color.x.min.green, a_color.x.max.green) +
					std::max(a_color.x.min.blue, a_color.x.max.blue);

				const auto y =
					static_cast<std::uint32_t>(std::max(a_color.y.min.red, a_color.y.max.red)) +
					std::max(a_color.y.min.green, a_color.y.max.green) +
					std::max(a_color.y.min.blue, a_color.y.max.blue);

				const auto z =
					static_cast<std::uint32_t>(std::max(a_color.z.min.red, a_color.z.max.red)) +
					std::max(a_color.z.min.green, a_color.z.max.green) +
					std::max(a_color.z.min.blue, a_color.z.max.blue);

				return static_cast<float>(x + y + z) * ((1.0f / 255.0f) / 3.0f);
			}

			static constexpr bool is_color_dark(
				const RE::BGSDirectionalAmbientLightingColors::Directional& a_color) noexcept
			{
				return get_directional_ambient_color_intensity(a_color) < s_interiorAmbientLightThreshold;
			}

			static constexpr bool is_ambient_lighting_dark(const BGSLightingTemplate* a_templ) noexcept
			{
				return is_color_dark(a_templ->directionalAmbientLightingColors.directional);
			}

			static constexpr bool is_ambient_lighting_dark(const RE::INTERIOR_DATA* a_interior) noexcept
			{
				return a_interior ? is_color_dark(a_interior->directionalAmbientLightingColors.directional) : false;
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
			return a_sky ?
			           GetExteriorAmbientLightLevel(a_sky) < GetTorchLightLevel(a_sky) :
			           false;
		}

		float GetExteriorAmbientLightLevel(const RE::Sky* a_sky) noexcept
		{
			const auto& col = a_sky->skyColor[RE::TESWeather::ColorTypes::kAmbient];
			return col.r + col.g + col.b;
		}

		float GetTorchLightLevel(const RE::Sky* a_sky) noexcept
		{
			const bool isNightTime = a_sky ? detail::is_nighttime(a_sky) : false;
			auto&      sh          = SPtrHolder::GetSingleton();

			return isNightTime ? (sh.fTorchLightLevelNight ? *sh.fTorchLightLevelNight : 1.2f) : BASE_TORCH_LIGHT_LEVEL;
		}

		float GetTorchLightLevelRatio(const RE::Sky* a_sky) noexcept
		{
			const float torchLightLevel = GetTorchLightLevel(a_sky);
			return BASE_TORCH_LIGHT_LEVEL / torchLightLevel;
		}

		bool IsInteriorDark(const Actor* a_actor, const RE::Sky* a_sky, const TESObjectCELL* a_cell) noexcept
		{
			if (a_actor == *g_thePlayer)
			{
				if (const auto* const templ = detail::get_room_lighting_template(a_sky))
				{
					return detail::is_ambient_lighting_dark(templ);
				}
			}

			const auto* const intData = a_cell->cellData.interior;

			if (!intData || intData->lightingTemplateInheritanceFlags.test(RE::INTERIOR_DATA::Inherit::kAmbientColor))
			{
				const auto* const templ = a_cell->lightingTemplate;

				if (templ)
				{
					return detail::is_ambient_lighting_dark(templ);
				}
			}

			return detail::is_ambient_lighting_dark(intData);
		}

		float GetInteriorAmbientLightLevel(const Actor* a_actor, const RE::Sky* a_sky, const TESObjectCELL* a_cell) noexcept
		{
			if (a_actor == *g_thePlayer)
			{
				if (const auto* const templ = detail::get_room_lighting_template(a_sky))
				{
					return detail::get_directional_ambient_color_intensity(templ->directionalAmbientLightingColors.directional);
				}
			}

			const auto* const intData = a_cell->cellData.interior;

			if (!intData || intData->lightingTemplateInheritanceFlags.test(RE::INTERIOR_DATA::Inherit::kAmbientColor))
			{
				const auto* const templ = a_cell->lightingTemplate;

				if (templ)
				{
					return detail::get_directional_ambient_color_intensity(templ->directionalAmbientLightingColors.directional);
				}
			}

			return intData ? detail::get_directional_ambient_color_intensity(intData->directionalAmbientLightingColors.directional) : -1.0f;
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

		BGSLightingTemplate* GetActiveLightingTemplate(const Actor* a_actor, const RE::Sky* a_sky) noexcept
		{
			const auto cell = a_actor->GetParentCell();
			if (!cell ||
			    !cell->IsInterior() ||
			    cell->cellFlags.test(
					TESObjectCELL::Flag::kShowSky |
					TESObjectCELL::Flag::kUseSkyLighting))
			{
				return nullptr;
			}

			if (a_actor == *g_thePlayer)
			{
				if (const auto rlt = detail::get_room_lighting_template(a_sky))
				{
					return rlt;
				}
			}

			return cell->lightingTemplate;
		}

	}

}