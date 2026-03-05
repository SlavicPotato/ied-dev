#pragma once

#include "ConfigBase.h"

namespace IED
{
	namespace Data
	{
		enum class InventoryFlags : std::uint32_t
		{
			kNone = 0,

			kEquipmentMode        = 1u << 1,
			kIgnoreRaceEquipTypes = 1u << 6,
			kDisableIfEquipped    = 1u << 7,
			kSelectInvRandom      = 1u << 11,
			kCheckFav             = 1u << 10,

			//kEquipmentModeMask = kEquipmentMode,
		};

		DEFINE_ENUM_CLASS_BITWISE(InventoryFlags);

		struct configInventory_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
			};

			static constexpr auto DEFAULT_FLAGS =
				InventoryFlags::kNone;

			stl::flag<InventoryFlags> flags{ DEFAULT_FLAGS };
			configRange_t             countRange;
			configFormList_t          extraItems;

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar & flags.value;
				a_ar & countRange.min;
				a_ar & countRange.max;
				a_ar & extraItems;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar & flags.value;
				a_ar & countRange.min;
				a_ar & countRange.max;
				a_ar & extraItems;
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configInventory_t,
	::IED::Data::configInventory_t::Serialization::DataVersion1);
