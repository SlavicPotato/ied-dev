#pragma once

#include "ObjectEntryBase.h"

#include "IED/ActorState.h"
#include "IED/ConfigData.h"

namespace IED
{

	struct ObjectEntrySlot :
		ObjectEntryBase
	{
		Data::actorStateSlotEntry_t slotState;
		Data::ObjectSlot            slotid{ Data::ObjectSlot::kMax };
		Data::ObjectSlotExtra       slotidex{ Data::ObjectSlotExtra::kNone };
	};

	using ObjectSlotArray = std::array<
		ObjectEntrySlot,
		stl::underlying(Data::ObjectSlot::kMax)>;

}