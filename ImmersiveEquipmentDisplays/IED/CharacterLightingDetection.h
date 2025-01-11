#pragma once

namespace IED
{
	class CLD
	{
	public:
		static float GetLightLevel(Actor* a_actor) noexcept;

		static constexpr float D3 = 1.0f / 3.0f;

		static constexpr float get_diffuse_magnitude(
			const NiPointer<NiLight>& a_light,
			float                     a_mul) noexcept
		{
			const auto  fade       = a_light->fade;
			const auto& diffuseCol = a_light->diffuse;
			return (diffuseCol.r * fade +
			        diffuseCol.g * fade +
			        diffuseCol.b * fade) *
			       (a_mul * D3);
		}
	private:
		using isActorLitByLight_t = bool (*)(
			void*              a_this,  // unused
			RE::BSShadowLight* a_light,
			Actor*             a_actor,
			const NiPoint3&    a_atPos) noexcept;

		/* 
		* Returns true if light emitted by 'a_light' is hitting the actor at 'a_pos'
		*/
		inline static const auto IsLightHittingActor = IAL::Address<isActorLitByLight_t>(41661, 0);

		static bool FilterLight(
			const Actor*       a_actor,
			const RE::BSLight* a_light) noexcept;

		static float GetLightDiffuseMagnitude(
			const RE::BSLight* a_light,
			const NiPoint3&    a_pos) noexcept;

		static float GetLightMagnitude(
			Actor*               a_actor,
			RE::ShadowSceneNode* a_shadowSceneNode,
			const NiPoint3&      a_pos,
			std::uint64_t        a_filterFlags) noexcept;

		static float GetLightMagnitude(
			Actor*                         a_actor,
			RE::BSShaderPropertyLightData* a_spld,
			const NiPoint3&                a_pos) noexcept;


	};
}