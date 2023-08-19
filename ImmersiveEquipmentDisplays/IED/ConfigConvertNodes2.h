#pragma once

#include "ConfigSkeletonMatch.h"
#include "ConfigTransform.h"

namespace IED
{
	namespace Data
	{
		struct configConvertNodes2Entry_t
		{
			configTransform_t xfrmMOV;
			configTransform_t xfrmNode;
		};

		using configConvertNodes2EntryList_t = stl::vector<configConvertNodes2Entry_t>;

		struct configConvertNodes2_t
		{
			configSkeletonMatch_t                                        match;
			stl::flat_map<stl::fixed_string, configConvertNodes2Entry_t> entries;
		};

		using configConvertNodesList2_t = stl::list<configConvertNodes2_t>;
	}
}