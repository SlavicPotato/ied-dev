#pragma once

#include "IED/Controller/BipedSlotData.h"
#include "IED/Inventory.h"
#include "IED/NodeOverrideParams.h"

namespace IED
{
	struct BipedSlotCacheEntry;
	struct ObjectEntrySlot;

	struct ActorTempData
	{
		SKMP_ALIGNED_REDEFINE_NEW_PREF(stl::L1_CACHE_LINE_SIZE);

		Data::CollectorData::container_type       idt;
		Data::CollectorData::eq_container_type    eqt;
		SlotResults                               sr;
		nodeOverrideParams_t::item_container_type nc;
		UseCountContainer                         uc;
		stl::cache_aligned::vector<const void*>   pc;
		stl::cache_aligned::vector<Game::FormID>  fl;
	};
}