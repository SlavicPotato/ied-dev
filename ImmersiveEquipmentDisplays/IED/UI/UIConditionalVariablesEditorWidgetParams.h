#pragma once

#include "IED/ConfigConditionalVars.h"
#include "IED/ConfigInventory.h"

namespace IED
{
	namespace UI
	{
		struct ConditionalVariablesEditorWidgetParams
		{
			Data::configConditionalVariablesHolder_t& holder;
			Data::configInventory_t&                  data;
		};
	}
}