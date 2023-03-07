#pragma once

#include "ConfigOutfitOverride.h"

namespace IED
{
	struct processParams_t;

	namespace Data
	{
		namespace OM
		{
			enum class ConfigOutfitEntryFlags : std::uint32_t
			{
				kNone = 0,
			};

			DEFINE_ENUM_CLASS_BITWISE(ConfigOutfitEntryFlags);

			struct configOutfitEntry_t :
				configOutfit_t
			{
				friend class boost::serialization::access;

			public:
				enum Serialization : unsigned int
				{
					DataVersion1 = 1,
				};

				static constexpr auto DEFAULT_FLAGS = ConfigOutfitEntryFlags::kNone;

				configOutfitEntry_t() = default;

				inline void clear()
				{
					*this = {};
				}

				const configOutfit_t&   get_config(IED::processParams_t& a_params) const noexcept;
				const outfitOverride_t* get_override(IED::processParams_t& a_params) const noexcept;

				template <class Tf>
				void visit_overrides(Tf a_func)
				{
					for (auto& e : overrides)
					{
						e.visit(a_func);
					}
				}

				stl::flag<ConfigOutfitEntryFlags> entryFlags{ DEFAULT_FLAGS };
				outfitOverrideList_t              overrides;

			private:
				static const outfitOverride_t* get_override_impl(
					IED::processParams_t&       a_params,
					const outfitOverrideList_t& a_list) noexcept;

			protected:
				template <class Archive>
				void serialize(Archive& a_ar, const unsigned int a_version)
				{
					a_ar& static_cast<configOutfit_t&>(*this);
					a_ar& entryFlags.value;
					a_ar& overrides;
				}
			};
		}
	}
}

BOOST_CLASS_VERSION(
	::IED::Data::OM::configOutfitEntry_t,
	::IED::Data::OM::configOutfitEntry_t::Serialization::DataVersion1);
