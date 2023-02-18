#pragma once

namespace IED
{
	class Controller;

	struct actorWeaponNodeInfo_t
	{
		struct Entry
		{
			stl::fixed_string name;
			stl::fixed_string parent;
			std::uint16_t     numChildren;
			bool              visible;
		};

		void Update(Actor* a_actor, const Controller& a_controller);

		stl::vector<Entry> data;
	};
}