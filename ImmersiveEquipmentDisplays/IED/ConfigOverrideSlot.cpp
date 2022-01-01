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
			-> result_copy
		{
			result_copy result;

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
			-> result_copy
		{
			result_copy result;

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
			-> result_copy
		{
			result_copy result;

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
			-> result_copy
		{
			result_copy result;

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

	}
}