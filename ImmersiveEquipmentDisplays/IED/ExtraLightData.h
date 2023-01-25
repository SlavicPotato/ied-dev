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

		struct extraLightData_t
		{
			static constexpr auto DEFAULT_FLAGS =
				ExtraLightFlags::kNone;

			stl::flag<ExtraLightFlags> flags{ DEFAULT_FLAGS };
			float                      shadowDepthBias{ 0.0f };
			float                      fieldOfView{ 0.0f };
		};

	}
}