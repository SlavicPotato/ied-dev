#pragma once

namespace IED
{
	class SkeletonID;

	namespace Data
	{
		struct configSkeletonMatch_t
		{
			[[nodiscard]] bool test(const SkeletonID& a_id) const;

			[[nodiscard]] inline constexpr bool empty() const noexcept
			{
				return ids.empty() && signatures.empty();
			}

			stl::set_sa<std::int32_t>  ids;
			stl::set_sa<std::uint64_t> signatures;
		};
	}
}