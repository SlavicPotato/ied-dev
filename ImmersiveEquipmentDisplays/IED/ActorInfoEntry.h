#pragma once

#include "ConfigData.h"

namespace IED
{
	struct npcInfoEntry_t
	{
		constexpr auto& get_npc_or_template() const noexcept
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
		bool                                     active{ false };
		bool                                     attached{ false };
		bool                                     dead{ false };
		bool                                     hasLoadedState{ false };
		std::uint32_t                            flags{ false };
		std::string                              name;
		Game::FormID                             race;
		Game::FormID                             worldspace;
		std::pair<Game::FormID, std::string>     cell;
		Game::FormID                             cellOwner;
		Game::FormID                             cellLightingTemplate;
		std::optional<TESObjectCELL::CellCoords> cellCoords;
		std::pair<Game::FormID, std::string>     location;
		std::pair<Game::FormID, std::string>     skin;
		std::pair<Game::FormID, std::string>     idle;
		Game::FormID                             package;
		std::pair<Game::FormID, std::string>     furniture;
		std::pair<Game::FormID, Game::FormID>    equipped;
		std::pair<std::string, std::string>      equippedNames;
		std::pair<std::uint8_t, std::uint8_t>    equippedTypes;
		std::pair<Game::FormID, bool>            outfit;
		float                                    weight{ 0.0f };
		std::optional<std::pair<float, float>>   inventoryWeight;
		NiPoint3                                 pos;
		NiPoint3                                 rot;
		ActorState::ActorState1                  state1;
		ActorState::ActorState2                  state2;
		std::uint16_t                            level{ 0 };
		bool                                     pcLevelMult{ false };
		long long                                ts{ 0 };
		float                                    lightLevel{ 0 };
		float                                    directionalAmbientLightLevel{ 0 };

		std::shared_ptr<npcInfoEntry_t> npc;

		inline Game::FormID GetRace() const noexcept
		{
			if (race)
			{
				return race;
			}
			else
			{
				return npc ? npc->race : Game::FormID{};
			}
		}

		inline Game::FormID GetBase() const noexcept
		{
			return npc ? npc->form : Game::FormID{};
		}

		inline Game::FormID GetBaseOrTemplate() const noexcept
		{
			return npc ? npc->get_npc_or_template() : Game::FormID{};
		}

		constexpr auto GetSex() const noexcept
		{
			return npc && npc->female ?
			           Data::ConfigSex::Female :
			           Data::ConfigSex::Male;
		}
	};

	using ActorInfoHolder = stl::unordered_map<Game::FormID, actorInfoEntry_t>;
	using NPCInfoHolder   = stl::unordered_map<Game::FormID, std::shared_ptr<npcInfoEntry_t>>;
}