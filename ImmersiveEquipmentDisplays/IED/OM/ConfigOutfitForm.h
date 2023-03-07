#pragma once

#include "IED/ConfigCommon.h"

namespace IED
{
	namespace Data
	{
		namespace OM
		{
			struct configOutfitForm_t
			{
			public:
				stl::vector<Data::configFormZeroMissing_t> items;
			};

			using configOutfitFormList_t = stl::flat_map<stl::fixed_string, configOutfitForm_t>;
		}
	}
}