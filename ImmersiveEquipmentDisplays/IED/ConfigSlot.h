#pragma once

#include "ConfigBase.h"

namespace IED
{
	namespace Data
	{
		enum class SlotFlags : std::uint32_t
		{
			kNone = 0,

			kAlwaysUnload    = 1u << 0,
			kCheckCannotWear = 1u << 1,
		};

		DEFINE_ENUM_CLASS_BITWISE(SlotFlags);

		struct configSlot_t : public configBase_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			static inline constexpr auto DEFAULT_SLOT_FLAGS = SlotFlags::kNone;

			stl::flag<SlotFlags> slotFlags{ DEFAULT_SLOT_FLAGS };
			configFormList_t     preferredItems;
			configFormFilter_t   itemFilter;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configBase_t&>(*this);
				a_ar& slotFlags.value;
				a_ar& preferredItems;
				a_ar& itemFilter;
			}
		};

		class configStoreSlot_t;

		struct configSlotHolderCopy_t;

		struct configSlotHolder_t
		{
			friend class boost::serialization::access;
			friend class configStoreSlot_t;

		public:
			using data_type = configSexRoot_t<configSlot_t>;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configSlotHolder_t() = default;

			configSlotHolder_t(const configSlotHolder_t& a_rhs);

			configSlotHolder_t(configSlotHolder_t&&) = default;

			configSlotHolder_t& operator=(const configSlotHolder_t& a_rhs);

			configSlotHolder_t& operator=(configSlotHolder_t&&) = default;

			explicit configSlotHolder_t(
				const configSlotHolderCopy_t& a_rhs);

			explicit configSlotHolder_t(
				configSlotHolderCopy_t&& a_rhs);

			configSlotHolder_t& operator=(
				const configSlotHolderCopy_t& a_rhs);

			configSlotHolder_t& operator=(
				configSlotHolderCopy_t&& a_rhs);

			void clear() noexcept
			{
				for (auto& e : data)
				{
					e.reset();
				}
			}

			bool constexpr empty() const noexcept
			{
				for (auto& e : data)
				{
					if (e)
					{
						return false;
					}
				}

				return true;
			}

			inline constexpr const auto& get(ObjectSlot a_slot) const noexcept
			{
				return data[stl::underlying(a_slot)];
			}

			inline constexpr auto& get(ObjectSlot a_slot) noexcept
			{
				return data[stl::underlying(a_slot)];
			}

			template <class Tf>
			void visit(Tf a_func)
			{
				for (auto& e : data)
				{
					if (e)
					{
						e->visit(a_func);
					}
				}
			}

			std::unique_ptr<data_type> data[stl::underlying(ObjectSlot::kMax)];

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& data;
			}
		};

		struct configSlotHolderCopy_t
		{
			friend class boost::serialization::access;
			friend class configStoreSlot_t;

			template <class Td>
			struct data_value_pair
			{
				ConfigClass first{ ConfigClass::Global };
				Td          second;
			};

		public:
			using data_type = data_value_pair<configSexRoot_t<configSlot_t>>;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configSlotHolderCopy_t() = default;

			configSlotHolderCopy_t(const configSlotHolderCopy_t& a_rhs);

			configSlotHolderCopy_t(configSlotHolderCopy_t&&) = default;

			configSlotHolderCopy_t& operator=(const configSlotHolderCopy_t& a_rhs);

			configSlotHolderCopy_t& operator=(configSlotHolderCopy_t&&) = default;

			configSlotHolderCopy_t(
				const configSlotHolder_t& a_rhs,
				ConfigClass               a_initclass);

			configSlotHolderCopy_t(
				configSlotHolder_t&& a_rhs,
				ConfigClass          a_initclass);

			void clear() noexcept
			{
				for (auto& e : data)
				{
					e.reset();
				}
			}

			constexpr bool empty() const noexcept
			{
				for (auto& e : data)
				{
					if (e)
					{
						return false;
					}
				}

				return true;
			}

			inline constexpr const auto& get(ObjectSlot a_slot) const noexcept
			{
				return data[stl::underlying(a_slot)];
			}

			inline constexpr auto& get(ObjectSlot a_slot) noexcept
			{
				return data[stl::underlying(a_slot)];
			}

			template <class Tf>
			void visit(Tf a_func)
			{
				for (auto& e : data)
				{
					if (e)
					{
						e->visit(a_func);
					}
				}
			}

			configSlotHolder_t copy_cc(
				ConfigClass a_class) const;

			void copy_cc(
				ConfigClass         a_class,
				configSlotHolder_t& a_out) const;

			std::unique_ptr<data_type> data[stl::underlying(ObjectSlot::kMax)];

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& data;
			}
		};

		using configMapSlot_t = configFormMap_t<configSlotHolder_t>;

		class configStoreSlot_t :
			public configStoreBase_t<configSlotHolder_t>
		{
		public:
			using holderCache_t = configHolderCache_t<configMapSlot_t>;

		private:
			void FillResultCopy(
				ConfigClass             a_class,
				const data_type&        a_data,
				configSlotHolderCopy_t& a_out) const
			{
				using enum_type = std::underlying_type_t<ObjectSlot>;

				for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
				{
					auto& from = a_data.data[i];

					if (!from)
					{
						continue;
					}

					if (auto& to = a_out.data[i]; !to)
					{
						to = std::make_unique<configSlotHolderCopy_t::data_type>(a_class, *from);
					}
				}
			}

		public:
			configSlotHolderCopy_t GetGlobalCopy(
				GlobalConfigType a_type) const;

			configSlotHolderCopy_t GetRaceCopy(
				Game::FormID     a_race,
				GlobalConfigType a_globtype) const;

			configSlotHolderCopy_t GetNPCCopy(
				Game::FormID a_npc,
				Game::FormID a_race) const;

			configSlotHolderCopy_t GetActorCopy(
				Game::FormID a_actor,
				Game::FormID a_npc,
				Game::FormID a_race) const;

			const configSlotHolder_t::data_type* GetActor(
				Game::FormID   a_actor,
				Game::FormID   a_npc,
				Game::FormID   a_race,
				ObjectSlot     a_slot,
				holderCache_t& a_hc) const;

			template <class Tf>
			void visit(Tf a_func)
			{
				for (auto& e : data)
				{
					for (auto& f : e)
					{
						f.second.visit(a_func);
					}
				}

				for (auto& e : global)
				{
					e.visit(a_func);
				}
			}
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configSlot_t,
	::IED::Data::configSlot_t::Serialization::DataVersion1);
