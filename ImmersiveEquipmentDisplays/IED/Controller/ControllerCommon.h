#pragma once

namespace IED
{
	enum class ControllerUpdateFlags : std::uint32_t
	{
		kNone = 0,

		kPlaySound                    = 1u << 2,
		kVisibilitySounds             = 1u << 3,
		kImmediateUpdateTransforms    = 1u << 4,
		kWantEffectShaderConfigUpdate = 1u << 5,

		kAll      = kPlaySound,
		kSoundAll = kPlaySound | kVisibilitySounds
	};

	DEFINE_ENUM_CLASS_BITWISE(ControllerUpdateFlags);
}