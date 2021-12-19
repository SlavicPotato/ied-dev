#pragma once

namespace IED
{
	namespace Papyrus
	{
		static inline constexpr bool ValidateString(const BSFixedString& a_str)
		{
			return a_str.data != nullptr && a_str.data[0] != 0;
		}

		stl::fixed_string GetKey(
			const BSFixedString& a_key);

	}
}