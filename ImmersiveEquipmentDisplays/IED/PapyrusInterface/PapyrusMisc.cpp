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
			namespace detail
			{
				[[nodiscard]] constexpr auto translate_biped_object(
					Actor*       a_actor,
					BIPED_OBJECT a_object) noexcept
				{
					if (a_object < BIPED_OBJECT::kTotal)
					{
						return a_object;
					}

					switch (a_object)
					{
					case BIPED_OBJECT::kRaceHead:
						return a_actor->GetHeadBipedObject();
					case BIPED_OBJECT::kRaceHair:
						return a_actor->GetHairBipedObject();
					case BIPED_OBJECT::kRaceShield:
						return a_actor->GetShieldBipedObject();
					case BIPED_OBJECT::kRaceBody:
						return a_actor->GetBodyBipedObject();
					default:
						return BIPED_OBJECT::kNone;
					}
				}
			}

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

				const auto bip = detail::translate_biped_object(
					a_actor,
					static_cast<BIPED_OBJECT>(a_bipedObject));

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