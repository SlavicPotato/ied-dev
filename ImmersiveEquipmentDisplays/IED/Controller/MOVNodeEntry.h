#pragma once

#include "IED/WeaponPlacementID.h"

namespace IED
{
	struct MOVNodeEntry
	{
		bool has_visible_geometry() const;

		NiPointer<NiNode> node;
		WeaponPlacementID placementID;
	};
}