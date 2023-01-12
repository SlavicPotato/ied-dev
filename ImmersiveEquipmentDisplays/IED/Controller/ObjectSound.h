#pragma once

#include "SoundDescriptor.h"

namespace IED
{
	struct ObjectSound
	{
		BSSoundHandle   handle;
		SoundDescriptor desc;
	};
}