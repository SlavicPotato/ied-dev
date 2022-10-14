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

		auto steps = Game::Unk2f6b948::GetSteps();

		if (m_parallel)
		{
			std::for_each(
				std::execution::par,
				a_map.begin(),
				a_map.end(),
				[&](auto& a_e) {
					UpdateActor(steps, a_e.second);
				});
		}
		else
		{
			for (auto& e : a_map)
			{
				UpdateActor(steps, e.second);
			}
		}

		m_timer.End(m_currentTime);
	}

	void EffectController::UpdateActor(
		const Game::Unk2f6b948::Steps& a_steps,
		const ActorObjectHolder&       a_holder)
	{
		if (!a_holder.IsCellAttached())
		{
			return;
		}

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

		auto step = a_holder.GetActorFormID() == Data::IData::GetPlayerRefID() ?
		                a_steps.player :
                        a_steps.npc;

		a_holder.visit([&](auto& a_entry) {
			UpdateObjectEffects(actor, a_entry, step);
		});
	}

	void EffectController::UpdateEffectsOnDisplay(
		const EffectShaderData&       a_data,
		const ObjectEntryBase::State& a_state,
		float                         a_step)
	{
		for (const auto& e : a_data.data)
		{
			e.update_effect_data(a_step);

			if (a_state.flags.test(ObjectEntryFlags::kIsGroup))
			{
				for (auto& f : a_state.groupObjects)
				{
					auto& object = e.flags.test(EffectShaderData::EntryFlags::kTargetRoot) ?
					                   f.second.rootNode :
                                       f.second.object;

					ProcessNiObjectTree(object, e);
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

	void EffectController::UpdateEffectsOnEquipped(
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

		for (std::uint8_t i = 0; i < 2; i++)
		{
			auto& biped = a_actor->GetBiped1(static_cast<bool>(i));
			if (!biped)
			{
				continue;
			}

			auto& object = biped->get_object(a_data.bipedObject).object;
			if (!object)
			{
				continue;
			}

			for (auto& e : a_data.data)
			{
				NiAVObject* target = object.get();

				if (a_data.sheathNode &&
				    e.flags.test(EffectShaderData::EntryFlags::kTargetRoot))
				{
					if (object->m_parent == a_data.sheathNode)
					{
						target = object->m_parent;
					}
					else
					{
						ProcessNiObjectTree(a_data.sheathNode, e);
					}
				}

				ProcessNiObjectTree(target, e);
			}
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

	void EffectController::UpdateObjectEffects(
		[[maybe_unused]] Actor*  a_actor,
		const ObjectEntryCustom& a_entry,
		float                    a_step)
	{
		auto& state = a_entry.state;
		if (!state)
		{
			return;
		}

		auto& efdata = a_entry.effectShaderData;
		if (!efdata)
		{
			return;
		}

		UpdateEffectsOnDisplay(*efdata, *state, a_step);
	}

	void EffectController::UpdateObjectEffects(
		Actor*                 a_actor,
		const ObjectEntrySlot& a_entry,
		float                  a_step)
	{
		auto& efdata = a_entry.effectShaderData;
		if (!efdata)
		{
			return;
		}

		if (efdata->targettingEquipped)
		{
			UpdateEffectsOnEquipped(a_actor, *efdata, a_step);
		}
		else
		{
			if (auto& state = a_entry.state)
			{
				UpdateEffectsOnDisplay(*efdata, *state, a_step);
			}
		}
	}
}