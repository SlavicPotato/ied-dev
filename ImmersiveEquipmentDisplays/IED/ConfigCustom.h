#pragma once

#include "ConfigBase.h"
#include "ConfigBipedObjectList.h"
#include "ConfigInventory.h"
#include "ConfigLastEquipped.h"
#include "ConfigModelGroup.h"
#include "ConfigVariableSourceSelector.h"

namespace IED
{
	namespace Data
	{
		enum class CustomFlags : std::uint32_t
		{
			kNone = 0,

			kIsInInventory    = 1u << 0,
			kIgnorePlayer     = 1u << 2,
			kLeftWeapon       = 1u << 3,
			kUseChance        = 1u << 5, 
			kAlwaysUnload     = 1u << 4,
			kDisableHavok     = 1u << 8,
			kGroupMode        = 1u << 9,
			kLastEquippedMode = 1u << 12,

			// legacy last-equipped flags
			kUnused1 = 1u << 13,  // kPrioritizeRecentSlots
			kUnused2 = 1u << 14,  // kDisableIfSlotOccupied
			kUnused3 = 1u << 15,  // kSkipOccupiedSlots

			// legacy inv flags
			
			kUnused5 = 1u << 6,   // kIgnoreRaceEquipTypes
			kUnused6 = 1u << 7,   // kDisableIfEquipped
			kUnused7 = 1u << 11,  // kSelectInvRandom
			kUnused8 = 1u << 1,   // kEquipmentMode
			kUnused9 = 1u << 10,  // kCheckFav

			kVariableMode = 1u << 16,

			kNonSingleMask = kGroupMode | kLastEquippedMode,
			//kEquipmentModeMask = kEquipmentMode,
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
				DataVersion6 = 6,
			};

			static constexpr auto DEFAULT_CUSTOM_FLAGS =
				CustomFlags::kAlwaysUnload;

			constexpr void move_legacy_flags_to_le() noexcept
			{
				lastEquipped.flags.set(LastEquippedFlags::kPrioritizeRecentBipedSlots, customFlags.consume(CustomFlags::kUnused1));
				lastEquipped.flags.set(LastEquippedFlags::kDisableIfSlotOccupied, customFlags.consume(CustomFlags::kUnused2));
				lastEquipped.flags.set(LastEquippedFlags::kSkipOccupiedBipedSlots, customFlags.consume(CustomFlags::kUnused3));
			}

			constexpr void move_custom_flags_to_inv() noexcept
			{
				inv.flags.set(InventoryFlags::kEquipmentMode, customFlags.consume(CustomFlags::kUnused8));
				inv.flags.set(InventoryFlags::kIgnoreRaceEquipTypes, customFlags.consume(CustomFlags::kUnused5));
				inv.flags.set(InventoryFlags::kDisableIfEquipped, customFlags.consume(CustomFlags::kUnused6));
				inv.flags.set(InventoryFlags::kSelectInvRandom, customFlags.consume(CustomFlags::kUnused7));
				inv.flags.set(InventoryFlags::kCheckFav, customFlags.consume(CustomFlags::kUnused8));
			}

			stl::flag<CustomFlags>         customFlags{ DEFAULT_CUSTOM_FLAGS };
			configCachedForm_t             form;
			configCachedForm_t             modelForm;
			std::uint32_t                  priority{ 0 };  // unused
			float                          probability{ 100.0f };
			configModelGroup_t             group;
			configLastEquipped_t           lastEquipped;
			configVariableSourceSelector_t vss;
			configInventory_t              inv;

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& static_cast<const configBase_t&>(*this);
				a_ar & customFlags.value;
				a_ar & form;
				a_ar & modelForm;
				a_ar & priority;
				a_ar & probability;
				a_ar & group;
				a_ar & lastEquipped;
				a_ar & vss.varSource;
				a_ar & vss.formVars;
				a_ar & inv;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configBase_t&>(*this);
				a_ar & customFlags.value;
				a_ar & form;
				a_ar & modelForm;

				if (a_version < DataVersion6)
				{
					a_ar & inv.countRange.min;
					a_ar & inv.countRange.max;
				}

				a_ar & priority;
				a_ar & probability;

				if (a_version < DataVersion6)
				{
					a_ar & inv.extraItems;
				}

				if (a_version >= DataVersion2)
				{
					a_ar & group;

					if (a_version >= DataVersion5)
					{
						a_ar & lastEquipped;
						a_ar & vss.varSource;
						a_ar & vss.formVars;

						if (a_version >= DataVersion6)
						{
							a_ar & inv;
						}
						else
						{
							move_custom_flags_to_inv();
						}
					}
					else if (a_version >= DataVersion3)
					{
						move_legacy_flags_to_le();

						a_ar & lastEquipped.bipedSlots;

						if (a_version >= DataVersion4)
						{
							a_ar & lastEquipped.filterConditions.list;
						}
					}
				}

				form.zero_missing_or_deleted();
				modelForm.zero_missing_or_deleted();
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
				a_ar & data;
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
			void visit(Tf a_func)                                         //
				noexcept(noexcept(configCustomHolder_t().visit(a_func)))  //
				requires(requires(configCustomHolder_t a_v, Tf a_func) { a_v.visit(a_func); })
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
	::IED::Data::configCustom_t::Serialization::DataVersion6);

BOOST_CLASS_VERSION(
	::IED::Data::configCustomHolder_t,
	::IED::Data::configCustomHolder_t::Serialization::DataVersion1);
