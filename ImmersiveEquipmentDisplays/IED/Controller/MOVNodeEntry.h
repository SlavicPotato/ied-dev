#pragma once

#include "IED/WeaponPlacementID.h"

namespace IED
{
	struct MOVNodeEntry
	{
		NiPointer<NiNode> node;
		WeaponPlacementID placementID;
	};
}