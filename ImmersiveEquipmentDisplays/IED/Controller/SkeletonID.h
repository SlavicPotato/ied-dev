#pragma once

namespace IED
{
	class SkeletonID
	{
	public:
		SkeletonID(NiNode* a_root);

		[[nodiscard]] inline constexpr auto& get_id() const noexcept
		{
			return m_id;
		}
		
		[[nodiscard]] inline constexpr auto& get_version() const noexcept
		{
			return m_xpmse_version;
		}

	private:
		std::optional<std::int32_t> m_id;
		std::optional<float>        m_xpmse_version;
	};
}