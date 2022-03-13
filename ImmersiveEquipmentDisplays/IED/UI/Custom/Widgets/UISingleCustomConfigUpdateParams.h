#pragma once

#include "IED/ConfigCustom.h"

namespace IED
{
	namespace UI
	{
		struct SingleCustomConfigUpdateParams
		{
			stl::fixed_string          name;
			Data::ConfigSex            sex;
			Data::configCustomEntry_t& entry;
		};

	}
}