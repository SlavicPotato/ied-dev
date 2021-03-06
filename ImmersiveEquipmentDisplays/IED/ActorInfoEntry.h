#pragma once

#include "ConfigData.h"

namespace IED
{
	struct npcInfoEntry_t
	{
		inline constexpr auto& get_npc_or_template() const noexcept
		{
			return nontemp;
		}

		Game::FormID  form;
		Game::FormID  templ;
		Game::FormID  nontemp;
		std::string   name;
		bool          active{ false };
		Game::FormID  race;
		std::uint32_t flags{ 0 };
		float         weight{ 0.0f };
		bool          female{ false };
	};

	struct actorInfoEntry_t
	{
		bool         active{ false };
		bool         attached{ false };
		std::string  name;
		Game::FormID race;
		Game::FormID worldspace;
		Game::FormID cell;
		Game::FormID skin;
		float        weight{ 0.0f };
		long long    ts{ 0 };

		std::shared_ptr<npcInfoEntry_t> npc;

		inline Game::FormID GetRace() const noexcept
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

		inline Game::FormID GetBase() const noexcept
		{
			return npc ? npc->form : 0;
		}

		inline Game::FormID GetBaseOrTemplate() const noexcept
		{
			return npc ? npc->get_npc_or_template() : 0;
		}

		inline constexpr auto GetSex() const noexcept
		{
			return npc && npc->female ?
			           Data::ConfigSex::Female :
                       Data::ConfigSex::Male;
		}
	};

	using ActorInfoHolder = stl::unordered_map<Game::FormID, actorInfoEntry_t>;
	using NPCInfoHolder   = stl::unordered_map<Game::FormID, std::shared_ptr<npcInfoEntry_t>>;
}