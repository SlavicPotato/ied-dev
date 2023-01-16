#include "pch.h"

#include "EngineExtensions.h"
#include "ReferenceLightController.h"
#include "StringHolder.h"

namespace IED
{
	ReferenceLightController ReferenceLightController::m_Instance;

	namespace detail
	{
		static constexpr RE::LIGHT_CREATE_PARAMS make_params(
			const TESObjectLIGH* a_lightForm) noexcept
		{
			RE::LIGHT_CREATE_PARAMS params;

			params.portalStrict = a_lightForm->data.flags.test_any(TES_LIGHT_FLAGS::kPortalStrict);
			params.affectLand   = true;  // EngineFixes patch (always true for chars), default is (a_actor->flags & kTESObjectREFRFlag_DoesntLightLandscape) == 0
			params.lightsWater  = true;  // (a_actor->flags & TESObjectREFR::kFlag_DoesnLightWater) == 0;
			params.neverFades   = true;  // !a_refr->IsHeadingMarker();
			params.unk00        = true;  // arg4 in the original func

			params.data3D.lensFlareRenderData = static_cast<RE::BSLensFlareRenderData*>(a_lightForm->lensFlare);

			if (a_lightForm->data.flags.test_any(TES_LIGHT_FLAGS::kShadowFlags))
			{
				params.shadow          = true;
				params.falloffExponent = a_lightForm->data.fallofExponent;

				if (a_lightForm->data.flags.test_any(TES_LIGHT_FLAGS::kSpotShadow))
				{
					params.shadowAngle = 0.0f;  // 0 if base != TESObjectLIGH (TESObjectREFR::GetLightExtraFOV(a_refr) * 0.017453292);
				}
				else if (a_lightForm->data.flags.test_any(TES_LIGHT_FLAGS::kHemiShadow))
				{
					params.shadowAngle = std::numbers::pi_v<float>;
				}
				else
				{
					params.shadowAngle = std::numbers::pi_v<float> * 2.0f;
				}

				params.nearDistance    = a_lightForm->data.nearDistance;
				params.shadowDepthBias = 0.0f;  // 0 if base != TESObjectLIGH (TESObjectREFR::GetExtraLightShadowDepthBias(a_refr))

				//params.unk00           = true;
			}
			else
			{
				params.shadow = false;
			}

			return params;
		}

		static constexpr RE::LIGHT_CREATE_PARAMS make_params() noexcept
		{
			return {
				true,
				false,
				false,
				true,
				true,
				true,
				1.0f,
				1.0f,
				5.0f,
				0.0f,
				0,
				{ nullptr, nullptr }
			};
		}

		static constexpr NiColor make_diffuse(
			const TESObjectLIGH* a_lightForm) noexcept
		{
			constexpr auto mul = 1.0f / 255.0f;

			if (!a_lightForm->data.flags.test_any(TES_LIGHT_FLAGS::kNegative))
			{
				return {
					static_cast<float>(a_lightForm->data.color.red) * mul,
					static_cast<float>(a_lightForm->data.color.green) * mul,
					static_cast<float>(a_lightForm->data.color.blue) * mul
				};
			}
			else
			{
				return {
					-(static_cast<float>(a_lightForm->data.color.red) * mul),
					-(static_cast<float>(a_lightForm->data.color.green) * mul),
					-(static_cast<float>(a_lightForm->data.color.blue) * mul)
				};
			}
		}

		static constexpr NiPoint3 make_radius(
			const TESObjectLIGH* a_lightForm) noexcept
		{
			auto radius = static_cast<float>(a_lightForm->data.radius);

			if (radius <= 0.0f)
			{
				radius = 0.1f;
			}

			return { radius, radius, radius };
		}
	}

	static REFR_LIGHT* GetExtraLight(Actor* a_actor) noexcept
	{
		auto extraLight = a_actor->extraData.Get<ExtraLight>();
		if (extraLight)
		{
			return extraLight->lightData;
		}
		else
		{
			return nullptr;
		}
	}

	static constexpr TESObjectLIGH* GetEquippedLHLight(
		const Actor* const a_actor) noexcept
	{
		const auto* const pm = a_actor->processManager;
		if (!pm)
		{
			return nullptr;
		}

		const auto* const mid = pm->middleProcess;
		if (!mid)
		{
			return nullptr;
		}

		const auto* const lh = mid->leftHand;
		if (!lh)
		{
			return nullptr;
		}

		const auto obj = lh->type;
		if (!obj)
		{
			return nullptr;
		}

		return obj->As<TESObjectLIGH>();
	}

	void ReferenceLightController::Initialize()
	{
		if (auto edl = ScriptEventSourceHolder::GetSingleton())
		{
			edl->AddEventSink<TESCellAttachDetachEvent>(this);
		}

		m_initialized = true;
	}

	void ReferenceLightController::OnUpdatePlayerLight(
		PlayerCharacter* a_actor) const noexcept
	{
		const shared_lock lock(m_lock);

		visit_lights(
			a_actor,
			[&](auto& a_entry) noexcept [[msvc::forceinline]] {
				UpdateRefrLight(a_entry.form, a_entry.niLight, a_actor, -1.0f);
			});
	}

	void ReferenceLightController::OnActorCrossCellBoundary(
		Actor* a_actor) const noexcept
	{
		const shared_lock lock(m_lock);

		visit_lights(a_actor, [&](auto& a_entry) [[msvc::forceinline]] {
			const auto params = detail::make_params(a_entry.form);

			const auto ssn = *EngineExtensions::m_shadowSceneNode;
			ssn->CreateAndAddLight(a_entry.niLight.get(), params);
		});
	}

	void ReferenceLightController::OnActorCellAttached(
		Actor* a_actor) const noexcept
	{
		if (a_actor == *g_thePlayer)
		{
			return;
		}

		if (!a_actor->Get3D2())
		{
			return;
		}

		OnActorCrossCellBoundary(a_actor);

		if (m_fixVanillaLightOnCellAttach.load(std::memory_order_relaxed))
		{
			ReAddActorExtraLight(a_actor);
		}
	}

	void ReferenceLightController::OnRefreshLightOnSceneMove(
		Actor* a_actor) const noexcept
	{
		const shared_lock lock(m_lock);

		visit_lights(
			a_actor,
			[](auto& a_entry) noexcept [[msvc::forceinline]] {
				if (auto& bsl = a_entry.bsLight)
				{
					QueueAddLight(
						*EngineExtensions::m_shadowSceneNode,
						bsl.get());
				}
			});
	}

	void ReferenceLightController::OnActorUpdate(
		Actor*      a_actor,
		REFR_LIGHT* a_extraLight) const noexcept
	{
		if (a_actor == *g_thePlayer)
		{
			const shared_lock lock(m_lock);

			visit_lights(
				a_actor,
				[](auto& a_entry) noexcept [[msvc::forceinline]] {
					UnkQueueBSLight(
						*EngineExtensions::m_shadowSceneNode,
						a_entry.niLight.get());
				});
		}
		else
		{
			if (a_extraLight &&
			    m_fixVanillaNPCLightUpdates.load(std::memory_order_relaxed))
			{
				if (a_extraLight->light)
				{
					if (auto equipped = GetEquippedLHLight(a_actor))
					{
						if (::NRTTI<NiPointLight>::IsType(a_extraLight->light->GetRTTI()))
						{
							UpdateRefrLight(
								equipped,
								reinterpret_cast<const NiPointer<NiPointLight>&>(a_extraLight->light),
								a_actor,
								-1.0f);
						}
					}
				}
			}

			if (m_npcLightUpdates.load(std::memory_order_relaxed))
			{
				const shared_lock lock(m_lock);

				visit_lights(
					a_actor,
					[&](auto& a_entry) noexcept [[msvc::forceinline]] {
						UpdateRefrLight(
							a_entry.form,
							a_entry.niLight,
							a_actor,
							-1.0f);

						UnkQueueBSLight(
							*EngineExtensions::m_shadowSceneNode,
							a_entry.niLight.get());
					});
			}
			else
			{
				const shared_lock lock(m_lock);

				visit_lights(
					a_actor,
					[](auto& a_entry) noexcept [[msvc::forceinline]] {
						UnkQueueBSLight(
							*EngineExtensions::m_shadowSceneNode,
							a_entry.niLight.get());
					});
			}
		}
	}

	void ReferenceLightController::AddLight(
		Game::FormID       a_actor,
		TESObjectLIGH*     a_form,
		const ObjectLight& a_light) noexcept
	{
		if (m_initialized)
		{
			const unique_lock lock(m_lock);

			auto& e = m_data.try_emplace(a_actor).first->second;

			e.emplace_front(a_form, a_light.niObject, a_light.bsObject);
		}
	}

	void ReferenceLightController::RemoveLight(
		Game::FormID  a_actor,
		NiPointLight* a_light) noexcept
	{
		if (m_initialized)
		{
			const unique_lock lock(m_lock);

			auto it = m_data.find(a_actor);
			if (it != m_data.end())
			{
				it->second.remove_if(
					[&](auto& a_v) {
						return a_v.niLight == a_light;
					});
			}
		}
	}

	void ReferenceLightController::RemoveActor(
		Game::FormID a_actor) noexcept
	{
		if (m_initialized)
		{
			const unique_lock lock(m_lock);

			m_data.erase(a_actor);
		}
	}

	ObjectLight ReferenceLightController::CreateAndAttachPointLight(
		const TESObjectLIGH* a_lightForm,
		Actor*               a_actor,
		NiNode*              a_object) noexcept
	{
		const auto sh = BSStringHolder::GetSingleton();

		auto attachmentNode = ::Util::Node::GetNodeByName(a_object, sh->m_attachLight);

		if (!attachmentNode)
		{
			attachmentNode = a_object;
		}

		char name[224];
		stl::snprintf(name, "%.8X PtLight", a_lightForm->formID.get());

		const auto pointLight = NiPointLight::Create();

		pointLight->m_name = name;

		attachmentNode->AttachChild(pointLight, true);

		pointLight->ambient = {};
		pointLight->radius  = detail::make_radius(a_lightForm);
		pointLight->diffuse = detail::make_diffuse(a_lightForm);

		// at this point, beth sets a flag on the base form if a transform controller is found on a_object

		RE::BSLight* bsLight = nullptr;

		const auto* const extraLitWaterRefs = a_actor->extraData.Get<ExtraLitWaterRefs>();

		if (!extraLitWaterRefs ||
		    extraLitWaterRefs->refs.empty())  // ?
		{
			const auto params = detail::make_params(a_lightForm);

			const auto ssn = *EngineExtensions::m_shadowSceneNode;
			bsLight        = ssn->CreateAndAddLight(pointLight, params);
		}

		NiPointLightSetAttenuation(pointLight, static_cast<std::int32_t>(pointLight->radius.x));

		/*
			original code:

			if ( !TESObjectREFR::unk_186(a_refr) )      // false if base != TESObjectLIGH
				v25 = TESObjectREFR::GetExtraLightDataFade(a_refr, a_lightForm->fade);// arg2 if base != TESObjectLIGH, else ExtraLightData->fade + arg2
			pointLight->fade = v25;
		*/

		pointLight->fade = a_lightForm->fade;

		return { pointLight, a_actor == *g_thePlayer ? bsLight : nullptr };
	}

	void ReferenceLightController::CleanupLights(NiNode* a_node) noexcept
	{
		QueueRemoveAllLights(*EngineExtensions::m_shadowSceneNode, a_node, true, true);
	}

	std::size_t ReferenceLightController::GetNumLights() const noexcept
	{
		const shared_lock lock(m_lock);

		std::size_t i = 0;

		for (auto& e : m_data)
		{
			for ([[maybe_unused]] auto& f : e.second)
			{
				i++;
			}
		}

		return i;
	}

	void ReferenceLightController::ReAddActorExtraLight(Actor* a_actor) noexcept
	{
		const auto* const refrLight = GetExtraLight(a_actor);
		if (!refrLight)
		{
			return;
		}

		const auto& light = refrLight->light;
		if (!light)
		{
			return;
		}

		const auto* const torch = GetEquippedLHLight(a_actor);

		const auto params =
			torch ?
				detail::make_params(torch) :
				detail::make_params();

		const auto ssn = *EngineExtensions::m_shadowSceneNode;
		ssn->CreateAndAddLight(light.get(), params);
	}

	EventResult ReferenceLightController::ReceiveEvent(
		const TESCellAttachDetachEvent*           a_evn,
		BSTEventSource<TESCellAttachDetachEvent>* a_dispatcher)
	{
		if (a_evn && a_evn->attached && a_evn->reference)
		{
			if (auto actor = a_evn->reference->As<Actor>())
			{
				OnActorCellAttached(actor);
			}
		}

		return EventResult::kContinue;
	}
}