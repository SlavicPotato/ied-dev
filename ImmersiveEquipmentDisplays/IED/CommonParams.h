#pragma once

#include "FormHolder.h"
#include "FormCommon.h"

namespace IED
{
	struct CommonParams
	{
		Actor* actor;
		mutable SetObjectWrapper<TESFurniture*> furniture;
		mutable SetObjectWrapper<Game::ObjectRefHandle> furnHandle;
		mutable SetObjectWrapper<bool> layingDown;

		SKMP_FORCEINLINE bool get_using_furniture() const
		{
			if (!furnHandle)
			{
				furnHandle = Game::ObjectRefHandle{};

				if (auto pm = actor->processManager)
				{
					if (actor->actorState.actorState1.sitSleepState == ActorState::SIT_SLEEP_STATE::kIsSitting ||
					    actor->actorState.actorState1.sitSleepState == ActorState::SIT_SLEEP_STATE::kIsSleeping)
					{
						furnHandle = pm->GetOccupiedFurniture();
					}
				}
			}

			return *furnHandle != Game::ObjectRefHandle{};
		}

		SKMP_FORCEINLINE auto get_furniture() const
		{
			if (!furniture)
			{
				if (get_using_furniture())
				{
					NiPointer<TESObjectREFR> ref;
					if (furnHandle->Lookup(ref))
					{
						if (auto base = ref->baseForm)
						{
							furniture = base->As<TESFurniture>();
							return *furniture;
						}
					}
				}

				furniture = nullptr;
			}

			return *furniture;
		}

		SKMP_FORCEINLINE bool get_laying_down() const
		{
			if (!layingDown)
			{
				layingDown = false;

				if (auto furn = get_furniture())
				{
					if (auto kw = FormHolder::GetSingleton().layDown)
					{
						if (IFormCommon::HasKeyword(furn, kw))
						{
							return *(layingDown = true);
						}
					}

					layingDown = furn->furnFlags.test(
						TESFurniture::ActiveMarker::kMustExitToTalk |
						TESFurniture::ActiveMarker::kUnknown31);
				}
			}

			return *layingDown;
		}
	};
}