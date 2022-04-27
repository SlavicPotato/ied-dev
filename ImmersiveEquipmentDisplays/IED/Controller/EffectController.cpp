#include "pch.h"

#include "EffectController.h"

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
		if (!a_holder.GetActor()->formID)
		{
			return;
		}

		if (!a_holder.IsCellAttached())
		{
			return;
		}

		auto step = a_holder.GetActorFormID() == Data::IData::GetPlayerRefID() ?
		                a_steps.player :
                        a_steps.npc;

		for (auto& f : a_holder.GetSlots())
		{
			if (f.state)
			{
				UpdateEffects(f.state->effectShaders, step);
			}
		}

		for (auto& f : a_holder.GetCustom())
		{
			for (auto& g : f)
			{
				for (auto& h : g.second)
				{
					if (h.second.state)
					{
						UpdateEffects(h.second.state->effectShaders, step);
					}
				}
			}
		}
	}

	void EffectController::UpdateEffects(EffectShaderData& a_data, float a_step)
	{
		for (auto& e : a_data.data)
		{
			e.update_effect_data(a_step);

			if (e.flags.test(EffectShaderData::EntryFlags::kForce))
			{
				for (auto& f : e.nodes)
				{
					if (f.prop->effectData != e.shaderData)
					{
						f.prop->SetEffectShaderData(e.shaderData);
					}
				}
			}
			else
			{
				for (auto& f : e.nodes)
				{
					if (!f.prop->effectData)
					{
						if (f.prop->AcceptsEffectData())
						{
							f.prop->SetEffectShaderData(e.shaderData);
						}
					}
					else
					{
						if (f.prop->effectData == e.shaderData &&
						    !f.prop->AcceptsEffectData())
						{
							f.prop->ClearEffectShaderData();
						}
					}
				}
			}
		}
	}
}