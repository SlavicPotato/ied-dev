#pragma once

#include "IED/ConfigCommon.h"

namespace IED
{
	namespace Data
	{
		namespace OM
		{
			enum class ConfigOutfitFlags : std::uint32_t
			{
				kNone = 0,

				kUseID = 1u << 0
			};

			DEFINE_ENUM_CLASS_BITWISE(ConfigOutfitFlags);

			struct configOutfit_t
			{
				friend class boost::serialization::access;

			public:
				enum Serialization : unsigned int
				{
					DataVersion1 = 1,
				};

				static constexpr auto DEFAULT_FLAGS = ConfigOutfitFlags::kNone;

				configOutfit_t() = default;

				stl::flag<ConfigOutfitFlags> flags{ DEFAULT_FLAGS };
				configFormZeroMissing_t      outfit;
				stl::fixed_string            id;

			protected:
				template <class Archive>
				void serialize(Archive& a_ar, const unsigned int a_version)
				{
					a_ar& flags.value;
					a_ar& outfit;
					a_ar& id;
				}
			};
		}
	}
}

BOOST_CLASS_VERSION(
	::IED::Data::OM::configOutfit_t,
	::IED::Data::OM::configOutfit_t::Serialization::DataVersion1);
