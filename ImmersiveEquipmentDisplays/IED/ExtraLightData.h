#pragma once

namespace IED
{
	namespace Data
	{

		enum class ExtraLightFlags : std::uint32_t
		{
			kNone = 0,

			kTargetSelf         = 1u << 0,
			kDontLightWater     = 1u << 1,
			kDontLightLandscape = 1u << 2
		};

		DEFINE_ENUM_CLASS_BITWISE(ExtraLightFlags);

		struct ExtraLightData
		{
			static constexpr auto DEFAULT_FLAGS =
				ExtraLightFlags::kNone;

			[[nodiscard]] constexpr friend bool operator==(
				const ExtraLightData& a_lhs,
				const ExtraLightData& a_rhs) noexcept
			{
				return a_lhs.flags == a_rhs.flags && a_lhs.shadowDepthBias == a_rhs.shadowDepthBias && a_lhs.fieldOfView == a_rhs.fieldOfView;
			}
			
			[[nodiscard]] constexpr friend bool operator!=(
				const ExtraLightData& a_lhs,
				const ExtraLightData& a_rhs) noexcept
			{
				return !(a_lhs.flags == a_rhs.flags && a_lhs.shadowDepthBias == a_rhs.shadowDepthBias && a_lhs.fieldOfView == a_rhs.fieldOfView);
			}

			stl::flag<ExtraLightFlags> flags{ DEFAULT_FLAGS };
			float                      shadowDepthBias{ 0.0f };
			float                      fieldOfView{ 0.0f };
		};

	}
}