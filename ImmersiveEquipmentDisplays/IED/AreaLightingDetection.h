#pragma once

namespace IED
{
	namespace ALD
	{
		static constexpr float DEFAULT_SA_ROUND_TO_NEAREST = 5.0f * std::numbers::pi_v<float> / 180.0f;

		bool                 IsExteriorDark(const RE::Sky* a_sky) noexcept;
		bool                 IsInteriorDark(const Actor* a_actor, const RE::Sky* a_sky, const TESObjectCELL* a_cell) noexcept;
		bool                 IsSunAngleLessThan(const RE::Sky* a_sky, float a_angle) noexcept;
		float                GetRoundedSunAngle(const RE::Sky* a_sky, float a_nearest = DEFAULT_SA_ROUND_TO_NEAREST) noexcept;
		float                GetSunAngle(const RE::Sky* a_sky) noexcept;
		BGSLightingTemplate* GetRoomLightingTemplate(const RE::Sky* a_sky) noexcept;
	}
}