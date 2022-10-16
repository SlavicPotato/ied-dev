#pragma once

#include "ConfigConditionalVars.h"
#include "ConfigCustom.h"
#include "ConfigNodeOverride.h"
#include "ConfigSlot.h"

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
				DataVersion1 = 1,
				DataVersion2 = 2,
			};

			inline void clear()
			{
				slot       = {};
				custom     = {};
				transforms = {};
				condvars   = {};
			}

			configStoreSlot_t                  slot;
			configStoreCustom_t                custom;
			configStoreNodeOverride_t          transforms;
			configConditionalVariablesHolder_t condvars;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& slot;
				a_ar& custom;
				a_ar& transforms;

				if (a_version >= DataVersion2)
				{
					a_ar& condvars;
				}
			}
		};
	}
}

BOOST_CLASS_VERSION(
	IED::Data::configStore_t,
	IED::Data::configStore_t::Serialization::DataVersion2);
