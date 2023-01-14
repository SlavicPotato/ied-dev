#pragma once

namespace IED
{
	namespace Util
	{
		namespace Common
		{
			[[nodiscard]] constexpr bool IsREFRValid(const TESObjectREFR* a_refr) noexcept
			{
				return a_refr &&
				       a_refr->formID != 0 &&
				       a_refr->loadedState &&
				       (a_refr->flags & (TESObjectREFR::kFlag_Disabled | TESObjectREFR::kFlagTESObjectREFR_Deleted)) == 0;
			}
		}
	}
}