#pragma once

#include <ext/Hash.h>

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

		SkeletonID(NiNode* a_root);

		[[nodiscard]] inline constexpr auto& id() const noexcept
		{
			return m_id;
		}

		[[nodiscard]] inline constexpr auto& bsx_flags() const noexcept
		{
			return m_bsx;
		}

		[[nodiscard]] inline constexpr auto& xp_version() const noexcept
		{
			return m_xpmse_version;
		}

		[[nodiscard]] inline constexpr auto& signature() const noexcept
		{
			return m_signature;
		}

		[[nodiscard]] inline constexpr auto& presence_flags() const noexcept
		{
			return m_pflags;
		}

	private:
		std::optional<std::int32_t>              m_id;
		std::optional<stl::flag<BSXFlags::Flag>> m_bsx;
		std::optional<float>                     m_xpmse_version;
		stl::flag<PresenceFlags>                 m_pflags{ PresenceFlags::kNone };
		std::uint64_t                            m_signature{ hash::fnv1::fnv_offset_basis };
	};

	DEFINE_ENUM_CLASS_BITWISE(SkeletonID::PresenceFlags);
}