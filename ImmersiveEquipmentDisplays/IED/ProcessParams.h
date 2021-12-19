#pragma once

#include "ConfigOverride.h"
#include "Controller/ControllerCommon.h"
#include "Controller/ObjectManagerData.h"
#include "FormCommon.h"
#include "FormHolder.h"
#include "Inventory.h"
#include "CommonParams.h"

namespace IED
{
	struct processState_t :
		CommonParams
	{
		bool update{ false };
		bool effectShadersReset{ false };
	};

	struct processParams_t
	{
		NiNode* const root;
		NiNode* const npcroot;
		Actor* const actor;
		const Game::ObjectRefHandle handle;
		ActorObjectHolder& objects;
		TESNPC* const npc;
		EntryDataList* const dataList;
		TESRace* const race;
		const Data::ConfigSex configSex;
		const stl::flag<ControllerUpdateFlags> flags;
		ItemCandidateCollector collector;
		processState_t state;
	};

}  // namespace IED