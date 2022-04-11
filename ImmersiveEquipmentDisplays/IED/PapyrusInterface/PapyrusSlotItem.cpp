#include "pch.h"

#include "PapyrusCommon.h"
#include "PapyrusSlotItem.h"
#include "PapyrusSlotItemCommon.h"
#include "PapyrusSlotItemImpl.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Slot
		{
			static TESForm* GetSlottedForm(
				StaticFunctionTag*,
				Actor*       a_actor,
				std::int32_t a_slot)
			{
				if (!a_actor)
				{
					return nullptr;
				}

				auto slot = GetSlot(a_slot);
				if (slot < Data::ObjectSlot::kMax)
				{
					return GetSlottedFormImpl(a_actor->formID, slot);
				}
				else
				{
					return nullptr;
				}
			}

			bool Register(VMClassRegistry* a_registry)
			{
				a_registry->RegisterFunction(
					new NativeFunction2<StaticFunctionTag, TESForm*, Actor*, std::int32_t>(
						"GetSlottedForm",
						SCRIPT_NAME,
						GetSlottedForm,
						a_registry));

				return true;
			}
		}
	}
}