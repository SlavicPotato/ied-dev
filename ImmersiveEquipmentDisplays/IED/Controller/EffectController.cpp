#include "pch.h"

#include "EffectController.h"

#include "ActorObjectHolder.h"
#include "EffectShaderData.h"
#include "INode.h"

#include "IED/Data.h"

namespace IED
{
	void EffectController::ProcessEffects(const ActorObjectMap& a_map) noexcept
	{
		m_timer.Begin();

		if (m_flags.test_any(EffectControllerFlags::kEnableMask))
		{
			ProcessEffectsImpl(a_map);
		}

		m_timer.End(m_currentTime);
	}

	void EffectController::ProcessEffectsImpl(const ActorObjectMap& a_map) noexcept
	{
		const auto stepMuls = Game::Unk2f6b948::GetStepMultipliers();

		const auto interval = *Game::g_frameTimerSlow;

		std::optional<PhysUpdateData> physUpdateData;

		if (PhysicsProcessingEnabled())
		{
			PreparePhysicsUpdateData(interval, physUpdateData);
		}

		if (m_flags.test(EffectControllerFlags::kParallelProcessing))
		{
			std::for_each(
				std::execution::par,
				a_map.begin(),
				a_map.end(),
				[&](auto& a_e) noexcept {
					RunUpdates(interval, stepMuls, physUpdateData, a_e.second);
				});
		}
		else
		{
			for (auto& e : a_map)
			{
				RunUpdates(interval, stepMuls, physUpdateData, e.second);
			}
		}
	}

	void EffectController::PreparePhysicsUpdateData(
		float                          a_interval,
		std::optional<PhysUpdateData>& a_data) noexcept
	{
		constexpr auto confTimeTick = 1.0f / 30.0f;
		constexpr auto maxSubSteps  = 15.0f;

		m_averageInterval    = m_averageInterval * 0.875f + a_interval * 0.125f;
		const float timeTick = std::min(m_averageInterval, confTimeTick);

		m_timeAccum += a_interval;

		if (m_timeAccum > timeTick * 0.25f)
		{
			a_data.emplace(
				timeTick,
				std::min(m_timeAccum, timeTick * maxSubSteps),
				timeTick * 1.25f,
				m_timeAccum);

			m_timeAccum = 0.0f;
		}
	}

	void EffectController::RunUpdates(
		const float                          a_interval,
		const Game::Unk2f6b948::Steps&       a_stepMuls,
		const std::optional<PhysUpdateData>& a_physUpdData,
		const ActorObjectHolder&             a_holder) noexcept
	{
		if (!a_holder.IsCellAttached())
		{
			return;
		}

		const auto stepMul =
			a_holder.GetActorFormID() == Data::IData::GetPlayerRefID() ?
				a_stepMuls.player :
				a_stepMuls.npc;

		if (ShaderProcessingEnabled())
		{
			UpdateShaders(a_interval * stepMul, a_holder);
		}

		if (a_physUpdData)
		{
			UpdatePhysics(stepMul, *a_physUpdData, a_holder);
		}
	}

	void EffectController::UpdateShaders(
		const float              a_step,
		const ActorObjectHolder& a_holder) noexcept
	{
		NiPointer<TESObjectREFR> refr;
		if (!a_holder.GetHandle().Lookup(refr))
		{
			return;
		}

		if (!refr->formID)
		{
			return;
		}

		auto actor = refr->As<Actor>();
		if (!actor)
		{
			return;
		}

		a_holder.visit([&](auto& a_entry) noexcept [[msvc::forceinline]] {
			UpdateObjectShaders(actor, a_entry, a_step);
		});
	}

	void EffectController::UpdatePhysics(
		const float              a_stepMul,
		const PhysUpdateData&    a_physUpdData,
		const ActorObjectHolder& a_holder) noexcept
	{
		constexpr unsigned int ftz_daz_mask = _MM_FLUSH_ZERO_MASK | _MM_DENORMALS_ZERO_MASK;
		constexpr unsigned int ftz_daz_on   = _MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON;

		const auto current_csr = _mm_getcsr();

		const auto ftz_daz_bk = current_csr & ftz_daz_mask;

		_mm_setcsr((current_csr & ~ftz_daz_mask) | ftz_daz_on);

		a_holder.SimReadTransforms(a_physUpdData.timeAccum * a_stepMul);

		auto timeStep = a_physUpdData.timeStep;

		while (timeStep >= a_physUpdData.maxTime)
		{
			a_holder.SimUpdate(a_physUpdData.timeTick * a_stepMul);

			timeStep -= a_physUpdData.timeTick;
		}

		a_holder.SimUpdate(timeStep * a_stepMul);

		a_holder.SimWriteTransforms();

		_mm_setcsr((_mm_getcsr() & ~ftz_daz_mask) | ftz_daz_bk);
	}

	void EffectController::UpdateShadersOnDisplay(
		const EffectShaderData&       a_data,
		const ObjectEntryBase::State& a_state,
		float                         a_step) noexcept
	{
		for (const auto& e : a_data.data)
		{
			e.update_effect_data(a_step);

			if (a_state.flags.test(ObjectEntryFlags::kIsGroup))
			{
				if (e.flags.test(EffectShaderData::EntryFlags::kTargetRoot))
				{
					ProcessNiObjectTree(a_state.nodes.rootNode, e);
				}
				else
				{
					for (auto& f : a_state.groupObjects)
					{
						ProcessNiObjectTree(f.second.object, e);
					}
				}
			}
			else
			{
				auto& object = e.flags.test(EffectShaderData::EntryFlags::kTargetRoot) ?
				                   a_state.nodes.rootNode :
				                   a_state.nodes.object;

				ProcessNiObjectTree(object, e);
			}
		}
	}

	void EffectController::UpdateShadersOnEquipped(
		Actor*                  a_actor,
		const EffectShaderData& a_data,
		float                   a_step) noexcept
	{
		if (a_data.bipedObject >= BIPED_OBJECT::kNone)
		{
			return;
		}

		for (auto& e : a_data.data)
		{
			e.update_effect_data(a_step);
		}

		auto& biped = a_actor->GetCurrentBiped();
		if (!biped)
		{
			return;
		}

		const auto& object = biped->get_object(a_data.bipedObject).object;
		if (!object)
		{
			return;
		}

		const bool thirdPerson = a_actor->GetBiped1(false) == biped;

		const auto& sheathNode = a_data.GetSheathNode(!thirdPerson);

		for (auto& e : a_data.data)
		{
			NiAVObject* target = object.get();

			if (sheathNode &&
			    e.flags.test(EffectShaderData::EntryFlags::kTargetRoot))
			{
				if (object->m_parent == sheathNode.get())
				{
					target = object->m_parent;
				}
				else
				{
					ProcessNiObjectTree(sheathNode, e);
				}
			}

			ProcessNiObjectTree(target, e);
		}
	}

	void EffectController::ProcessNiObjectTree(
		NiAVObject*                    a_object,
		const EffectShaderData::Entry& a_entry) noexcept
	{
		using namespace Util::Node;

		TraverseGeometry(a_object, [&](BSGeometry* a_geometry) noexcept {
			if (auto& effect = a_geometry->m_spEffectState)
			{
				if (auto shaderProp = ::NRTTI<BSShaderProperty>()(effect.get()))
				{
					if (!a_entry.targetNodes.empty())
					{
						if (!a_entry.targetNodes.contains(a_geometry->m_name))
						{
							return VisitorControl::kContinue;
						}
					}

					const bool trySet = a_entry.flags.test(EffectShaderData::EntryFlags::kForce) ?
					                        shaderProp->effectData != a_entry.shaderData :
					                        !shaderProp->effectData;

					if (trySet)
					{
						if (shaderProp->AcceptsEffectData())
						{
							shaderProp->SetEffectShaderData(a_entry.shaderData);
						}
					}
					else
					{
						if (shaderProp->effectData == a_entry.shaderData &&
						    !shaderProp->AcceptsEffectData())
						{
							shaderProp->ClearEffectShaderData();
						}
					}
				}
			}

			return VisitorControl::kContinue;
		});
	}

	void EffectController::UpdateObjectShaders(
		[[maybe_unused]] Actor*  a_actor,
		const ObjectEntryCustom& a_entry,
		float                    a_step) noexcept
	{
		auto& state = a_entry.data.state;
		if (!state)
		{
			return;
		}

		auto& efdata = a_entry.data.effectShaderData;
		if (!efdata)
		{
			return;
		}

		UpdateShadersOnDisplay(*efdata, *state, a_step);
	}

	void EffectController::UpdateObjectShaders(
		Actor*                 a_actor,
		const ObjectEntrySlot& a_entry,
		float                  a_step) noexcept
	{
		auto& efdata = a_entry.data.effectShaderData;
		if (!efdata)
		{
			return;
		}

		if (efdata->targettingEquipped)
		{
			UpdateShadersOnEquipped(a_actor, *efdata, a_step);
		}
		else
		{
			if (auto& state = a_entry.data.state)
			{
				UpdateShadersOnDisplay(*efdata, *state, a_step);
			}
		}
	}
}