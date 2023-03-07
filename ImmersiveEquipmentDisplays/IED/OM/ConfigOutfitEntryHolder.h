#pragma once

#include "ConfigOutfitEntry.h"

namespace IED
{
	namespace Data
	{
		namespace OM
		{
			struct configOutfitEntryHolder_t
			{
				friend class boost::serialization::access;

			public:
				using data_type = configSexRoot_t<configOutfitEntry_t>;

				enum Serialization : unsigned int
				{
					DataVersion1 = 1
				};

				inline void clear() noexcept
				{
					data.clear();
				}

				template <class Tf>
				void visit(Tf a_func)
				{
					for (auto& e : data())
					{
						a_func(e);
					}
				}

				data_type data;

			private:
				template <class Archive>
				void serialize(Archive& a_ar, const unsigned int a_version)
				{
					a_ar& data;
				}
			};
		}
	}
}

BOOST_CLASS_VERSION(
	::IED::Data::OM::configOutfitEntryHolder_t,
	::IED::Data::OM::configOutfitEntryHolder_t::Serialization::DataVersion1);
