#pragma once

namespace IED
{
	class SkeletonID;

	namespace Data
	{
		struct configSkeletonMatch_t
		{
			[[nodiscard]] bool test(const SkeletonID& a_id) const noexcept;
			[[nodiscard]] bool test(const SkeletonID& a_id, NiNode* a_root) const noexcept;

			[[nodiscard]] constexpr bool empty() const noexcept
			{
				return ids.empty() && signatures.empty();
			}

			stl::cache_aligned::flat_set<std::int32_t>    ids;
			stl::cache_aligned::flat_set<std::uint64_t>   signatures;
			bool                                          matchAny{ false };
		};
	}
}