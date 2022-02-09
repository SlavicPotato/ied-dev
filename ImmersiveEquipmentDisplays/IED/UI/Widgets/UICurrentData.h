#pragma once

namespace IED
{
	namespace UI
	{
		template <class Th, class Td>
		struct UICurrentData
		{
			Th handle{};
			Td* data{ nullptr };

			[[nodiscard]] inline constexpr explicit operator bool() const noexcept
			{
				return static_cast<bool>(data);
			}
		};

	}
}