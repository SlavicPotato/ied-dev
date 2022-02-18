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
			for (auto& f : e.second.nodes)
			{
				/*auto& effect = geometry->m_spEffectState;
				if (!effect)
				{
					continue;
				}

				auto shaderProp = ni_cast(effect.get(), BSShaderProperty);
				if (!shaderProp || !shaderProp->AcceptsEffectData())
				{
					continue;
				}*/

				if (f.first->effectData != e.second.shaderData)
				{
					f.first->SetEffectShaderData(e.second.shaderData);
				}
			}
		}
	}
}