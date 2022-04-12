#pragma once

#include "IED/Data.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Slot
		{
			TESForm* GetSlottedFormImpl(
				Game::FormID     a_actor,
				Data::ObjectSlot a_slot);
		}
	}
}