#include "pch.h"

#include "ConfigOverrideSlot.h"
#include "Data.h"

namespace IED
{
	namespace Data
	{
		auto configStoreSlot_t::GetGlobal(
			GlobalConfigType a_type) const
			-> result
		{
			result result;

			FillResult(
				ConfigClass::Global,
				GetGlobalData(a_type),
				result);

			return result;
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

		auto configStoreSlot_t::GetRace(
			Game::FormID a_race,
			GlobalConfigType a_globtype) const
			-> result
		{
			result result;

			auto& racemap = GetRaceData();
			if (auto it = racemap.find(a_race); it != racemap.end())
			{
				FillResult(ConfigClass::Race, it->second, result);
			}

			FillResult(
				ConfigClass::Global,
				GetGlobalData(a_globtype),
				result);

			return result;
		}

		auto configStoreSlot_t::GetRaceCopy(
			Game::FormID a_race,
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

		auto configStoreSlot_t::GetNPC(
			Game::FormID a_npc,
			Game::FormID a_race) const
			-> result
		{
			result result;

			auto& npcmap = GetNPCData();
			if (auto it = npcmap.find(a_npc); it != npcmap.end())
			{
				FillResult(ConfigClass::NPC, it->second, result);
			}

			auto& racemap = GetRaceData();
			if (auto it = racemap.find(a_race); it != racemap.end())
			{
				FillResult(ConfigClass::Race, it->second, result);
			}

			auto type =
				a_npc == IData::GetPlayerBaseID() ?
                    GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			FillResult(
				ConfigClass::Global,
				GetGlobalData(type),
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

		auto configStoreSlot_t::GetActor(
			Game::FormID a_actor,
			Game::FormID a_npc,
			Game::FormID a_race) const
			-> result
		{
			result result;

			auto& actormap = GetActorData();
			if (auto it = actormap.find(a_actor); it != actormap.end())
			{
				FillResult(ConfigClass::Actor, it->second, result);
			}

			auto& npcmap = GetNPCData();
			if (auto it = npcmap.find(a_npc); it != npcmap.end())
			{
				FillResult(ConfigClass::NPC, it->second, result);
			}

			auto& racemap = GetRaceData();
			if (auto it = racemap.find(a_race); it != racemap.end())
			{
				FillResult(ConfigClass::Race, it->second, result);
			}

			auto type =
				a_actor == IData::GetPlayerRefID() ?
                    GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			FillResult(
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
		}

		configSlotHolder_t& configSlotHolder_t::operator=(const configSlotHolder_t& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				auto& dst = data[i];

				if (auto& src = a_rhs.data[i])
				{
					if (dst)
					{
						*dst = *src;
					}
					else
					{
						dst = std::make_unique<data_type>(*src);
					}
				}
				else
				{
					dst.reset();
				}
			}

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
		}

		configSlotHolder_t::configSlotHolder_t(configSlotHolderCopy_t&& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				if (auto& src = a_rhs.data[i])
				{
					data[i] = std::make_unique<data_type>(std::move(src->second));
				}
			}
		}

		configSlotHolder_t& configSlotHolder_t::operator=(const configSlotHolderCopy_t& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				auto& dst = data[i];

				if (auto& src = a_rhs.data[i])
				{
					if (dst)
					{
						*dst = src->second;
					}
					else
					{
						dst = std::make_unique<data_type>(src->second);
					}
				}
				else
				{
					dst.reset();
				}
			}

			return *this;
		}

		configSlotHolder_t& configSlotHolder_t::operator=(configSlotHolderCopy_t&& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				auto& dst = data[i];

				if (auto& src = a_rhs.data[i])
				{
					if (dst)
					{
						*dst = std::move(src->second);
					}
					else
					{
						data[i] = std::make_unique<data_type>(std::move(src->second));
					}
				}
				else
				{
					dst.reset();
				}
			}

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
		}

		configSlotHolderCopy_t& configSlotHolderCopy_t::operator=(const configSlotHolderCopy_t& a_rhs)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				auto& dst = data[i];

				if (auto& src = a_rhs.data[i])
				{
					if (dst)
					{
						*dst = *src;
					}
					else
					{
						dst = std::make_unique<data_type>(*src);
					}
				}
				else
				{
					dst.reset();
				}
			}

			return *this;
		}

		configSlotHolderCopy_t::configSlotHolderCopy_t(
			const configSlotHolder_t& a_rhs,
			ConfigClass a_initclass)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				if (auto& src = a_rhs.data[i])
				{
					data[i] = std::make_unique<data_type>(a_initclass, *src);
				}
			}
		}

		configSlotHolderCopy_t::configSlotHolderCopy_t(
			configSlotHolder_t&& a_rhs,
			ConfigClass a_initclass)
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				if (auto& src = a_rhs.data[i])
				{
					data[i] = std::make_unique<data_type>(a_initclass, std::move(*src));
				}
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

			return result;
		}

		void configSlotHolderCopy_t::copy_cc(
			ConfigClass a_class,
			configSlotHolder_t& a_out) const
		{
			using enum_type = std::underlying_type_t<ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(ObjectSlot::kMax); i++)
			{
				if (auto& src = data[i]; src && src->first == a_class)
				{
					a_out.data[i] = std::make_unique<configSlotHolder_t::data_type>(src->second);
				}
				else
				{
					a_out.data[i].reset();
				}
			}
		}

	}
}