#pragma once

namespace IED
{
	struct actorActiveEffectInfo_t
	{
		struct formEntry_t
		{
			Game::FormID id;
			std::uint8_t type;
			std::string  name;
		};

		struct entry_t
		{
			entry_t(ActiveEffect *a_effect);

			formEntry_t effect;
			formEntry_t spell;
			formEntry_t source;
		};

		void Update(Actor* a_actor);

		stl::vector<entry_t> data;

	};
}