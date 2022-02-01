#pragma once

#include "../UICommon.h"

#include "IED/Controller/NodeOverrideData.h"

namespace IED
{
	namespace UI
	{
		class UISimpleNodeList
		{
		public:
			static bool DrawNodeList(
				const char* a_label,
				stl::fixed_string& a_current,
				const NodeOverrideData::nodeList_t& a_data);
		};
	}
}