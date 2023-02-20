#pragma once

#include "Localization/Common.h"

namespace IED
{
	namespace UI
	{
		enum class UILightingTemplateInheritanceFlagsWidgetStrings : Localization::StringID
		{
			AmbientColor        = 8000,
			DirectionalColor    = 8001,
			FogColor            = 8002,
			FogNear             = 8003,
			FogFar              = 8004,
			DirectionalRotation = 8005,
			DirectionalFade     = 8006,
			ClipDistance        = 8007,
			FogPower            = 8008,
			FogMax              = 8009,
			LightFadeDistances  = 8010,
			ComboLabel          = 8011,
		};
	}
}