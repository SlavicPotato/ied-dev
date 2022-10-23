#include "IED/ConfigData.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Misc
		{
			TESForm* GetLastEquippedFormImpl(
				Game::FormID a_actor,
				BIPED_OBJECT a_bipedObject,
				bool         a_trackedActor,
				bool         a_ignoreIfOccupied);
		}
	}
}