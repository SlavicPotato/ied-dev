#include "pch.h"

#include "ConfigSlot.h"
#include "Data.h"

namespace IED
{
	namespace Data
	{
		void configStoreSlot_t::FillResultCopy(
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

			if (a_data.priority && !a_out.priority)
			{
				a_out.priority = std::make_unique<configSlotHolderCopy_t::prio_data_type>(a_class, *a_data.priority);
			}
		}

		auto configStoreSlot_t::GetGlobalCopy(GlobalConfigType a_type) const
			-> configSlotHolderCopy_t
		{
			configSlotHolderCopy_t result;

			FillResultCopy(
				ConfigClass::Global,
				GetGlobalData(a_type),
				result);

			return result;
		}

		auto configStoreSlot_t::GetRaceCopy(
			Game::FormID     a_race,
			GlobalConfigType a_globtype) const
			-> configSlotHolderCopy_t
		{
			configSlotHolderCopy_t result;

			auto& racemap = GetRaceData();
			if (auto it = racemap.find(a_race); it != racemap.end())
			{
				FillResultCopy(ConfigClass::Race, it->second, result);
			}

			FillResultCopy(
				ConfigClass::Global,
				GetGlobalData(a_globtype),
				result);

			return result;
		}

		auto configStoreSlot_t::GetNPCCopy(
			Game::FormID a_npc,
			Game::FormID a_race) const
			-> configSlotHolderCopy_t
		{
			configSlotHolderCopy_t result;

			auto& npcmap = GetNPCData();
			if (auto it = npcmap.find(a_npc); it != npcmap.end())
			{
				FillResultCopy(ConfigClass::NPC, it->second, result);
			}

			auto& racemap = GetRaceData();
			if (auto it = racemap.find(a_race); it != racemap.end())
			{
				FillResultCopy(ConfigClass::Race, it->second, result);
			}

			auto type =
				a_npc == IData::GetPlayerBaseID() ?
					GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			FillResultCopy(
				ConfigClass::Global,
				GetGlobalData(type),
				result);

			return result;
		}

		auto configStoreSlot_t::GetActorCopy(
			Game::FormID a_actor,
			Game::FormID a_npc,
			Game::FormID a_race) const
			-> configSlotHolderCopy_t
		{
			configSlotHolderCopy_t result;

			auto& actormap = GetActorData();
			if (auto it = actormap.find(a_actor); it != actormap.end())
			{
				FillResultCopy(ConfigClass::Actor, it->second, result);
			}

			auto& npcmap = GetNPCData();
			if (auto it = npcmap.find(a_npc); it != npcmap.end())
			{
				FillResultCopy(ConfigClass::NPC, it->second, result);
			}

			auto& racemap = GetRaceData();
			if (auto it = racemap.find(a_race); it != racemap.end())
			{
				FillResultCopy(ConfigClass::Race, it->second, result);
			}

			auto type =
				a_actor == IData::GetPlayerRefID() ?
					GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			FillResultCopy(
				ConfigClass::Global,
				GetGlobalData(type),
				result);

			return result;
		}

		const configSlotHolder_t::data_type* configStoreSlot_t::GetActor(
			Game::FormID   a_actor,
			Game::FormID   a_npc,
			Game::FormID   a_race,
			ObjectSlot     a_slot,
			holderCache_t& a_hc) const
		{
			if (auto& b = GetActorData(); !b.empty())
			{
				if (auto d = a_hc.get_actor(a_actor, b))
				{
					if (auto r = d->get(a_slot).get())
					{
						return r;
					}
				}
			}

			if (auto& b = GetNPCData(); !b.empty())
			{
				if (auto d = a_hc.get_npc(a_npc, b))
				{
					if (auto r = d->get(a_slot).get())
					{
						return r;
					}
				}
			}

			if (auto& b = GetRaceData(); !b.empty())
			{
				if (auto d = a_hc.get_race(a_race, b))
				{
					if (auto r = d->get(a_slot).get())
					{
						return r;
					}
				}
			}

			auto type =
				a_actor == Data::IData::GetPlayerRefID() ?
					GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			return GetGlobalData(type).get(a_slot).get();
		}

		const configSlotHolder_t::prio_data_type* configStoreSlot_t::GetActorPriority(
			Game::FormID   a_actor,
			Game::FormID   a_npc,
			Game::FormID   a_race,
			holderCache_t& a_hc) const
		{
			if (auto& b = GetActorData(); !b.empty())
			{
				if (auto d = a_hc.get_actor(a_actor, b))
				{
					if (d->priority)
					{
						return d->priority.get();
					}
				}
			}

			if (auto& b = GetNPCData(); !b.empty())
			{
				if (auto d = a_hc.get_npc(a_npc, b))
				{
					if (d->priority)
					{
						return d->priority.get();
					}
				}
			}

			if (auto& b = GetRaceData(); !b.empty())
			{
				if (auto d = a_hc.get_race(a_race, b))
				{
					if (d->priority)
					{
						return d->priority.get();
					}
				}
			}

			auto type =
				a_actor == Data::IData::GetPlayerRefID() ?
					GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			return GetGlobalData(type).priority.get();
		}

		configSlotHolder_t::configSlotHolder_t(const configSlotHolder_t& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				if (auto& src = a_rhs.data[i])
				{
					data[i] = std::make_unique<data_type>(*src);
				}
			}

			if (a_rhs.priority)
			{
				priority = std::make_unique<configSlotHolder_t::prio_data_type>(*a_rhs.priority);
			}
		}

		configSlotHolder_t& configSlotHolder_t::operator=(const configSlotHolder_t& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				assign_uptr(a_rhs.data[i], data[i]);
			}

			assign_uptr(a_rhs.priority, priority);

			return *this;
		}

		configSlotHolder_t::configSlotHolder_t(const configSlotHolderCopy_t& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				if (auto& src = a_rhs.data[i])
				{
					data[i] = std::make_unique<data_type>(src->second);
				}
			}

			if (a_rhs.priority)
			{
				priority = std::make_unique<configSlotHolder_t::prio_data_type>(a_rhs.priority->second);
			}
		}

		configSlotHolder_t::configSlotHolder_t(configSlotHolderCopy_t&& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				if (auto& src = a_rhs.data[i])
				{
					data[i] = std::make_unique<data_type>(std::move(src->second));

					src.reset();
				}
			}

			if (a_rhs.priority)
			{
				priority = std::make_unique<configSlotHolder_t::prio_data_type>(std::move(a_rhs.priority->second));

				a_rhs.priority.reset();
			}
		}

		configSlotHolder_t& configSlotHolder_t::operator=(const configSlotHolderCopy_t& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				assign_uptr(a_rhs.data[i], data[i]);
			}

			assign_uptr(a_rhs.priority, priority);

			return *this;
		}

		configSlotHolder_t& configSlotHolder_t::operator=(configSlotHolderCopy_t&& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				assign_uptr(std::move(a_rhs.data[i]), data[i]);
			}

			assign_uptr(std::move(a_rhs.priority), priority);

			return *this;
		}

		configSlotHolderCopy_t::configSlotHolderCopy_t(const configSlotHolderCopy_t& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				if (auto& src = a_rhs.data[i])
				{
					data[i] = std::make_unique<data_type>(*src);
				}
			}

			if (a_rhs.priority)
			{
				priority = std::make_unique<prio_data_type>(*a_rhs.priority);
			}
		}

		configSlotHolderCopy_t& configSlotHolderCopy_t::operator=(const configSlotHolderCopy_t& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				assign_uptr(a_rhs.data[i], data[i]);
			}

			assign_uptr(a_rhs.priority, priority);

			return *this;
		}

		configSlotHolderCopy_t::configSlotHolderCopy_t(
			const configSlotHolder_t& a_rhs,
			ConfigClass               a_initclass)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				if (auto& src = a_rhs.data[i])
				{
					data[i] = std::make_unique<data_type>(a_initclass, *src);
				}
			}

			if (a_rhs.priority)
			{
				priority = std::make_unique<prio_data_type>(a_initclass, *a_rhs.priority);
			}
		}

		configSlotHolderCopy_t::configSlotHolderCopy_t(
			configSlotHolder_t&& a_rhs,
			ConfigClass          a_initclass)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				if (auto& src = a_rhs.data[i])
				{
					data[i] = std::make_unique<data_type>(a_initclass, std::move(*src));

					src.reset();
				}
			}

			if (a_rhs.priority)
			{
				priority = std::make_unique<prio_data_type>(a_initclass, std::move(*a_rhs.priority));

				a_rhs.priority.reset();
			}
		}

		configSlotHolder_t configSlotHolderCopy_t::copy_cc(
			ConfigClass a_class) const
		{
			configSlotHolder_t result;

			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				if (auto& src = data[i]; src && src->first == a_class)
				{
					result.data[i] = std::make_unique<configSlotHolder_t::data_type>(src->second);
				}
			}

			if (priority && priority->first == a_class)
			{
				result.priority = std::make_unique<configSlotHolder_t::prio_data_type>(priority->second);
			}

			return result;
		}

		void configSlotHolderCopy_t::copy_cc(
			ConfigClass         a_class,
			configSlotHolder_t& a_out) const
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				auto& dst = a_out.data[i];

				if (auto& src = data[i]; src && src->first == a_class)
				{
					if (dst)
					{
						*dst = src->second;
					}
					else
					{
						dst = std::make_unique<configSlotHolder_t::data_type>(src->second);
					}
				}
				else
				{
					dst.reset();
				}
			}

			copy_cc_prio(a_class, a_out);
		}

		void configSlotHolderCopy_t::copy_cc_prio(
			ConfigClass         a_class,
			configSlotHolder_t& a_out) const
		{
			if (auto& src = priority; src && src->first == a_class)
			{
				if (auto& dst = a_out.priority)
				{
					*dst = src->second;
				}
				else
				{
					dst = std::make_unique<configSlotHolder_t::prio_data_type>(src->second);
				}
			}
			else
			{
				a_out.priority.reset();
			}
		}

	}
}