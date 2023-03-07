#include "pch.h"

#include "ConfigStoreOutfit.h"

#include "IED/Data.h"

namespace IED
{
	namespace Data
	{
		namespace OM
		{
			auto configStoreOutfit_t::GetNPC(
				Game::FormID a_npc,
				Game::FormID a_race) const noexcept
				-> const data_type&
			{
				if (auto& b = GetNPCData(); !b.empty())
				{
					if (auto it = b.find(a_npc); it != b.end())
					{
						return it->second;
					}
				}

				return GetRace(a_race);
			}

			auto configStoreOutfit_t::GetRace(
				Game::FormID a_race) const noexcept
				-> const data_type&
			{
				if (auto& b = GetRaceData(); !b.empty())
				{
					if (auto it = b.find(a_race); it != b.end())
					{
						return it->second;
					}
				}

				return GetGlobal();
			}

			auto configStoreOutfit_t::GetGlobal() const noexcept
				-> const data_type&
			{
				return GetGlobalData(GlobalConfigType::NPC);
			}
		}
	}
}