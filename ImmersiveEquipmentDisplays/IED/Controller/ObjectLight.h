#pragma once

#include "IED/ExtraLightData.h"

namespace IED
{
	struct ObjectLight
	{
		void Cleanup(NiNode* a_root) noexcept;
		void Release() noexcept;

		NiPointer<NiPointLight>        niObject;
		NiPointer<RE::BSLight>         bsObject;
		Data::ExtraLightData           extraLightData;
	};
}