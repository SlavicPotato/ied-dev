#pragma once

#include "BipedSlotData.h"
#include "ObjectEntryBase.h"

#include "IED/ConfigData.h"

namespace IED
{
	struct ObjectEntrySlot :
		ObjectEntryBase
	{
		ObjectEntrySlot() = delete;

		using tuple_init_type = std::tuple<
			DisplaySlotCacheEntry&,
			const Data::ObjectSlot,
			const Data::ObjectSlotExtra>;

		inline constexpr ObjectEntrySlot(
			const tuple_init_type& a_init)  //
			noexcept(std::is_nothrow_default_constructible_v<ObjectEntryBase>) :
			slotState(std::get<0>(a_init)),
			slotid(std::get<1>(a_init)),
			slotidex(std::get<2>(a_init))
		{
		}

		DisplaySlotCacheEntry&      slotState;
		const Data::ObjectSlot      slotid;
		const Data::ObjectSlotExtra slotidex;
	};

	using ObjectSlotArray = std::array<
		ObjectEntrySlot,
		stl::underlying(Data::ObjectSlot::kMax)>;

}