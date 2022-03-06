#pragma once

#include "ConfigOverrideBase.h"
#include "ConfigOverrideModelGroup.h"

namespace IED
{
	namespace Data
	{
		enum class CustomFlags : std::uint32_t
		{
			kNone = 0,

			kIsInInventory        = 1u << 0,
			kEquipmentMode        = 1u << 1,
			kIgnorePlayer         = 1u << 2,
			kLeftWeapon           = 1u << 3,
			kAlwaysUnload         = 1u << 4,
			kUseChance            = 1u << 5,
			kIgnoreRaceEquipTypes = 1u << 6,
			kDisableIfEquipped    = 1u << 7,
			kDisableHavok         = 1u << 8,
			kUseGroup             = 1u << 9,
			kCheckFav             = 1u << 10,
			kSelectInvRandom      = 1u << 11,
		};

		DEFINE_ENUM_CLASS_BITWISE(CustomFlags);

		struct configCustom_t :
			public configBase_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2,
			};

			static inline constexpr auto DEFAULT_CUSTOM_FLAGS = CustomFlags::kAlwaysUnload;

			stl::flag<CustomFlags> customFlags{ DEFAULT_CUSTOM_FLAGS };
			configCachedForm_t     form;
			configCachedForm_t     modelForm;
			configRange_t          countRange;
			std::uint32_t          priority{ 0 };
			float                  chance{ 100.0f };
			configFormList_t       extraItems;
			configModelGroup_t     group;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configBase_t&>(*this);
				a_ar& customFlags.value;
				a_ar& form;
				a_ar& modelForm;
				a_ar& countRange.min;
				a_ar& countRange.max;
				a_ar& priority;
				a_ar& chance;
				a_ar& extraItems;

				if (a_version >= DataVersion2)
				{
					a_ar& group;
				}
			}
		};

		using configCustomEntry_t = configSexRoot_t<configCustom_t>;

		struct configCustomNameValue_t
		{
			stl::fixed_string   name;
			ConfigSex           sex{ ConfigSex::Male };
			configCustomEntry_t data;
		};

		using configCustomEntryMap_t =
			std::unordered_map<stl::fixed_string, configCustomEntry_t>;

		struct configCustomHolder_t
		{
			friend class boost::serialization::access;

		public:
			using data_type = configCustomEntryMap_t;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			inline void clear() noexcept
			{
				data.clear();
			}

			inline bool empty() const noexcept
			{
				return data.empty();
			}

			template <class Tf>
			void visit(Tf a_func)
			{
				for (auto& e : data)
				{
					e.second.visit(a_func);
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

		using configCustomPluginMap_t = std::unordered_map<stl::fixed_string, configCustomHolder_t>;

		using configMapCustom_t = configFormMap_t<configCustomPluginMap_t>;

		class configStoreCustom_t :
			public configStoreBase_t<configCustomPluginMap_t>
		{
			struct lookup_result_t
			{
				configCustomHolder_t& data;
			};

		public:
			template <class Tf>
			void visit(Tf a_func)
			{
				for (auto& e : data)
				{
					for (auto& f : e)
					{
						for (auto& g : f.second)
						{
							g.second.visit(a_func);
						}
					}
				}

				for (auto& e : global)
				{
					for (auto& f : e)
					{
						f.second.visit(a_func);
					}
				}
			}
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configCustom_t,
	::IED::Data::configCustom_t::Serialization::DataVersion2);

BOOST_CLASS_VERSION(
	::IED::Data::configCustomHolder_t,
	::IED::Data::configCustomHolder_t::Serialization::DataVersion1);
