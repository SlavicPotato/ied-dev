#include "pch.h"

#include "BipedDataCache.h"
#include "IED/FormCommon.h"

namespace IED
{
	const BipedSlotDataPtr& BipedDataCache::GetOrCreate(
		Game::FormID  a_actor,
		std::uint64_t a_cc) noexcept
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
				[&](auto& a_v) [[msvc::forceinline]] {
					if (a_v.second.data)
					{
						s.emplace_back(std::addressof(a_v));
					}
				});

			std::sort(
				s.begin(),
				s.end(),
				[](const auto& a_lhs, const auto& a_rhs) [[msvc::forceinline]] {
					return a_lhs->second.data->accessed >
				           a_rhs->second.data->accessed;
				});

			m_data.clear();

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

	namespace detail
	{
		SKMP_FORCEINLINE constexpr void clean_form_list(
			stl::boost_vector<Data::configFormZeroMissing_t>& a_data,
			std::uint32_t                                     a_maxCount)
		{
			std::erase_if(
				a_data,
				[](auto& a_v) [[msvc::forceinline]] {
					return !a_v;
				});

			while (!a_data.empty() &&
			       a_data.size() > a_maxCount)
			{
				a_data.pop_back();
			}

			a_data.shrink_to_fit();
		}
	}

	void BipedDataCache::clean_entry(BipedCacheEntry& a_entry)
	{
		// probably can't happen but just in case
		if (!a_entry.data)
		{
			a_entry.data = std::make_shared<BipedSlotData>();
			return;
		}

		for (auto& e : a_entry.data->biped)
		{
			detail::clean_form_list(e.forms, m_maxFormsPerSlot);
		}

		for (auto& e : a_entry.data->displays)
		{
			detail::clean_form_list(e.lastSlotted, m_maxFormsPerSlot);
		}
	}
}