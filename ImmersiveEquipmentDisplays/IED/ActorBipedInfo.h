#pragma once

namespace IED
{
	struct actorBipedInfo_t
	{
		struct Entry
		{
			Game::FormID                                item;
			Game::FormID                                addon;
			std::optional<std::pair<std::string, bool>> geometry;
		};

		void Update(Actor* a_actor);

		std::array<Entry, stl::underlying(BIPED_OBJECT::kTotal)> data;
	};
}