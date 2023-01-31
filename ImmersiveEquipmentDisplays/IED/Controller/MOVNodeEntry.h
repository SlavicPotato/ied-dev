#pragma once

#include "IED/Physics/SimComponent.h"
#include "IED/WeaponPlacementID.h"

namespace IED
{
	struct MOVNodeEntry
	{
		bool has_visible_geometry() const noexcept;
		bool parent_has_visible_geometry() const noexcept;

		const NiPointer<NiNode>         node;
		const NiTransform               origTransform;  // cached or read from loaded actor 3D
		const WeaponPlacementID         placementID;
		stl::smart_ptr<PHYSimComponent> simComponent;
	};
}