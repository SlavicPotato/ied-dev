#include "pch.h"

#include "GeneralInfo.h"

#include "AreaLightingDetection.h"

namespace IED
{
	void generalInfoEntry_t::Update()
	{
		*this = {};

		if (const auto* const sky = RE::TES::GetSingleton()->sky)
		{
			timeOfDay = Data::GetTimeOfDay2(sky);

			if (const auto* const weather = sky->currentWeather)
			{
				currentWeather = weather->formID;
			}

			if (const auto* const weather = sky->lastWeather)
			{
				lastWeather = weather->formID;
			}

			weatherTrans = sky->currentWeatherPct;

			if (const auto roomRef = sky->currentRoom.get_ptr())
			{
				room = roomRef->formID;

				if (const auto extraRoomRef = roomRef->extraData.Get<ExtraRoomRefData>())
				{
					if (const auto* const data = extraRoomRef->data)
					{
						if (const auto* const lt = data->lightingTemplate)
						{
							roomLightingTemplate = lt->formID;
						}
					}
				}
			}

			sunAngle                  = ALD::GetSunAngle(sky);
			exteriorAmbientLightLevel = ALD::GetExteriorAmbientLightLevel(sky);
			torchLightLevel           = ALD::GetTorchLightLevel(sky);
			gameHour                  = sky->currentGameHour;
		}

		if (const auto* const calendar = RE::Calendar::GetSingleton())
		{
			day        = calendar->GetDay();
			dayOfWeek  = calendar->GetDayOfWeek();
			month      = calendar->GetMonth();
			year       = calendar->GetYear();
			daysPassed = calendar->GetDaysPassed();
			timescale  = calendar->GetTimescale();
		}

		frameTimer     = *Game::g_frameTimer;
		frameTimerSlow = *Game::g_frameTimerSlow;

		if (auto s = Game::Unk2f6b948::GetSingleton())
		{
			globalTimeMultiplier = *Game::g_currentGlobalTimeMultiplier;
			worldTimeMultiplier  = s->GetWorldTimeMultiplier();
			playerTimeMultiplier = s->GetPlayerTimeMultiplier();
		}

		gameRuntime = *Game::g_gameRuntimeMS;
	}
}