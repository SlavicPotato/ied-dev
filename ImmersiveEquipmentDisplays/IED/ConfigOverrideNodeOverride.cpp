#include "pch.h"

#include "ConfigOverrideNodeOverride.h"

#include "IED/Data.h"

namespace IED
{
	namespace Data
	{
		configNodeOverrideHolder_t configStoreNodeOverride_t::GetActor(
			Game::FormID a_actor,
			Game::FormID a_npc,
			Game::FormID a_race) const
		{
			configNodeOverrideHolder_t result;

			if (auto& actorData = GetActorData(); !actorData.empty())
			{
				if (auto it = actorData.find(a_actor); it != actorData.end())
				{
					CopyEntries(it->second, result);
				}
			}

			if (auto& npcData = GetNPCData(); !npcData.empty())
			{
				if (auto it = npcData.find(a_npc); it != npcData.end())
				{
					CopyEntries(it->second, result);
				}
			}

			if (auto& raceData = GetRaceData(); !raceData.empty())
			{
				if (auto it = raceData.find(a_race); it != raceData.end())
				{
					CopyEntries(it->second, result);
				}
			}

			auto type =
				a_actor == Data::IData::GetPlayerRefID() ?
                    GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			CopyEntries(GetGlobalData(type), result);

			return result;
		}

		configNodeOverrideHolder_t configStoreNodeOverride_t::GetNPC(
			Game::FormID a_npc,
			Game::FormID a_race) const
		{
			configNodeOverrideHolder_t result;

			if (auto& npcData = GetNPCData(); !npcData.empty())
			{
				if (auto it = npcData.find(a_npc); it != npcData.end())
				{
					CopyEntries(it->second, result);
				}
			}

			if (auto& raceData = GetRaceData(); !raceData.empty())
			{
				if (auto it = raceData.find(a_race); it != raceData.end())
				{
					CopyEntries(it->second, result);
				}
			}

			auto type =
				a_npc == Data::IData::GetPlayerBaseID() ?
                    GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			CopyEntries(GetGlobalData(type), result);

			return result;
		}

		configNodeOverrideHolder_t configStoreNodeOverride_t::GetRace(
			Game::FormID a_race,
			GlobalConfigType a_globtype) const
		{
			configNodeOverrideHolder_t result;

			if (auto& raceData = GetRaceData(); !raceData.empty())
			{
				if (auto it = raceData.find(a_race); it != raceData.end())
				{
					CopyEntries(it->second, result);
				}
			}

			CopyEntries(GetGlobalData(a_globtype), result);

			return result;
		}

		const configNodeOverrideEntryTransform_t* configStoreNodeOverride_t::GetActorCME(
			Game::FormID a_actor,
			Game::FormID a_npc,
			Game::FormID a_race,
			const stl::fixed_string& a_node,
			holderCache_t& a_hc) const
		{
			if (auto& actorData = GetActorData(); !actorData.empty())
			{
				if (auto data = a_hc.get_actor(a_actor, actorData))
				{
					if (auto r = GetEntry(data->data, a_node))
					{
						return r;
					}
				}
			}

			if (auto& npcData = GetNPCData(); !npcData.empty())
			{
				if (auto data = a_hc.get_npc(a_npc, npcData))
				{
					if (auto r = GetEntry(data->data, a_node))
					{
						return r;
					}
				}
			}

			if (auto& raceData = GetRaceData(); !raceData.empty())
			{
				if (auto data = a_hc.get_race(a_race, raceData))
				{
					if (auto r = GetEntry(data->data, a_node))
					{
						return r;
					}
				}
			}

			auto type =
				a_actor == Data::IData::GetPlayerRefID() ?
                    GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			return GetEntry(GetGlobalData(type).data, a_node);
		}

		const configNodeOverrideEntryPlacement_t* configStoreNodeOverride_t::GetActorPlacement(
			Game::FormID a_actor,
			Game::FormID a_npc,
			Game::FormID a_race,
			const stl::fixed_string& a_node,
			holderCache_t& a_hc) const
		{
			if (auto& actorData = GetActorData(); !actorData.empty())
			{
				if (auto data = a_hc.get_actor(a_actor, actorData))
				{
					if (auto r = GetEntry(data->placementData, a_node))
					{
						return r;
					}
				}
			}

			if (auto& npcData = GetNPCData(); !npcData.empty())
			{
				if (auto data = a_hc.get_npc(a_npc, npcData))
				{
					if (auto r = GetEntry(data->placementData, a_node))
					{
						return r;
					}
				}
			}

			if (auto& raceData = GetRaceData(); !raceData.empty())
			{
				if (auto data = a_hc.get_race(a_race, raceData))
				{
					if (auto r = GetEntry(data->placementData, a_node))
					{
						return r;
					}
				}
			}

			auto type =
				a_actor == Data::IData::GetPlayerRefID() ?
                    GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			return GetEntry(GetGlobalData(type).placementData, a_node);
		}

	}
}