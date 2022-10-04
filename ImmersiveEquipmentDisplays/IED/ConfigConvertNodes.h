#pragma once

#include "ConfigSkeletonMatch.h"

namespace IED
{
	namespace Data
	{
		struct configConvertNodes_t
		{
			configSkeletonMatch_t match;
		};

		using configConvertNodesList_t = stl::list<configConvertNodes_t>;
	}
}