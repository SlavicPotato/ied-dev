#include "pch.h"

#include "ConfigNodeOverride.h"

#include "IED/Data.h"

namespace IED
{
	namespace Data
	{
		configNodeOverrideHolderCopy_t configStoreNodeOverride_t::GetActorCopy(
			Game::FormID a_actor,
			Game::FormID a_npc,
			Game::FormID a_race) const
		{
			configNodeOverrideHolderCopy_t result;

			if (auto& actorData = GetActorData(); !actorData.empty())
			{
				if (auto it = actorData.find(a_actor); it != actorData.end())
				{
					CopyEntries(it->second, result, ConfigClass::Actor);
				}
			}

			if (auto& npcData = GetNPCData(); !npcData.empty())
			{
				if (auto it = npcData.find(a_npc); it != npcData.end())
				{
					CopyEntries(it->second, result, ConfigClass::NPC);
				}
			}

			if (auto& raceData = GetRaceData(); !raceData.empty())
			{
				if (auto it = raceData.find(a_race); it != raceData.end())
				{
					CopyEntries(it->second, result, ConfigClass::Race);
				}
			}

			auto type =
				a_actor == Data::IData::GetPlayerRefID() ?
					GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			CopyEntries(GetGlobalData(type), result, ConfigClass::Global);

			return result;
		}

		configNodeOverrideHolderCopy_t configStoreNodeOverride_t::GetNPCCopy(
			Game::FormID a_npc,
			Game::FormID a_race) const
		{
			configNodeOverrideHolderCopy_t result;

			if (auto& npcData = GetNPCData(); !npcData.empty())
			{
				if (auto it = npcData.find(a_npc); it != npcData.end())
				{
					CopyEntries(it->second, result, ConfigClass::NPC);
				}
			}

			if (auto& raceData = GetRaceData(); !raceData.empty())
			{
				if (auto it = raceData.find(a_race); it != raceData.end())
				{
					CopyEntries(it->second, result, ConfigClass::Race);
				}
			}

			auto type =
				a_npc == Data::IData::GetPlayerBaseID() ?
					GlobalConfigType::Player :
                    GlobalConfigType::NPC;

			CopyEntries(GetGlobalData(type), result, ConfigClass::Global);

			return result;
		}

		configNodeOverrideHolderCopy_t configStoreNodeOverride_t::GetRaceCopy(
			Game::FormID     a_race,
			GlobalConfigType a_globtype) const
		{
			configNodeOverrideHolderCopy_t result;

			if (auto& raceData = GetRaceData(); !raceData.empty())
			{
				if (auto it = raceData.find(a_race); it != raceData.end())
				{
					CopyEntries(it->second, result, ConfigClass::Race);
				}
			}

			CopyEntries(GetGlobalData(a_globtype), result, ConfigClass::Global);

			return result;
		}

		configNodeOverrideHolderCopy_t configStoreNodeOverride_t::GetGlobalCopy(
			GlobalConfigType a_globtype) const
		{
			configNodeOverrideHolderCopy_t result;

			CopyEntries(GetGlobalData(a_globtype), result, ConfigClass::Global);

			return result;
		}

		const configNodeOverrideEntryTransform_t* configStoreNodeOverride_t::GetActorTransform(
			Game::FormID             a_actor,
			Game::FormID             a_npc,
			Game::FormID             a_race,
			const stl::fixed_string& a_node,
			holderCache_t&           a_hc) const
		{
			return GetActorConfig<configNodeOverrideEntryTransform_t>(
				a_actor,
				a_npc,
				a_race,
				a_node,
				a_hc);
		}

		const configNodeOverrideEntryPlacement_t* configStoreNodeOverride_t::GetActorPlacement(
			Game::FormID             a_actor,
			Game::FormID             a_npc,
			Game::FormID             a_race,
			const stl::fixed_string& a_node,
			holderCache_t&           a_hc) const
		{
			return GetActorConfig<configNodeOverrideEntryPlacement_t>(
				a_actor,
				a_npc,
				a_race,
				a_node,
				a_hc);
		}

		const configNodeOverrideEntryPhysics_t* configStoreNodeOverride_t::GetActorPhysics(
			Game::FormID             a_actor,
			Game::FormID             a_npc,
			Game::FormID             a_race,
			const stl::fixed_string& a_node,
			holderCache_t&           a_hc) const
		{
			return GetActorConfig<configNodeOverrideEntryPhysics_t>(
				a_actor,
				a_npc,
				a_race,
				a_node,
				a_hc);
		}

		configNodeOverrideHolder_t::configNodeOverrideHolder_t(
			const configNodeOverrideHolderCopy_t& a_rhs)
		{
			__init(a_rhs);
		}

		configNodeOverrideHolder_t::configNodeOverrideHolder_t(
			configNodeOverrideHolderCopy_t&& a_rhs)
		{
			__init(std::move(a_rhs));
		}

		configNodeOverrideHolder_t& configNodeOverrideHolder_t::operator=(
			const configNodeOverrideHolderCopy_t& a_rhs)
		{
			clear();
			__init(a_rhs);

			return *this;
		}

		configNodeOverrideHolder_t& configNodeOverrideHolder_t::operator=(
			configNodeOverrideHolderCopy_t&& a_rhs)
		{
			clear();
			__init(std::move(a_rhs));

			return *this;
		}

		void configNodeOverrideHolder_t::__init(const configNodeOverrideHolderCopy_t& a_rhs)
		{
			for (auto& [i, e] : a_rhs.transformData)
			{
				transformData.emplace(i, e.second);
			}

			for (auto& [i, e] : a_rhs.placementData)
			{
				placementData.emplace(i, e.second);
			}

			for (auto& [i, e] : a_rhs.physicsData)
			{
				physicsData.emplace(i, e.second);
			}

			flags = a_rhs.flags;
		}

		void configNodeOverrideHolder_t::__init(configNodeOverrideHolderCopy_t&& a_rhs)
		{
			for (auto& [i, e] : a_rhs.transformData)
			{
				transformData.emplace(i, std::move(e.second));
			}

			for (auto& [i, e] : a_rhs.placementData)
			{
				placementData.emplace(i, std::move(e.second));
			}

			for (auto& [i, e] : a_rhs.physicsData)
			{
				physicsData.emplace(i, std::move(e.second));
			}

			flags = a_rhs.flags;
		}

		configNodeOverrideHolderCopy_t::configNodeOverrideHolderCopy_t(
			const configNodeOverrideHolder_t& a_rhs,
			ConfigClass                       a_initclass)
		{
			for (auto& [i, e] : a_rhs.transformData)
			{
				transformData.try_emplace(i, a_initclass, e);
			}

			for (auto& [i, e] : a_rhs.placementData)
			{
				placementData.try_emplace(i, a_initclass, e);
			}

			for (auto& [i, e] : a_rhs.physicsData)
			{
				physicsData.try_emplace(i, a_initclass, e);
			}

			flags = a_rhs.flags;
		}

		configNodeOverrideHolderCopy_t::configNodeOverrideHolderCopy_t(
			configNodeOverrideHolder_t&& a_rhs,
			ConfigClass                  a_initclass)
		{
			for (auto& [i, e] : a_rhs.transformData)
			{
				transformData.try_emplace(i, a_initclass, std::move(e));
			}

			for (auto& [i, e] : a_rhs.placementData)
			{
				placementData.try_emplace(i, a_initclass, std::move(e));
			}

			for (auto& [i, e] : a_rhs.physicsData)
			{
				physicsData.try_emplace(i, a_initclass, std::move(e));
			}

			flags = a_rhs.flags;
		}

		configNodeOverrideHolder_t configNodeOverrideHolderCopy_t::copy_cc(
			ConfigClass a_class) const
		{
			configNodeOverrideHolder_t result;

			for (auto& [i, e] : transformData)
			{
				if (e.first == a_class)
				{
					result.transformData.emplace(i, e.second);
				}
			}

			for (auto& [i, e] : placementData)
			{
				if (e.first == a_class)
				{
					result.placementData.emplace(i, e.second);
				}
			}

			for (auto& [i, e] : physicsData)
			{
				if (e.first == a_class)
				{
					result.physicsData.emplace(i, e.second);
				}
			}

			result.flags = flags;

			return result;
		}

		void configNodeOverrideHolderCopy_t::copy_cc(
			ConfigClass                 a_class,
			configNodeOverrideHolder_t& a_dst) const
		{
			a_dst.clear();

			for (auto& [i, e] : transformData)
			{
				if (e.first == a_class)
				{
					a_dst.transformData.emplace(i, e.second);
				}
			}

			for (auto& [i, e] : placementData)
			{
				if (e.first == a_class)
				{
					a_dst.placementData.emplace(i, e.second);
				}
			}

			for (auto& [i, e] : physicsData)
			{
				if (e.first == a_class)
				{
					a_dst.physicsData.emplace(i, e.second);
				}
			}

			a_dst.flags = flags;
		}

	}
}