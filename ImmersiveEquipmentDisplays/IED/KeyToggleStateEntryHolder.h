#pragma once

#include "KeyToggleStateEntry.h"

namespace IED
{
	namespace KB
	{
		enum class KeyToggleStateEntryHolderFlags : std::uint32_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(KeyToggleStateEntryHolderFlags);

		struct KeyToggleStateEntryHolder
		{
			using container_type = stl::cache_aligned::flat_map<stl::fixed_string, KeyToggleStateEntry>;
			using state_data     = stl::boost_vector<std::pair<stl::fixed_string, bool>>;

			state_data make_state_data() const;

			stl::flag<KeyToggleStateEntryHolderFlags> flags{ KeyToggleStateEntryHolderFlags::kNone };
			container_type                            entries;
		};
	}
}