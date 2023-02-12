#include "pch.h"

#include "AreaLightingDetection.h"

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

		static float* s_fTorchLightLevelNight = nullptr;

		void InitGSPtr()
		{
			s_fTorchLightLevelNight = ISettingCollection::GetGameSettingAddr<float>("fTorchLightLevelNight");
		}

		bool IsExteriorDark(const RE::Sky* a_sky) noexcept
		{
			if (!a_sky)
			{
				return false;
			}

			bool isNightTime;

			if (const auto* const climate = a_sky->currentClimate)
			{
				isNightTime = detail::is_nighttime(a_sky, climate);
			}
			else
			{
				isNightTime = detail::is_nighttime();
			}

			const auto& col = a_sky->skyColor[3];

			return (col.r + col.g + col.b) < (isNightTime ? (s_fTorchLightLevelNight ? *s_fTorchLightLevelNight : 1.2f) : 0.6f);
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

		BGSLightingTemplate* GetRoomLightingTemplate(const RE::Sky* a_sky) noexcept
		{
			const auto* const cell = (*g_thePlayer)->parentCell;
			return cell && cell->IsInterior() ? detail::get_room_lighting_template(a_sky) : nullptr;
		}

	}

}