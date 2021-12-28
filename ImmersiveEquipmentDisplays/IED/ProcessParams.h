#pragma once

#include "CommonParams.h"
#include "ConfigOverride.h"
#include "Controller/ControllerCommon.h"
#include "Controller/ObjectManagerData.h"
#include "FormCommon.h"
#include "FormHolder.h"
#include "Inventory.h"

namespace IED
{
	enum class ProcessStateUpdateFlags : std::uint8_t
	{
		kNone = 0,

		kMenuUpdate = 1ui8 << 0,
		kForceUpdate = 1ui8 << 1,

		kUpdateMask = kMenuUpdate | kForceUpdate,

		kEffectShadersReset = 1ui8 << 2
	};

	DEFINE_ENUM_CLASS_BITWISE(ProcessStateUpdateFlags);

	struct processState_t :
		CommonParams
	{
		stl::flag<ProcessStateUpdateFlags> flags{ ProcessStateUpdateFlags::kNone };
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