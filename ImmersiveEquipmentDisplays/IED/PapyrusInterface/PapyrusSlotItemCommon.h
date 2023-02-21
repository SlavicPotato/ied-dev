#pragma once

#include "IED/Data.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Slot
		{

			[[nodiscard]] constexpr Data::ObjectSlot GetSlot(std::int32_t a_slot) noexcept
			{
				if (a_slot < 0 || a_slot > stl::underlying(Data::ObjectSlot::kMax))
				{
					return Data::ObjectSlot::kNone;
				}
				else
				{
					return static_cast<Data::ObjectSlot>(a_slot);
				}
			}

		}
	}
}