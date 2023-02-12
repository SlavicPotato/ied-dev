#pragma once

namespace IED
{
	namespace ALD
	{
		void InitGSPtr();

		bool                 IsExteriorDark(const RE::Sky* a_sky) noexcept;
		bool                 IsActorInDarkInterior(const Actor* a_actor, const RE::Sky* a_sky) noexcept;
		bool                 IsSunAngleLessThan(const RE::Sky* a_sky, float a_angle) noexcept;
		BGSLightingTemplate* GetRoomLightingTemplate(const RE::Sky* a_sky) noexcept;
	}
}