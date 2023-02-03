#pragma once

#include "IED/ConfigCommon.h"

namespace IED
{
	struct BipedSlotCacheEntry
	{
	public:
		friend class boost::serialization::access;

	public:
		static constexpr std::size_t DEFAULT_MAX_FORMS = 16;

		enum Serialization : unsigned int
		{
			DataVersion1 = 1,
		};

		stl::boost_vector<Data::configFormZeroMissing_t> forms;
		std::uint64_t                                    seen{ 0 };
		bool                                             occupied{ false };

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& forms;
			a_ar& seen;
		}
	};

	struct DisplaySlotCacheEntry
	{
	public:
		friend class boost::serialization::access;

	public:
		enum Serialization : unsigned int
		{
			DataVersion1 = 1,
			DataVersion2 = 2,
		};

		void insert_last_slotted(
			Game::FormID  a_form,
			std::uint32_t a_limit) noexcept;

		Data::configFormZeroMissing_t                    lastEquipped;
		stl::boost_vector<Data::configFormZeroMissing_t> lastSlotted;
		std::uint64_t                                    lastSeenEquipped{ 0 };

	private:
		template <class Archive>
		void save(Archive& a_ar, const unsigned int a_version) const
		{
			a_ar& lastEquipped;
			a_ar& lastSlotted;
			a_ar& lastSeenEquipped;
		}

		template <class Archive>
		void load(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& lastEquipped;

			if (a_version >= DataVersion2)
			{
				a_ar& lastSlotted;
			}
			else
			{
				Data::configFormZeroMissing_t tmp;
				a_ar&                         tmp;

				if (tmp)
				{
					lastSlotted.emplace_back(tmp);
				}
			}

			a_ar& lastSeenEquipped;
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER();
	};

	struct BipedSlotData
	{
	public:
		friend class boost::serialization::access;

	public:
		enum Serialization : unsigned int
		{
			DataVersion1 = 1,
			DataVersion2 = 2,
		};

		using biped_data_type = std::array<
			BipedSlotCacheEntry,
			stl::underlying(BIPED_OBJECT::kTotal)>;

		using display_slot_data_type = std::array<
			DisplaySlotCacheEntry,
			stl::underlying(Data::ObjectSlot::kMax)>;

		[[nodiscard]] constexpr auto& get(BIPED_OBJECT a_object) noexcept
		{
			assert(a_object < BIPED_OBJECT::kTotal);
			return biped[stl::underlying(a_object)];
		}

		[[nodiscard]] constexpr auto& get(BIPED_OBJECT a_object) const noexcept
		{
			assert(a_object < BIPED_OBJECT::kTotal);
			return biped[stl::underlying(a_object)];
		}

		[[nodiscard]] constexpr auto& get(Data::ObjectSlot a_slot) noexcept
		{
			assert(a_slot < Data::ObjectSlot::kMax);
			return displays[stl::underlying(a_slot)];
		}

		[[nodiscard]] constexpr auto& get(Data::ObjectSlot a_slot) const noexcept
		{
			assert(a_slot < Data::ObjectSlot::kMax);
			return displays[stl::underlying(a_slot)];
		}

		std::uint64_t          accessed{ 0 };
		biped_data_type        biped;
		display_slot_data_type displays;

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& accessed;
			a_ar& biped;

			if (a_version >= DataVersion2)
			{
				a_ar& displays;
			}
		}
	};

	using BipedSlotDataPtr = std::shared_ptr<BipedSlotData>;
}

BOOST_CLASS_VERSION(
	::IED::BipedSlotCacheEntry,
	::IED::BipedSlotCacheEntry::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::DisplaySlotCacheEntry,
	::IED::DisplaySlotCacheEntry::Serialization::DataVersion2);

BOOST_CLASS_VERSION(
	::IED::BipedSlotData,
	::IED::BipedSlotData::Serialization::DataVersion2);
