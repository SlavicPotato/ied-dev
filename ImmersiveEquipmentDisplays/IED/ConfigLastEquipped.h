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
			kFallBackToSlotted     = 1u << 3,
		};

		DEFINE_ENUM_CLASS_BITWISE(LastEquippedFlags);

		struct configLastEquipped_t
		{
			friend class boost::serialization::access;

		public:
			static constexpr auto DEFAULT_FLAGS =
				LastEquippedFlags::kPrioritizeRecentSlots |
				LastEquippedFlags::kDisableIfSlotOccupied;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2,
			};

			stl::flag<LastEquippedFlags>     flags{ DEFAULT_FLAGS };
			configBipedObjectList_t          bipedSlots;
			equipmentOverrideConditionList_t filterConditions;
			Data::configObjectSlotList_t     slots;

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& flags.value;
				a_ar& bipedSlots;
				a_ar& filterConditions;
				a_ar& slots;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& bipedSlots;
				a_ar& filterConditions;

				if (a_version >= DataVersion2)
				{
					a_ar& slots;
				}
				else
				{
					Data::ObjectSlot tmp;
					a_ar&            tmp;

					slots.emplace_back(tmp);
				}
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};
	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configLastEquipped_t,
	::IED::Data::configLastEquipped_t::Serialization::DataVersion2);
