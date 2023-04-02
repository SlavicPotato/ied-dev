#pragma once

#include "KeyToggleStateEntry.h"

namespace IED
{
	namespace KB
	{
		struct KeyToggleStateEntryHolder
		{
			using container_type = stl::cache_aligned::flat_map<stl::fixed_string, KeyToggleStateEntry>;
			using state_data     = stl::boost_vector<std::pair<stl::fixed_string, bool>>;
			using state_data2    = stl::boost_vector<std::pair<stl::fixed_string, std::uint32_t>>;

			state_data2 make_state_data() const;

			container_type                            entries;
		};
	}
}