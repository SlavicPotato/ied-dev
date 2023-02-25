#pragma once

#include "IED/ExtraLightData.h"

namespace IED
{
	struct ObjectLight
	{
		NiPointer<NiPointLight>        niObject;
		NiPointer<RE::BSLight>         bsObject;
		Data::ExtraLightData           extraLightData;
	};
}