#pragma once

namespace IED
{
	class SkeletonID;

	namespace Data
	{
		struct configSkeletonMatch_t
		{
			[[nodiscard]] bool test(const SkeletonID& a_id) const noexcept;

			[[nodiscard]] inline constexpr bool empty() const noexcept
			{
				return ids.empty() && signatures.empty();
			}

			stl::flat_set<std::int32_t>  ids;
			stl::flat_set<std::uint64_t> signatures;
		};
	}
}