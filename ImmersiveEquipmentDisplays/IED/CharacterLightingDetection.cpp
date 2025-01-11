#include "pch.h"

#include <ext/BSShaderPropertyLightData.h>
#include <ext/ImageSpaceManager.h>
#include <ext/TES.h>

#include "CharacterLightingDetection.h"

#include "ReferenceLightController.h"

namespace IED
{
	static constexpr float AMB_MULT = 3.0f;

	bool CLD::FilterLight(
		const Actor*       a_actor,
		const RE::BSLight* a_light) noexcept
	{
		// ignore IED-attached lights
		if (ReferenceLightController::GetSingleton().HasBSLight(a_actor->formID, a_light))
		{
			return false;
		}

		// ignore magic caster lights
		for (const auto* const e : a_actor->magicCasters)
		{
			if (e && e->light.get() == a_light)
			{
				return false;
			}
		}

		return true;
	}

	float CLD::GetLightDiffuseMagnitude(
		const RE::BSLight* a_light,
		const NiPoint3&    a_pos) noexcept
	{
		const auto& niLight = a_light->light;

		float c;

		if (a_light->pointLight)
		{
			const float dx = a_pos.x - niLight->m_worldTransform.pos.x;
			const float dy = a_pos.y - niLight->m_worldTransform.pos.y;
			const float dz = a_pos.z - niLight->m_worldTransform.pos.z;
			const float m  = std::sqrtf(dx * dx + dy * dy + dz * dz) * 1.0f / niLight->radius.x;
			const float n  = std::clamp(m, 0.0f, 1.0f);
			c              = 1.0f - n * n;
		}
		else
		{
			c = 1.0f;
		}

		return get_diffuse_magnitude(niLight, c);
	}

	float CLD::GetLightLevel(Actor* a_actor) noexcept
	{
		auto shadowSceneNode  = RE::ShadowSceneNode::GetSingleton();
		auto middleHigh       = a_actor->processManager ? a_actor->processManager->middleProcess : nullptr;
		auto lightingProperty = middleHigh ? middleHigh->lightingProperty.get() : nullptr;
		if (shadowSceneNode == nullptr ||
		    lightingProperty == nullptr)
		{
			return 0.0f;
		}

		const bool inInterior = a_actor->parentCell && a_actor->parentCell->IsInterior();

		auto sunLight = shadowSceneNode->sunLight;

		const auto pos = a_actor->GetTorsoPosition();

		float       lm;
		const float sunLightDiffuseMag = sunLight ? get_diffuse_magnitude(sunLight->light, 0.15f) : 0.0f;

		if (a_actor == *g_thePlayer)
		{
			lm = GetLightMagnitude(a_actor, shadowSceneNode, pos);

			auto shadowDirLight = reinterpret_cast<RE::BSShadowLight*>(shadowSceneNode->shadowDirLight);

			auto tes = RE::TES::GetSingleton();
			if (!tes->interiorCell &&
			    (!tes->worldSpace || !tes->worldSpace->worldspaceFlags.test(TESWorldSpace::Flag::kFixedDimensions)) &&
			    shadowDirLight &&
			    !IsLightHittingActor(nullptr, shadowDirLight, a_actor, pos))
			{
				sunLight = 0;
			}
		}
		else
		{
			lm = GetLightMagnitude(a_actor, lightingProperty->lightData, pos);
		}

		if (inInterior || sunLight)
		{
			return lm * 0.3f + RE::ImageSpaceManager::GetSingleton()->data.baseData.hdr.sunlightScale * sunLightDiffuseMag;
		}
		else
		{
			return lm * 0.3f;
		}
	}

	float CLD::GetLightMagnitude(
		Actor*               a_actor,
		RE::ShadowSceneNode* a_shadowSceneNode,
		const NiPoint3&      a_pos) noexcept
	{
		float result = 0.0f;

		for (const auto& e : a_shadowSceneNode->unk130)
		{
			if (e)
			{
				if (FilterLight(a_actor, e.get()))
				{
					result += GetLightDiffuseMagnitude(e.get(), a_pos);
				}
			}
		}

		for (const auto& e : a_shadowSceneNode->unk148)
		{
			if (e)
			{
				if (FilterLight(a_actor, e.get()) && 
					IsLightHittingActor(nullptr, e.get(), a_actor, a_pos))
				{
					result += GetLightDiffuseMagnitude(e.get(), a_pos);
				}
			}
		}

		return result;
	}

	float CLD::GetLightMagnitude(
		Actor*                         a_actor,
		RE::BSShaderPropertyLightData* a_spld,
		const NiPoint3&                a_pos) noexcept
	{
		float result = 0.0f;

		if (a_spld != nullptr)
		{
			for (const auto* const e : a_spld->lights)
			{
				if (e != nullptr)
				{
					if (FilterLight(a_actor, e))
					{
						result += GetLightDiffuseMagnitude(e, a_pos);
					}
				}
			}
		}

		return result;
	}
}