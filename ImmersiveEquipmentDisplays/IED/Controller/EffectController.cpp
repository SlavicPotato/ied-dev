#include "pch.h"

#include "EffectController.h"

#include "ActorObjectHolder.h"
#include "EffectShaderData.h"
#include "INode.h"

#include "IED/Data.h"
#include "IED/ReferenceLightController.h"

namespace IED
{
	void EffectController::PreparePhysicsUpdateData(
		float                             a_interval,
		std::optional<PhysicsUpdateData>& a_data) noexcept
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

	void EffectController::RunEffectUpdates(
		const float                             a_interval,
		const Game::Unk2f6b948::Steps&          a_stepMuls,
		const std::optional<PhysicsUpdateData>& a_physUpdData,
		const ActorObjectHolder&                a_holder) noexcept
	{
		stl::ftz_daz_ctl_scoped<_MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON> fds;

		const auto stepMul =
			a_holder.IsPlayer() ?
				a_stepMuls.player :
				a_stepMuls.npc;

		if (a_physUpdData)
		{
			UpdatePhysics(stepMul, *a_physUpdData, a_holder);
		}

		if (ShaderProcessingEnabled())
		{
			UpdateShaders(a_holder.GetActor(), a_interval * stepMul, a_holder);
		}
	}

	void EffectController::UpdateShaders(
		Actor*                   a_actor,
		const float              a_step,
		const ActorObjectHolder& a_holder) noexcept
	{
		a_holder.visit([&](auto& a_entry) noexcept [[msvc::forceinline]] {
			UpdateObjectShaders(a_actor, a_entry, a_step);
		});
	}

	void EffectController::UpdatePhysics(
		const float              a_stepMul,
		const PhysicsUpdateData& a_physUpdData,
		const ActorObjectHolder& a_holder) noexcept
	{
		a_holder.SimReadTransforms(a_physUpdData.timeAccum * a_stepMul);

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
		if (a_data.bipedObject < BIPED_OBJECT::kTotal)
		{
			return;
		}

		if (a_data.data.empty())
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
		const bool  thirdPerson = a_actor->GetBiped1(false) == biped;
		const auto& sheathNode  = a_data.GetSheathNode(!thirdPerson);

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

	static constexpr bool may_set_shader_data(BSShaderProperty* a_prop) noexcept
	{
		return !a_prop->flags.test(BSShaderProperty::Flag::kDecal) &&
		       a_prop->AcceptsEffectData();
	}

	void EffectController::ProcessNiObjectTree(
		NiAVObject*                    a_object,
		const EffectShaderData::Entry& a_entry) noexcept
	{
		using namespace Util::Node;

		TraverseGeometry(a_object, [&](BSGeometry* a_geometry) noexcept {
			if (const auto& effect = a_geometry->m_spEffectState)
			{
				if (const auto shaderProp = ::NRTTI<BSShaderProperty>()(effect.get()))
				{
					if (!a_entry.targetNodes.empty())
					{
						if (!a_entry.targetNodes.contains(a_geometry->m_name))
						{
							return VisitorControl::kContinue;
						}
					}

					const bool trySet =
						a_entry.flags.test(EffectShaderData::EntryFlags::kForce) ?
							shaderProp->effectData != a_entry.shaderData :
							!shaderProp->effectData;

					if (trySet)
					{
						if (may_set_shader_data(shaderProp))
						{
							shaderProp->SetEffectShaderData(a_entry.shaderData);
						}
					}
					else
					{
						if (shaderProp->effectData == a_entry.shaderData &&
						    !may_set_shader_data(shaderProp))
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