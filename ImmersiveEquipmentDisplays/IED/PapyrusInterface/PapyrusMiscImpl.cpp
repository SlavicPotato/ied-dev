#include "pch.h"

#include "PapyrusMiscImpl.h"

#include "IED/Controller/Controller.h"
#include "IED/Main.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Misc
		{
			static BipedSlotData* GetBipedSlotData(
				const Controller* a_controller,
				Game::FormID      a_actor,
				bool              a_trackedActor)
			{
				if (a_trackedActor)
				{
					auto& data = a_controller->GetObjects();

					auto it = data.find(a_actor);
					if (it != data.end())
					{
						return it->second.GetBipedSlotData().get();
					}
				}
				else
				{
					auto& data = a_controller->GetBipedDataCache().data();

					auto it = data.find(a_actor);
					if (it != data.end())
					{
						return it->second.data.get();
					}
				}

				return {};
			}

			TESForm* GetLastEquippedFormImpl(
				Game::FormID a_actor,
				BIPED_OBJECT a_bipedObject,
				bool         a_trackedActor,
				bool         a_ignoreIfOccupied)
			{
				const auto* controller = Initializer::GetController();

				const stl::lock_guard lock(controller->GetLock());

				auto data = GetBipedSlotData(controller, a_actor, a_trackedActor);
				if (!data)
				{
					return nullptr;
				}

				const auto& v = data->get(a_bipedObject);

				if (a_ignoreIfOccupied  &&
				    a_trackedActor && 
					v.occupied)
				{
					return nullptr;
				}

				for (auto& e : v.forms)
				{
					if (!e)
					{
						continue;
					}

					if (auto form = e.Lookup())
					{
						return form;
					}
				}

				return nullptr;
			}
		}
	}
}