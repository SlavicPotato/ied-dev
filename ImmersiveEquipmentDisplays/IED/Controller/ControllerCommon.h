#pragma once

namespace IED
{
	enum class ControllerUpdateFlags : std::uint32_t
	{
		kNone = 0,

		kPlayEquipSound               = 1u << 2,
		kVisibilitySounds             = 1u << 3,
		kImmediateTransformUpdate     = 1u << 4,
		kWantEffectShaderConfigUpdate = 1u << 5,
		kUseCachedParams              = 1u << 6,
		kFailVariableCondition        = 1u << 7,
		kFromProcessorTask            = 1u << 8,
		kDestroyed                    = 1u << 9,

		kAll      = kPlayEquipSound,
		kSoundAll = kPlayEquipSound | kVisibilitySounds
	};

	DEFINE_ENUM_CLASS_BITWISE(ControllerUpdateFlags);
}