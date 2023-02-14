#include "pch.h"

#include "ObjectLight.h"

#include "IED/ReferenceLightController.h"

namespace IED
{
	void ObjectLight::Cleanup(NiNode* a_root) noexcept
	{
		if (niObject)
		{
			ReferenceLightController::CleanupLights(a_root);
		}
	}
}