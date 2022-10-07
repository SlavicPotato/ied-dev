#pragma once

namespace IED
{
	namespace Data
	{
		struct configAdditionalCMENode_t
		{
			stl::fixed_string node;
			stl::fixed_string desc;
		};

		using configAdditionalCMENodeList_t = stl::list<configAdditionalCMENode_t>;
	}
}