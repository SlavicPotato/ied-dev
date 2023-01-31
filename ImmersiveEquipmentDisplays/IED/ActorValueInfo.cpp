#include "pch.h"

#include "ActorValueInfo.h"

namespace IED
{
	void actorValueInfo_t::Update(Actor* a_actor)
	{
		for (std::uint32_t i = 0; i < data.size(); i++)
		{
			data[i] = a_actor->GetActorValue(static_cast<RE::ActorValue>(i));
		}
	}
}