#pragma once

#include "ConfigBase.h"
#include "ConfigSlotPriority.h"

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
				DataVersion1 = 1,
				DataVersion2 = 2
			};

			static constexpr auto DEFAULT_SLOT_FLAGS = SlotFlags::kNone;

			stl::flag<SlotFlags>             slotFlags{ DEFAULT_SLOT_FLAGS };
			configFormList_t                 preferredItems;
			configFormFilter_t               itemFilter;
			equipmentOverrideConditionList_t itemFilterCondition;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configBase_t&>(*this);
				a_ar& slotFlags.value;
				a_ar& preferredItems;
				a_ar& itemFilter;

				if (a_version >= DataVersion2)
				{
					a_ar& itemFilterCondition;
				}
			}
		};

		class configStoreSlot_t;

		struct configSlotHolderCopy_t;

		struct configSlotHolder_t
		{
			friend class boost::serialization::access;
			friend class configStoreSlot_t;

		public:
			using data_type       = configSexRoot_t<configSlot_t>;
			using prio_data_type  = configSexRoot_t<configSlotPriority_t>;
			using array_data_type = std::unique_ptr<data_type>[stl::underlying(ObjectSlot::kMax)];

			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2
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

				priority.reset();
			}

			bool empty() const noexcept
			{
				if (priority)
				{
					return false;
				}

				for (auto& e : data)
				{
					if (e)
					{
						return false;
					}
				}

				return true;
			}

			[[nodiscard]] constexpr const auto& get(ObjectSlot a_slot) const noexcept
			{
				assert(a_slot < ObjectSlot::kMax);
				return data[stl::underlying(a_slot)];
			}

			[[nodiscard]] constexpr auto& get(ObjectSlot a_slot) noexcept
			{
				assert(a_slot < ObjectSlot::kMax);
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

			array_data_type                                        data;
			std::unique_ptr<configSexRoot_t<configSlotPriority_t>> priority;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& data;

				if (a_version >= DataVersion2)
				{
					a_ar& priority;
				}
			}
		};

		struct configSlotHolderCopy_t
		{
			friend class configStoreSlot_t;

		public:
			template <class Td>
			struct data_value_pair
			{
				ConfigClass first{ ConfigClass::Global };
				Td          second;
			};

			using data_type       = data_value_pair<configSexRoot_t<configSlot_t>>;
			using prio_data_type  = data_value_pair<configSexRoot_t<configSlotPriority_t>>;
			using array_data_type = std::unique_ptr<data_type>[stl::underlying(ObjectSlot::kMax)];

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

				priority.reset();
			}

			bool empty() const noexcept
			{
				if (priority)
				{
					return false;
				}

				for (auto& e : data)
				{
					if (e)
					{
						return false;
					}
				}

				return true;
			}

			[[nodiscard]] constexpr const auto& get(ObjectSlot a_slot) const noexcept
			{
				assert(a_slot < ObjectSlot::kMax);
				return data[stl::underlying(a_slot)];
			}

			[[nodiscard]] constexpr auto& get(ObjectSlot a_slot) noexcept
			{
				assert(a_slot < ObjectSlot::kMax);
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

			void copy_cc_prio(
				ConfigClass         a_class,
				configSlotHolder_t& a_out) const;

			array_data_type                 data;
			std::unique_ptr<prio_data_type> priority;
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
				configSlotHolderCopy_t& a_out) const;

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

			const configSlotHolder_t::prio_data_type* GetActorPriority(
				Game::FormID   a_actor,
				Game::FormID   a_npc,
				Game::FormID   a_race,
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

		template <class Td>
		constexpr void assign_uptr(
			const std::unique_ptr<Td>& a_src,
			std::unique_ptr<Td>&       a_dst)
		{
			if (a_src)
			{
				if (a_dst)
				{
					*a_dst = *a_src;
				}
				else
				{
					a_dst = std::make_unique<Td>(*a_src);
				}
			}
			else
			{
				a_dst.reset();
			}
		}

		template <class Td>
		constexpr void assign_uptr(
			const std::unique_ptr<configSlotHolderCopy_t::data_value_pair<Td>>& a_src,
			std::unique_ptr<Td>&                                                a_dst)
		{
			if (a_src)
			{
				if (a_dst)
				{
					*a_dst = a_src->second;
				}
				else
				{
					a_dst = std::make_unique<Td>(a_src->second);
				}
			}
			else
			{
				a_dst.reset();
			}
		}

		template <class Td>
		constexpr void assign_uptr(
			std::unique_ptr<configSlotHolderCopy_t::data_value_pair<Td>>&& a_src,
			std::unique_ptr<Td>&                                           a_dst)
		{
			if (a_src)
			{
				if (a_dst)
				{
					*a_dst = std::move(a_src->second);
				}
				else
				{
					a_dst = std::make_unique<Td>(std::move(a_src->second));
				}

				a_src.reset();
			}
			else
			{
				a_dst.reset();
			}
		}

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configSlot_t,
	::IED::Data::configSlot_t::Serialization::DataVersion2);

BOOST_CLASS_VERSION(
	::IED::Data::configSlotHolder_t,
	::IED::Data::configSlotHolder_t::Serialization::DataVersion2);
