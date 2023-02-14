#include "pch.h"

#include "SPtrHolder.h"

namespace IED
{
	SPtrHolder SPtrHolder::m_Instance;

	void SPtrHolder::Initialize()
	{
		m_Instance.InitializeImpl();
	}

	void SPtrHolder::InitializeImpl()
	{
		fTorchLightLevelNight = ISettingCollection::GetGameSettingAddr<float>("fTorchLightLevelNight");

		fTimeSpanSunriseStart = ISettingCollection::GetGameSettingAddr<float>("fTimeSpanSunriseStart");
		fTimeSpanSunriseEnd   = ISettingCollection::GetGameSettingAddr<float>("fTimeSpanSunriseEnd");
		fTimeSpanSunsetStart  = ISettingCollection::GetGameSettingAddr<float>("fTimeSpanSunsetStart");
		fTimeSpanSunsetEnd    = ISettingCollection::GetGameSettingAddr<float>("fTimeSpanSunsetEnd");

		m_hasTimespan = fTimeSpanSunriseStart && fTimeSpanSunriseEnd && fTimeSpanSunsetStart && fTimeSpanSunsetEnd;
	}

}