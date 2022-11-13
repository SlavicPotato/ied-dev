#include "pch.h"

#include "EffectController.h"

#include "ActorObjectHolder.h"
#include "EffectShaderData.h"
#include "INode.h"

#include "IED/Data.h"

namespace IED
{
	void EffectController::ProcessEffects(const ActorObjectMap& a_map)
	{
		m_timer.Begin();

		if (m_flags.test_any(EffectControllerFlags::kEnableMask))
		{
			ProcessEffectsImpl(a_map);
		}

		m_timer.End(m_currentTime);
	}

	void EffectController::ProcessEffectsImpl(const ActorObjectMap& a_map)
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
				[&](auto& a_e) {
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
		std::optional<PhysUpdateData>& a_data)
	{
		constexpr auto confTimeTick = 1.0f / 60.0f;
		constexpr auto maxSubSteps  = 15.0f;

		m_averageInterval = m_averageInterval * 0.875f + a_interval * 0.125f;
		float timeTick    = std::min(m_averageInterval, confTimeTick);

		m_timeAccum += a_interval;

		if (m_timeAccum > timeTick * 0.25f)
		{
			a_data.emplace(
				timeTick,
				std::min(m_timeAccum, timeTick * maxSubSteps),
				timeTick * 1.25f);

			m_timeAccum = 0.0f;
		}
	}

	void EffectController::RunUpdates(
		const float                          a_interval,
		const Game::Unk2f6b948::Steps&       a_stepMuls,
		const std::optional<PhysUpdateData>& a_physUpdData,
		const ActorObjectHolder&             a_holder)
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
		const ActorObjectHolder& a_holder)
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

		a_holder.visit([&](auto& a_entry) [[msvc::forceinline]] {
			UpdateObjectShaders(actor, a_entry, a_step);
		});
	}

	void EffectController::UpdatePhysics(
		const float              a_stepMul,
		const PhysUpdateData&    a_physUpdData,
		const ActorObjectHolder& a_holder) noexcept
	{
		a_holder.SimReadTransforms();

		auto timeStep = a_physUpdData.timeStep;

		while (timeStep >= a_physUpdData.maxTime)
		{
			a_holder.SimUpdate(a_physUpdData.timeTick * a_stepMul);

			timeStep -= a_physUpdData.timeTick;
		}

		a_holder.SimUpdate(timeStep * a_stepMul);

		a_holder.SimWriteTransforms();
	}

	void EffectController::UpdateShadersOnDisplay(
		const EffectShaderData&       a_data,
		const ObjectEntryBase::State& a_state,
		float                         a_step)
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
		float                   a_step)
	{
		if (a_data.bipedObject == BIPED_OBJECT::kNone)
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

		auto& object = biped->get_object(a_data.bipedObject).object;
		if (!object)
		{
			return;
		}

		bool thirdPerson = a_actor->GetBiped1(false) == biped;

		auto& sheathNode = a_data.GetSheathNode(!thirdPerson);

		for (auto& e : a_data.data)
		{
			NiAVObject* target = object.get();

			if (sheathNode &&
			    e.flags.test(EffectShaderData::EntryFlags::kTargetRoot))
			{
				if (object->m_parent == sheathNode)
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
		const EffectShaderData::Entry& a_entry)
	{
		Util::Node::TraverseGeometry(a_object, [&](BSGeometry* a_geometry) {
			if (auto& effect = a_geometry->m_spEffectState)
			{
				if (auto shaderProp = NRTTI<BSShaderProperty>()(effect.get()))
				{
					if (!a_entry.targetNodes.empty())
					{
						if (!a_entry.targetNodes.contains(a_geometry->m_name))
						{
							return Util::Node::VisitorControl::kContinue;
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

			return Util::Node::VisitorControl::kContinue;
		});
	}

	void EffectController::UpdateObjectShaders(
		[[maybe_unused]] Actor*  a_actor,
		const ObjectEntryCustom& a_entry,
		float                    a_step)
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
		float                  a_step)
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