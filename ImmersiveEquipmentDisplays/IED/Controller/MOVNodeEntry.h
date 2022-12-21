#pragma once

#include "IED/Physics/SimComponent.h"
#include "IED/WeaponPlacementID.h"

namespace IED
{

	struct MOVNodeEntry
	{
		bool has_visible_geometry() const noexcept;
		bool parent_has_visible_geometry() const noexcept;

		NiPointer<NiNode>                node;
		NiTransform                      origTransform;  // cached or read from loaded actor 3D
		WeaponPlacementID                placementID;
		std::shared_ptr<PHYSimComponent> simComponent;
	};
}