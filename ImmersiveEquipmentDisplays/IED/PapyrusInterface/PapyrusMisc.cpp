#include "pch.h"

#include "PapyrusCommon.h"
#include "PapyrusMisc.h"
#include "PapyrusMiscImpl.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Misc
		{
			static TESForm* GetLastEquippedForm(
				StaticFunctionTag*,
				Actor*       a_actor,
				std::int32_t a_bipedObject,
				bool         a_trackedActor,
				bool         a_ignoreIfOccupied)
			{
				if (!a_actor)
				{
					return nullptr;
				}

				auto bip = static_cast<BIPED_OBJECT>(a_bipedObject);
				if (bip >= BIPED_OBJECT::kTotal)
				{
					return nullptr;
				}

				return GetLastEquippedFormImpl(
					a_actor->formID,
					bip,
					a_trackedActor,
					a_ignoreIfOccupied);
			}

			bool Register(VMClassRegistry* a_registry)
			{
				a_registry->RegisterFunction(
					new NativeFunction4<StaticFunctionTag, TESForm*, Actor*, std::int32_t, bool, bool>(
						"GetLastEquippedForm",
						SCRIPT_NAME,
						GetLastEquippedForm,
						a_registry));

				return true;
			}
		}
	}
}