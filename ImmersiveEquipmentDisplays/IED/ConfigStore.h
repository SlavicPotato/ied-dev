#pragma once

#include "ConfigConditionalVars.h"
#include "ConfigCustom.h"
#include "ConfigNodeOverride.h"
#include "ConfigSlot.h"
#include "OM/ConfigStoreOutfit.h"

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
				DataVersion3 = 3,
			};

			inline void clear()
			{
				slot       = {};
				custom     = {};
				transforms = {};
				condvars   = {};
				outfit     = {};
			}

			configStoreSlot_t                  slot;
			configStoreCustom_t                custom;
			configStoreNodeOverride_t          transforms;
			configConditionalVariablesHolder_t condvars;
			OM::configStoreOutfit_t            outfit;

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

					if (a_version >= DataVersion3)
					{
						a_ar& outfit;
					}
				}
			}
		};
	}
}

BOOST_CLASS_VERSION(
	IED::Data::configStore_t,
	IED::Data::configStore_t::Serialization::DataVersion3);
