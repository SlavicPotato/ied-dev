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

			kPrioritizeRecentBipedSlots    = 1u << 0,
			kDisableIfSlotOccupied         = 1u << 1,
			kSkipOccupiedBipedSlots        = 1u << 2,
			kFallBackToSlotted             = 1u << 3,
			kPrioritizeRecentDisplaySlots  = 1u << 4,
			kSkipOccupiedDisplaySlots      = 1u << 5,
			kFallBackToAcquired            = 1u << 6,
			kPrioritizeRecentAcquiredTypes = 1u << 7,
		};

		DEFINE_ENUM_CLASS_BITWISE(LastEquippedFlags);

		struct configLastEquipped_t
		{
			friend class boost::serialization::access;

		public:
			static constexpr auto DEFAULT_FLAGS =
				LastEquippedFlags::kPrioritizeRecentBipedSlots |
				LastEquippedFlags::kPrioritizeRecentDisplaySlots |
				LastEquippedFlags::kDisableIfSlotOccupied |
				LastEquippedFlags::kPrioritizeRecentAcquiredTypes;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2,
				DataVersion3 = 3,
			};

			stl::flag<LastEquippedFlags>    flags{ DEFAULT_FLAGS };
			configBipedObjectList_t         bipedSlots;
			equipmentOverrideConditionSet_t filterConditions;
			configObjectSlotList_t          slots;
			configObjectTypeExtraList_t     acqList;

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& flags.value;
				a_ar& bipedSlots;
				a_ar& filterConditions.list;
				a_ar& slots;
				a_ar& acqList;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& bipedSlots;
				a_ar& filterConditions.list;

				if (a_version >= DataVersion2)
				{
					a_ar& slots;

					if (a_version >= DataVersion3)
					{
						a_ar& acqList;
					}
				}
				else
				{
					ObjectSlot tmp;
					a_ar&      tmp;

					if (tmp < ObjectSlot::kMax)
					{
						slots.emplace_back(tmp);
					}
				}
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};
	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configLastEquipped_t,
	::IED::Data::configLastEquipped_t::Serialization::DataVersion3);
