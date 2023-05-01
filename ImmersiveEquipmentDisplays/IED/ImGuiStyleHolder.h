#pragma once

namespace IED
{
	namespace Data
	{
		enum class ImGuiStyleHolderFlags : std::uint32_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(ImGuiStyleHolderFlags);

		class ImGuiStyleHolder
		{
		public:
			stl::flag<ImGuiStyleHolderFlags> flags{ ImGuiStyleHolderFlags::kNone };
			ImGuiStyle                       style;
		};
	}
}