#pragma once

#include "ConfigOutfit.h"

#include "IED/ConfigOverride.h"

namespace IED
{
	namespace Data
	{
		namespace OM
		{
			using outfitOverride_t     = configOverride_t<configOutfit_t>;
			using outfitOverrideList_t = configOverrideList_t<configOutfit_t>;
		}
	}
}