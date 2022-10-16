#pragma once

namespace IED
{
	namespace Data
	{
		enum class ConfigStoreSerializationFlags : std::uint32_t
		{
			kNone = 0,

			kSlotGlobalPlayer = 1u << 0,
			kSlotGlobalNPC    = 1u << 1,
			kSlotActor        = 1u << 2,
			kSlotNPC          = 1u << 3,
			kSlotRace         = 1u << 4,

			kCustomGlobal = 1u << 5,
			kCustomPlayer = 1u << 6,
			kCustomActor  = 1u << 7,
			kCustomNPC    = 1u << 8,
			kCustomRace   = 1u << 9,

			kNodeOverrideGlobalPlayer = 1u << 10,
			kNodeOverrideGlobalNPC    = 1u << 11,
			kNodeOverridePlayer       = 1u << 12,
			kNodeOverrideActor        = 1u << 13,
			kNodeOverrideNPC          = 1u << 14,
			kNodeOverrideRace         = 1u << 15,

			kConditionalVariables = 1u << 16,

			kAll = static_cast<std::underlying_type_t<ConfigStoreSerializationFlags>>(-1)
		};

		DEFINE_ENUM_CLASS_BITWISE(ConfigStoreSerializationFlags);
	}
}
