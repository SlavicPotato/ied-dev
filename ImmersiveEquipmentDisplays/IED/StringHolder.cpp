#include "pch.h"

#include "StringHolder.h"

#include "Data.h"

namespace IED
{
	StringHolder StringHolder::m_Instance;

	StringHolder::StringHolder()
	{
		using enum_type = std::underlying_type_t<Data::ObjectSlot>;

		for (enum_type i = 0; i < std::size(slotNames); i++)
		{
			auto slotName = Data::GetSlotName(static_cast<Data::ObjectSlot>(i));
			ASSERT(slotName);
			slotNames[i] = slotName;
		}
	}
}  // namespace IED