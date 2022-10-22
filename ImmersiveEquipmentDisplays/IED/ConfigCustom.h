#pragma once

#include "ConfigBase.h"
#include "ConfigBipedObjectList.h"
#include "ConfigLastEquipped.h"
#include "ConfigModelGroup.h"
#include "ConfigVariableSource.h"

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
			kGroupMode            = 1u << 9,
			kCheckFav             = 1u << 10,
			kSelectInvRandom      = 1u << 11,
			kLastEquippedMode     = 1u << 12,

			// legacy last-equipped flags
			kUnused1 = 1u << 13,  // kPrioritizeRecentSlots
			kUnused2 = 1u << 14,  // kDisableIfSlotOccupied
			kUnused3 = 1u << 15,  // kSkipOccupiedSlots

			kVariableMode = 1u << 16,

			kNonSingleMask     = kGroupMode | kLastEquippedMode,
			kEquipmentModeMask = kEquipmentMode,  //| kUseLastEquipped,
			kIsInInventoryMask = kIsInInventory | kLastEquippedMode,
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
				DataVersion3 = 3,
				DataVersion4 = 4,
				DataVersion5 = 5,
			};

			static inline constexpr auto DEFAULT_CUSTOM_FLAGS =
				CustomFlags::kAlwaysUnload;

			inline constexpr void move_legacy_flags_to_le() noexcept
			{
				lastEquipped.flags.set(LastEquippedFlags::kPrioritizeRecentSlots, customFlags.consume(CustomFlags::kUnused1));
				lastEquipped.flags.set(LastEquippedFlags::kDisableIfSlotOccupied, customFlags.consume(CustomFlags::kUnused2));
				lastEquipped.flags.set(LastEquippedFlags::kSkipOccupiedSlots, customFlags.consume(CustomFlags::kUnused3));
			}

			stl::flag<CustomFlags>  customFlags{ DEFAULT_CUSTOM_FLAGS };
			configCachedForm_t      form;
			configCachedForm_t      modelForm;
			configRange_t           countRange;
			std::uint32_t           priority{ 0 };  // unused
			float                   probability{ 100.0f };
			configFormList_t        extraItems;
			configModelGroup_t      group;
			configLastEquipped_t    lastEquipped;
			configVariableSource_t  varSource;
			configFixedStringList_t formVars;

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& static_cast<const configBase_t&>(*this);
				a_ar& customFlags.value;
				a_ar& form;
				a_ar& modelForm;
				a_ar& countRange.min;
				a_ar& countRange.max;
				a_ar& priority;
				a_ar& probability;
				a_ar& extraItems;
				a_ar& group;
				a_ar& lastEquipped;
				a_ar& varSource;
				a_ar& formVars;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configBase_t&>(*this);
				a_ar& customFlags.value;
				a_ar& form;
				a_ar& modelForm;
				a_ar& countRange.min;
				a_ar& countRange.max;
				a_ar& priority;
				a_ar& probability;
				a_ar& extraItems;

				if (a_version >= DataVersion2)
				{
					a_ar& group;

					if (a_version >= DataVersion5)
					{
						a_ar& lastEquipped;
						a_ar& varSource;
						a_ar& formVars;
					}
					else if (a_version >= DataVersion3)
					{
						move_legacy_flags_to_le();

						a_ar& lastEquipped.bipedSlots;

						if (a_version >= DataVersion4)
						{
							a_ar& lastEquipped.filterConditions;
						}
					}
				}
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		using configCustomEntry_t = configSexRoot_t<configCustom_t>;

		struct configCustomNameValue_t
		{
			stl::fixed_string   name;
			ConfigSex           sex{ ConfigSex::Male };
			configCustomEntry_t data;
		};

		using configCustomEntryMap_t =
			stl::boost_unordered_map<stl::fixed_string, configCustomEntry_t>;

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

		using configCustomPluginMap_t = stl::boost_unordered_map<
			stl::fixed_string,
			configCustomHolder_t>;

		using configMapCustom_t = configFormMap_t<configCustomPluginMap_t>;

		class configStoreCustom_t :
			public configStoreBase_t<configCustomPluginMap_t>
		{
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
	::IED::Data::configCustom_t::Serialization::DataVersion5);

BOOST_CLASS_VERSION(
	::IED::Data::configCustomHolder_t,
	::IED::Data::configCustomHolder_t::Serialization::DataVersion1);
