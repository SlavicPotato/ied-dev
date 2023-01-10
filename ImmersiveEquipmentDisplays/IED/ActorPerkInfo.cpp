#include "pch.h"

#include "ActorPerkInfo.h"

#include "FormCommon.h"

namespace IED
{
	void actorPerkInfo_t::Update(Actor* a_actor)
	{
		data.clear();

		auto npc = a_actor->GetActorBase();
		if (!npc)
		{
			return;
		}

		if (a_actor->processManager &&
		    a_actor->processManager->middleProcess)
		{
			if (a_actor == *g_thePlayer)
			{
				for (auto& e : static_cast<PlayerCharacter*>(a_actor)->addedPerks)
				{
					if (auto perk = e->perk)
					{
						data.try_emplace(
							perk->formID,
							e->currentRank,
							IFormCommon::GetFormName(perk));
					}
				}
			}

			for (std::uint32_t i = 0; i < npc->perkCount; i++)
			{
				const auto& e = npc->perks[i];

				if (auto perk = e.perk)
				{
					data.try_emplace(
						perk->formID,
						e.currentRank,
						IFormCommon::GetFormName(perk));
				}
			}
		}

	}
}