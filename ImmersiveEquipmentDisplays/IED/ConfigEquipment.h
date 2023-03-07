#pragma once

#include "ConfigBaseValues.h"
#include "ConfigCommon.h"
#include "ConfigData.h"
#include "ConfigLUIDTag.h"
#include "ConfigOverride.h"
#include "ConfigVariableConditionSource.h"

#include "ConditionalVariableStorage.h"

#include "TimeOfDay.h"
#include "WeatherClassificationFlags.h"

namespace IED
{
	namespace Data
	{
		using equipmentOverride_t     = configOverride_t<configBaseValues_t>;
		using equipmentOverrideList_t = configOverrideList_t<configBaseValues_t>;
	}
}
