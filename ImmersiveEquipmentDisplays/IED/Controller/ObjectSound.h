#pragma once

#include "SoundDescriptor.h"

namespace IED
{
	struct ObjectSound
	{
		BSSoundHandle           handle;
		BGSSoundDescriptorForm* form{ nullptr };
	};
}