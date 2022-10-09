#pragma once

namespace IED
{
	struct actorFactionInfo_t
	{
		struct entry_t
		{
			std::int8_t rank;
			std::string name;
		};

		void Update(Actor *a_actor);

		stl::map<Game::FormID, entry_t> data;
	};
}