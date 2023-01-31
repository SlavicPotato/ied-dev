#pragma once

namespace IED
{
	struct actorValueInfo_t
	{
		void Update(Actor* a_actor);

		std::array<float, 23> data;
	};
}