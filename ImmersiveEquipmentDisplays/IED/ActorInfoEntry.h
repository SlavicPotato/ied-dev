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
		long long     ts{ 0 };
	};

	struct actorInfoEntry_t
	{
		bool                                  active{ false };
		bool                                  attached{ false };
		bool                                  dead{ false };
		bool                                  hasLoadedState{ false };
		std::uint32_t                         flags{ false };
		std::string                           name;
		Game::FormID                          race;
		Game::FormID                          worldspace;
		std::pair<Game::FormID, std::string>  cell;
		std::pair<Game::FormID, std::string>  location;
		std::pair<Game::FormID, std::string>  skin;
		std::pair<Game::FormID, std::string>  idle;
		Game::FormID                          package;
		std::pair<Game::FormID, std::string>  furniture;
		std::pair<Game::FormID, Game::FormID> equipped;
		std::pair<std::string, std::string>   equippedNames;
		std::pair<std::uint8_t, std::uint8_t> equippedTypes;
		std::pair<Game::FormID, bool>         outfit;
		float                                 weight{ 0.0f };
		NiPoint3                              pos;
		NiPoint3                              rot;
		long long                             ts{ 0 };

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