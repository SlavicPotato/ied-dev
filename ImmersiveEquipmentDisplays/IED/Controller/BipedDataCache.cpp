#include "pch.h"

#include "BipedDataCache.h"
#include "IED/FormCommon.h"

namespace IED
{
	BipedSlotDataPtr BipedDataCache::GetOrCreate(
		Game::FormID  a_actor,
		std::uint64_t a_cc)
	{
		if (a_actor == 0x14)
		{
			m_playerEntry.data->accessed = a_cc;
			return m_playerEntry.data;
		}
		else
		{
			auto r                         = m_data.try_emplace(a_actor);
			r.first->second.data->accessed = a_cc;
			return r.first->second.data;
		}
	}

	void BipedDataCache::post_load(data_type&& a_input)
	{
		a_input.erase(0);

		if (m_maxSizeUpper &&
		    a_input.size() > m_maxSizeUpper)
		{
			stl::vector<data_type::value_type*> s;
			s.reserve(a_input.size());

			std::for_each(
				a_input.begin(),
				a_input.end(),
				[&](auto& a_v) {
					if (a_v.second.data)
					{
						s.emplace_back(std::addressof(a_v));
					}
				});

			std::sort(
				s.begin(),
				s.end(),
				[](const auto& a_lhs, const auto& a_rhs) {
					return a_lhs->second.data->accessed >
				           a_rhs->second.data->accessed;
				});

			m_data.clear();
			m_data.reserve(m_sizeTarget);

			for (auto& e : s)
			{
				if (m_data.size() >= m_sizeTarget)
				{
					break;
				}

				m_data.emplace(std::move(*e));
			}
		}
		else
		{
			m_data = std::move(a_input);
		}

		for (auto& e : m_data)
		{
			clean_entry(e.second);
		}
	}

	void BipedDataCache::clean_entry(BipedCacheEntry& a_entry)
	{
		// probably can't happen but just in case
		if (!a_entry.data)
		{
			a_entry.data = std::make_unique<BipedSlotData>();
			return;
		}

		for (auto& f : a_entry.data->data)
		{
			std::erase_if(f.forms, [](auto& a_v) { return !a_v; });

			while (!f.forms.empty() &&
			       f.forms.size() > m_maxFormsPerSlot)
			{
				f.forms.pop_back();
			}

			f.forms.shrink_to_fit();
		}
	}
}