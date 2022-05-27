#pragma once

#include "BipedSlotData.h"
#include "IED/ConfigCommon.h"

namespace IED
{
	struct BipedCacheEntry
	{
		friend class boost::serialization::access;

	public:
		BipedCacheEntry() :
			data(std::make_unique<BipedSlotData>())
		{
		}

		enum Serialization : unsigned int
		{
			DataVersion1 = 1,
		};

		BipedSlotDataPtr data;

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& data;
		}
	};

	class BipedDataCache
	{
		using data_type = stl::boost_unordered_map<Data::configForm_t, BipedCacheEntry>;

		inline static constexpr std::size_t DEFAULT_SIZE_THRESHOLD_UPPER = 1200;
		inline static constexpr std::size_t DEFAULT_SIZE_TARGET          = 1000;
		inline static constexpr std::size_t MIN_SIZE                     = 400;

		friend class boost::serialization::access;

	public:
		inline BipedDataCache(std::size_t a_size)
		{
			SetMaxSize(a_size);
		}

		enum Serialization : unsigned int
		{
			DataVersion1 = 1,
		};

		BipedSlotDataPtr GetOrCreate(Game::FormID a_actor, std::uint64_t a_cc);

		[[nodiscard]] inline auto size() const noexcept
		{
			return m_data.size();
		}

		[[nodiscard]] inline constexpr auto max_size() const noexcept
		{
			return m_maxSizeUpper;
		}

		[[nodiscard]] inline void clear()
		{
			return m_data.clear();
		}

		inline constexpr void SetMaxSize(std::size_t a_newSize) noexcept
		{
			if (!a_newSize)
			{
				m_maxSizeUpper = 0;
				m_sizeTarget   = 0;
			}
			else
			{
				m_maxSizeUpper = std::max(MIN_SIZE, a_newSize);
				m_sizeTarget   = m_maxSizeUpper - 200;
			}
		}

	private:
		data_type       m_data;
		BipedCacheEntry m_playerEntry;
		std::size_t     m_maxSizeUpper{ DEFAULT_SIZE_THRESHOLD_UPPER };
		std::size_t     m_sizeTarget{ DEFAULT_SIZE_TARGET };

		template <class Archive>
		void save(Archive& a_ar, const unsigned int a_version) const
		{
			a_ar& m_playerEntry;
			a_ar& m_data;
		}

		void post_load(data_type&& a_input);
		void clean_entry(BipedCacheEntry& a_entry);

		template <class Archive>
		void load(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& m_playerEntry;
			clean_entry(m_playerEntry);

			data_type tmp;
			a_ar&     tmp;

			post_load(std::move(tmp));
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER();
	};
}

BOOST_CLASS_VERSION(
	::IED::BipedCacheEntry,
	::IED::BipedCacheEntry::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::BipedDataCache,
	::IED::BipedDataCache::Serialization::DataVersion1);
