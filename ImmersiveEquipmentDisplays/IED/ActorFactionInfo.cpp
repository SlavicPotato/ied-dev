#include "pch.h"

#include "ActorFactionInfo.h"

#include "FormCommon.h"

namespace IED
{
	void actorFactionInfo_t::Update(Actor* a_actor)
	{
		data.clear();

		auto npc = a_actor->GetActorBase();
		if (!npc)
		{
			return;
		}

		if (const auto* extraFactionChanges = a_actor->extraData.Get<ExtraFactionChanges>())
		{
			for (const auto& info : extraFactionChanges->factions)
			{
				if (auto faction = info.faction)
				{
					data.try_emplace(
						faction->formID,
						info.rank,
						IFormCommon::GetFormName(faction),
						faction->data.flags,
						faction->IsLikelyMerchantFaction());
				}
			}
		}

		for (const auto& info : npc->factions)
		{
			if (auto faction = info.faction)
			{
				data.try_emplace(
					faction->formID,
					info.rank,
					IFormCommon::GetFormName(faction),
					faction->data.flags,
					faction->IsLikelyMerchantFaction());
			}
		}
	}
}