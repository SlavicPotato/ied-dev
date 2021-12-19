#pragma once

#include "ConfigOverrideCustom.h"
#include "ConfigOverrideSlot.h"
#include "ConfigOverrideNodeOverride.h"

namespace IED
{
	namespace Data
	{
		struct configStore_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			inline void clear()
			{
				slot = {};
				custom = {};
				transforms = {};
			}

			configStoreSlot_t slot;
			configStoreCustom_t custom;
			configStoreNodeOverride_t transforms;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& slot;
				ar& custom;
				ar& transforms;
			}
		};
	}
}

BOOST_CLASS_VERSION(
	IED::Data::configStore_t,
	IED::Data::configStore_t::Serialization::DataVersion1);
