#pragma once

#include "ConfigTransform.h"

namespace IED
{
	namespace Data
	{
		struct configExtraNodeEntry_t
		{
			stl::fixed_string parent;
			stl::fixed_string desc;
			configTransform_t xfrm_mov_m;
			configTransform_t xfrm_mov_f;
			configTransform_t xfrm_node_m;
			configTransform_t xfrm_node_f;
		};

		using configExtraNodeMap_t = std::unordered_map<stl::fixed_string, configExtraNodeEntry_t>;
	}
}