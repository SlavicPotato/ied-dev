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
			bool                                        animated{ false };
		};

		void Update(Actor* a_actor);

		using array_type = std::array<Entry, stl::underlying(BIPED_OBJECT::kTotal)>;

		std::unique_ptr<array_type> data;
	};
}