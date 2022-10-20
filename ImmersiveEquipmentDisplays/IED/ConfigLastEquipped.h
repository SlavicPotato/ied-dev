#pragma once

#include "ConfigBipedObjectList.h"
#include "ConfigEquipment.h"

namespace IED
{
	namespace Data
	{
		enum class LastEquippedFlags : std::uint32_t
		{
			kNone = 0,

			kPrioritizeRecentSlots = 1u << 0,
			kDisableIfSlotOccupied = 1u << 1,
			kSkipOccupiedSlots     = 1u << 2,
		};

		DEFINE_ENUM_CLASS_BITWISE(LastEquippedFlags);

		struct configLastEquipped_t
		{
			friend class boost::serialization::access;

		public:
			static inline constexpr auto DEFAULT_FLAGS =
				LastEquippedFlags::kPrioritizeRecentSlots |
				LastEquippedFlags::kDisableIfSlotOccupied;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
			};

			stl::flag<LastEquippedFlags>     flags{ DEFAULT_FLAGS };
			configBipedObjectList_t          bipedSlots;
			equipmentOverrideConditionList_t filterConditions;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& bipedSlots;
				a_ar& filterConditions;
			}
		};
	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configLastEquipped_t,
	::IED::Data::configLastEquipped_t::Serialization::DataVersion1);
