#include "pch.h"

#include "BipedSlotData.h"

namespace IED
{
	void DisplaySlotCacheEntry::insert_last_slotted(
		Game::FormID  a_form,
		std::uint32_t a_limit) noexcept
	{
		auto& data = lastSlotted;

		if (data.empty() || data.front() != a_form)
		{
			auto it = std::find(data.begin(), data.end(), a_form);
			if (it != data.end())
			{
				data.erase(it);
			}

			data.emplace(data.begin(), a_form);

			if (data.size() > a_limit)
			{
				data.pop_back();
			}
		}
	}
}