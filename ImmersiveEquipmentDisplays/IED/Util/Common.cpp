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
				if (a_refr == nullptr ||
				    a_refr->loadedState == nullptr ||
				    (a_refr->flags & TESForm::kFlagIsDeleted) == TESForm::kFlagIsDeleted)
				{
					return false;
				}
				return true;
			}

		}
	}
}