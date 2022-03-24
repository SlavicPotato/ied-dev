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

			for (auto& f : e.second.GetSlots())
			{
				if (f.state)
				{
					UpdateEffects(f.state->effectShaders);
				}
			}

			for (auto& f : e.second.GetCustom())
			{
				for (auto& g : f)
				{
					for (auto& h : g.second)
					{
						if (h.second.state)
						{
							UpdateEffects(h.second.state->effectShaders);
						}
					}
				}
			}
		}

		m_timer.End(m_currentTime);
	}

	void EffectController::UpdateEffects(const effectShaderData_t& a_data)
	{
		for (auto& e : a_data.data)
		{
			if (e.flags.test(effectShaderData_t::EntryFlags::kForce))
			{
				for (auto& f : e.nodes)
				{
					if (f->effectData != e.shaderData)
					{
						f->SetEffectShaderData(e.shaderData);
					}
				}
			}
			else
			{
				for (auto& f : e.nodes)
				{
					if (!f->effectData)
					{
						if (f->AcceptsEffectData())
						{
							f->SetEffectShaderData(e.shaderData);
						}
					}
					else
					{
						if (f->effectData == e.shaderData &&
						    !f->AcceptsEffectData())
						{
							f->ClearEffectShaderData();
						}
					}
				}
			}
		}
	}
}