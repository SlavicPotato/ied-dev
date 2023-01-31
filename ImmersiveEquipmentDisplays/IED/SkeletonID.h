#pragma once

namespace IED
{
	class SkeletonID
	{
	public:
		enum class PresenceFlags : std::uint32_t
		{
			kNone = 0,

			kHasNPCRootNode      = 1u << 0,
			kHasBoneLODExtraData = 1u << 1,
			kHasBoundExtraData   = 1u << 2,
		};

		SkeletonID(NiNode* a_root) noexcept;

		[[nodiscard]] constexpr auto& id() const noexcept
		{
			return m_id;
		}

		[[nodiscard]] constexpr auto& bsx_flags() const noexcept
		{
			return m_bsx;
		}

		[[nodiscard]] constexpr auto& xp_version() const noexcept
		{
			return m_xpmse_version;
		}

		[[nodiscard]] constexpr auto& signature() const noexcept
		{
			return m_signature;
		}

		[[nodiscard]] constexpr auto& presence_flags() const noexcept
		{
			return m_pflags;
		}

	private:
		std::optional<std::int32_t>              m_id;
		std::optional<stl::flag<BSXFlags::Flag>> m_bsx;
		std::optional<float>                     m_xpmse_version;
		stl::flag<PresenceFlags>                 m_pflags{ PresenceFlags::kNone };
		std::uint64_t                            m_signature;
	};

	DEFINE_ENUM_CLASS_BITWISE(SkeletonID::PresenceFlags);
}