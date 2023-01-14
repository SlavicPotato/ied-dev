#pragma once

namespace IED
{
	namespace Papyrus
	{
		static constexpr auto SCRIPT_NAME = "IED";

		[[nodiscard]] constexpr bool ValidateString(const BSFixedString& a_str) noexcept
		{
			return !a_str.empty();
		}

		[[nodiscard]] stl::fixed_string GetKey(const BSFixedString& a_key);
		[[nodiscard]] bool              ValidateKey(const stl::fixed_string& a_key);

	}
}