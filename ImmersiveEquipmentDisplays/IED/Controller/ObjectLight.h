#pragma once

namespace IED
{
	struct ObjectLight
	{
		NiPointer<NiPointLight> niObject;
		NiPointer<RE::BSLight>  bsObject;
	};
}