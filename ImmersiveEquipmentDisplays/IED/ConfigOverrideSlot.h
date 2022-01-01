#pragma once

#include "ConfigOverrideBase.h"

namespace IED
{
	namespace Data
	{
		enum class SlotFlags : std::uint32_t
		{
			kNone = 0,

			kAlwaysUnload = 1u << 0,
			kCheckCannotWear = 1u << 1
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
			configFormList_t preferredItems;
			configFormFilter_t itemFilter;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& static_cast<configBase_t&>(*this);
				ar& slotFlags.value;
				ar& preferredItems;
				ar& itemFilter;
			}
		};

		class configStoreSlot_t;

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

			configSlotHolder_t(const configSlotHolder_t& a_rhs)
			{
				using enum_type = std::underlying_type_t<ObjectSlot>;

				for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
				{
					if (auto& src = a_rhs.data[i])
					{
						data[i] = std::make_unique<data_type>(*src);
					}
				}
			}

			configSlotHolder_t(configSlotHolder_t&&) = default;

			configSlotHolder_t& operator=(const configSlotHolder_t& a_rhs)
			{
				using enum_type = std::underlying_type_t<ObjectSlot>;

				for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
				{
					if (auto& src = a_rhs.data[i])
					{
						if (auto& dst = data[i])
						{
							*dst = *src;
						}
						else
						{
							data[i] = std::make_unique<data_type>(*src);
						}
					}
					else
					{
						data[i].reset();
					}
				}

				return *this;
			}

			configSlotHolder_t& operator=(configSlotHolder_t&&) = default;

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
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& data;
			}
		};

		using configMapSlot_t = configFormMap_t<configSlotHolder_t>;

		class configStoreSlot_t :
			public configStoreBase_t<configSlotHolder_t>
		{
		public:
			struct result
			{
				struct result_entry
				{
					const data_type::data_type* data{ nullptr };
					ConfigClass conf_class{ ConfigClass::Global };
				};

				result_entry entries[stl::underlying(ObjectSlot::kMax)];
			};

			struct result_copy
			{
				struct result_entry
				{
				public:
					result_entry() = default;

					result_entry(const result_entry& a_rhs) :
						conf_class(a_rhs.conf_class)
					{
						if (a_rhs.data)
						{
							__copy(a_rhs);
						}
					}

					result_entry(result_entry&&) = default;

					result_entry& operator=(const result_entry& a_rhs)
					{
						conf_class = a_rhs.conf_class;

						if (a_rhs.data)
						{
							__copy(a_rhs);
						}
						else
						{
							data.reset();
						}

						return *this;
					}

					result_entry& operator=(result_entry&&) = default;

					std::unique_ptr<data_type::data_type> data;
					ConfigClass conf_class{ ConfigClass::Global };

				private:
					void __copy(const result_entry& a_rhs)
					{
						if (!data)
						{
							data = std::make_unique<data_type::data_type>(*a_rhs.data);
						}
						else
						{
							*data = *a_rhs.data;
						}
					}
				};

				inline constexpr const auto& get(ObjectSlot a_slot) const noexcept
				{
					return entries[stl::underlying(a_slot)];
				}

				inline constexpr auto& get(ObjectSlot a_slot) noexcept
				{
					return entries[stl::underlying(a_slot)];
				}

				result_copy() = default;

				result_copy(const configSlotHolder_t& a_rhs)
				{
					using enum_type = std::underlying_type_t<ObjectSlot>;

					for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
					{
						if (auto& src = a_rhs.data[i])
						{
							auto& dst = entries[i];

							dst.conf_class = ConfigClass::Global;
							dst.data = std::make_unique<configSlotHolder_t::data_type>(*src);
						}
					}
				}

				result_copy& operator=(const configSlotHolder_t& a_rhs)
				{
					using enum_type = std::underlying_type_t<ObjectSlot>;

					for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
					{
						auto& dst = entries[i];

						dst.conf_class = ConfigClass::Global;

						if (auto& src = a_rhs.data[i])
						{
							if (dst.data)
							{
								*dst.data = *src;
							}
							else
							{
								dst.data = std::make_unique<configSlotHolder_t::data_type>(*src);
							}
						}
						else
						{
							dst.data.reset();
						}
					}
				}

				result_entry entries[stl::underlying(ObjectSlot::kMax)];
			};

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

		private:
			SKMP_FORCEINLINE void FillResult(
				ConfigClass a_class,
				const data_type& a_data,
				result& a_out) const
			{
				using enum_type = std::underlying_type_t<ObjectSlot>;

				for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
				{
					auto& from = a_data.data[i];

					if (!from)
					{
						continue;
					}

					auto& to = a_out.entries[i];

					if (!to.data)
					{
						to.data = from.get();
						to.conf_class = a_class;
					}
				}
			}

			void FillResultCopy(
				ConfigClass a_class,
				const data_type& a_data,
				result_copy& a_out) const
			{
				using enum_type = std::underlying_type_t<ObjectSlot>;

				for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
				{
					auto& from = a_data.data[i];

					if (!from)
					{
						continue;
					}

					auto& to = a_out.entries[i];

					if (!to.data)
					{
						to.data = std::make_unique<data_type::data_type>(*from);
						to.conf_class = a_class;
					}
				}
			}

		public:
			result GetGlobal(
				GlobalConfigType a_type) const;

			result_copy GetGlobalCopy(
				GlobalConfigType a_type) const;

			result GetRace(
				Game::FormID a_race,
				GlobalConfigType a_globtype) const;

			result_copy GetRaceCopy(
				Game::FormID a_race,
				GlobalConfigType a_globtype) const;

			result GetNPC(
				Game::FormID a_npc,
				Game::FormID a_race) const;

			result_copy GetNPCCopy(
				Game::FormID a_npc,
				Game::FormID a_race) const;

			result GetActor(
				Game::FormID a_actor,
				Game::FormID a_npc,
				Game::FormID a_race) const;

			result_copy GetActorCopy(
				Game::FormID a_actor,
				Game::FormID a_npc,
				Game::FormID a_race) const;

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
	IED::Data::configSlot_t,
	IED::Data::configSlot_t::Serialization::DataVersion1);
