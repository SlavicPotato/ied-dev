#include "pch.h"

#include "ObjectLight.h"

#include "IED/ReferenceLightController.h"

namespace IED
{
	void ObjectLight::Cleanup(NiNode* a_object) noexcept
	{
		if (!niObject)
		{
			return;
		}

		if (sound.IsValid())
		{
			sound.FadeOutAndRelease(100ui16);
		}

		ReferenceLightController::CleanupLights(a_object);
	}
}