#include "pch.h"

#include "EffectController.h"

namespace IED
{
	void EffectController::ProcessEffects(const ActorObjectMap& a_map)
	{
		m_timer.Begin();

		for (auto& e : a_map)
		{
			if (!e.second.GetActor()->formID)
			{
				continue;
			}

			if (!e.second.IsCellAttached())
			{
				continue;
			}

			e.second.visit([](auto& a_object) {
				if (a_object.state)
				{
					UpdateEffects(a_object.state->effectShaders);
				}
			});
		}

		m_timer.End(m_currentTime);
	}

	void EffectController::UpdateEffects(const effectShaderData_t& a_data)
	{
		for (auto& e : a_data.data)
		{
			if (e.second.flags.test(effectShaderData_t::EntryFlags::kYield))
			{
				for (auto& f : e.second.nodes)
				{
					if (!f.first->effectData)
					{
						f.first->SetEffectShaderData(e.second.shaderData);
					}
				}
			}
			else
			{
				for (auto& f : e.second.nodes)
				{
					if (f.first->effectData != e.second.shaderData)
					{
						f.first->SetEffectShaderData(e.second.shaderData);
					}
				}
			}
		}
	}
}