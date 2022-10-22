#include "pch.h"

#include "PapyrusSlotItemImpl.h"

#include "IED/Controller/Controller.h"
#include "IED/Controller/ObjectManagerData.h"
#include "IED/Main.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Slot
		{
			TESForm* GetSlottedFormImpl(Game::FormID a_actor, Data::ObjectSlot a_slot)
			{
				const auto* controller = Initializer::GetController();

				stl::scoped_lock lock(controller->GetLock());

				auto& objects = controller->GetObjects();

				auto it = objects.find(a_actor);
				if (it != objects.end())
				{
					return it->second.GetSlot(a_slot).GetFormIfActive();
				}
				else
				{
					return nullptr;
				}
			}
		}
	}
}