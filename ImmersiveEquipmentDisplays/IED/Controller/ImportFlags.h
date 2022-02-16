#pragma once

namespace IED
{
	enum class ImportFlags : std::uint32_t
	{
		kNone           = 0,
		kMerge          = 1u << 0,
		kEraseTemporary = 1u << 1
	};

	DEFINE_ENUM_CLASS_BITWISE(ImportFlags);

}