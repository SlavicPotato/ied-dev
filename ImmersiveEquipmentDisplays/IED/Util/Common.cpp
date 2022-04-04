#include "pch.h"

#include "Common.h"

namespace IED
{
	namespace Util
	{
		namespace Common
		{
			bool IsREFRValid(const TESObjectREFR* a_refr)
			{
				return a_refr &&
				       a_refr->formID != 0 &&
				       a_refr->loadedState &&
				       !a_refr->IsDeleted();
			}
		}
	}
}