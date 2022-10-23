#pragma once

#include "IED/ConfigCommon.h"

namespace IED
{
	struct BipedSlotEntry
	{
	public:
		friend class boost::serialization::access;

	public:
		inline static constexpr std::size_t DEFAULT_MAX_FORMS = 16;

		enum Serialization : unsigned int
		{
			DataVersion1 = 1,
		};

		stl::boost_vector<Data::configFormZeroMissing_t> forms;
		bool                                             occupied{ false };
		std::uint64_t                                    seen{ 0 };

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& forms;
			a_ar& seen;
		}
	};

	struct BipedSlotData
	{
	public:
		friend class boost::serialization::access;

	public:
		enum Serialization : unsigned int
		{
			DataVersion1 = 1,
		};

		using data_type = std::array<
			BipedSlotEntry,
			stl::underlying(BIPED_OBJECT::kTotal)>;

		[[nodiscard]] inline constexpr auto& get(BIPED_OBJECT a_object) const noexcept
		{
			assert(a_object < BIPED_OBJECT::kTotal);
			return data[stl::underlying(a_object)];
		}

		[[nodiscard]] inline constexpr auto& get(BIPED_OBJECT a_object) noexcept
		{
			assert(a_object < BIPED_OBJECT::kTotal);
			return data[stl::underlying(a_object)];
		}

		std::uint64_t accessed{ 0 };
		data_type     data;

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& accessed;
			a_ar& data;
		}
	};

	using BipedSlotDataPtr = std::shared_ptr<BipedSlotData>;
}

BOOST_CLASS_VERSION(
	::IED::BipedSlotEntry,
	::IED::BipedSlotEntry::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::BipedSlotData,
	::IED::BipedSlotData::Serialization::DataVersion1);
