#pragma once

#include "ConfigOverrideBase.h"

namespace IED
{
	namespace Data
	{
		enum class CustomFlags : std::uint32_t
		{
			kNone = 0,

			kIsInInventory = 1u << 0,
			kConsumeItem = 1u << 1,
			kUseSharedCount = 1u << 2,
			kIgnorePlayer = 1u << 3,
			kDeltaEquipped = 1u << 4,
			kLeftWeapon = 1u << 5,
			kAlwaysUnload = 1u << 6,
			kUseChance = 1u << 7,

			kEquipmentMode = kConsumeItem | kUseSharedCount | kDeltaEquipped,

			kIgnoreRaceEquipTypes = 1u << 8,
			kIgnoreExcessItemCount = 1u << 9,

			kLoadARMA = 1u << 10,
		};

		DEFINE_ENUM_CLASS_BITWISE(CustomFlags);

		struct configCustom_t :
			public configBase_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			static inline constexpr auto DEFAULT_CUSTOM_FLAGS = CustomFlags::kAlwaysUnload;

			stl::flag<CustomFlags> customFlags{ DEFAULT_CUSTOM_FLAGS };
			configCachedForm_t form;
			configCachedForm_t modelForm;
			configRange_t countRange;
			std::uint32_t priority{ 0 };
			float chance{ 100.0f };
			configFormList_t extraItems;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& static_cast<configBase_t&>(*this);
				ar& customFlags.value;
				ar& form;
				ar& modelForm;
				ar& countRange.min;
				ar& countRange.max;
				ar& priority;
				ar& chance;
				ar& extraItems;
			}
		};

		using configCustomEntry_t = configSexRoot_t<configCustom_t>;

		struct configCustomNameValue_t
		{
			stl::fixed_string name;
			ConfigSex sex;
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
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& data;
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
			void clear()
			{
				for (auto& e : data)
				{
					e.clear();
				}

				for (auto& e : global)
				{
					e.clear();
				}
			}

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
	IED::Data::configCustom_t,
	IED::Data::configCustom_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configCustomHolder_t,
	IED::Data::configCustomHolder_t::Serialization::DataVersion1);
