#pragma once

namespace IED
{
	namespace Papyrus
	{
		inline static constexpr auto SCRIPT_NAME = "IED";

		[[nodiscard]] static inline constexpr bool ValidateString(const BSFixedString& a_str) noexcept
		{
			return !a_str.empty();
		}

		stl::fixed_string GetKey(const BSFixedString& a_key);

	}
}