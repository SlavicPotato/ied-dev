#pragma once

namespace IED
{
	enum class ExportFlags : std::uint32_t
	{
		kNone          = 0,
		kKeepGenerated = 1u << 0,
	};

	DEFINE_ENUM_CLASS_BITWISE(ExportFlags);

}