#pragma once

#include "TimeOfDay.h"

namespace IED
{
	struct generalInfoEntry_t
	{
		void Update();

		std::pair<Data::TimeOfDay, bool> timeOfDay;
		Game::FormID                     currentWeather;
		Game::FormID                     lastWeather;
		float                            weatherTrans{ 0.0f };
		Game::FormID                     room;
		Game::FormID                     roomLightingTemplate;
		float                            sunAngle{ 0.0f };
		float                            exteriorAmbientLightLevel{ 0.0f };
		float                            torchLightLevel{ 0.0f };
		float                            gameHour{ 0.0f };

		float             day{ 0.0f };
		RE::Calendar::Day dayOfWeek{ RE::Calendar::Day::kSundas };
		std::uint32_t     month;
		std::uint32_t     year;
		float             daysPassed{ 0.0f };
		float             timescale{ 0.0f };

		float frameTimer{ 0.0f };
		float frameTimerSlow{ 0.0f };
		float globalTimeMultiplier{ 0.0f };
		float worldTimeMultiplier{ 0.0f };
		float playerTimeMultiplier{ 0.0f };
	};
}