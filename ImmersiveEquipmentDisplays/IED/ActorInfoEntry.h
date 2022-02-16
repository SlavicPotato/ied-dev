#pragma once

#include "ConfigCommon.h"

namespace IED
{
	struct npcInfoEntry_t
	{
		Game::FormID  form;
		std::string   name;
		bool          active{ false };
		Game::FormID  race;
		std::uint32_t flags;
		float         weight;
		bool          female;
	};

	struct actorInfoEntry_t
	{
		bool         active{ false };
		bool         attached{ false };
		std::string  name;
		Game::FormID race;
		float        weight;
		long long    ts{ 0 };

		std::shared_ptr<npcInfoEntry_t> npc;

		inline auto GetRace() const noexcept
		{
			if (race)
			{
				return race;
			}
			else
			{
				return npc ? npc->race : 0;
			}
		}

		inline auto GetBase() const noexcept
		{
			return npc ? npc->form : 0;
		}

		inline constexpr auto GetSex() const noexcept
		{
			return npc && npc->female ?
                       Data::ConfigSex::Female :
                       Data::ConfigSex::Male;
		}
	};

	using ActorInfoHolder = std::unordered_map<Game::FormID, actorInfoEntry_t>;
	using NPCInfoHolder   = std::unordered_map<Game::FormID, std::shared_ptr<npcInfoEntry_t>>;
}