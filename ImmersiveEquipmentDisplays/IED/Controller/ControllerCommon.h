#pragma once

namespace IED
{
	enum class ControllerUpdateFlags : std::uint32_t
	{
		kNone = 0,

		kPlaySound                    = 1u << 2,
		kVisibilitySounds             = 1u << 3,
		kImmediateTransformUpdate     = 1u << 4,
		kWantEffectShaderConfigUpdate = 1u << 5,
		kUseCachedParams              = 1u << 6,
		kFailVariableCondition        = 1u << 7,
		kFromProcessorTask            = 1u << 8,

		kAll      = kPlaySound,
		kSoundAll = kPlaySound | kVisibilitySounds
	};

	DEFINE_ENUM_CLASS_BITWISE(ControllerUpdateFlags);
}