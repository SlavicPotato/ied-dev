#include "pch.h"

#include "ActorActiveEffectInfo.h"

#include "FormCommon.h"

namespace IED
{
	void actorActiveEffectInfo_t::Update(Actor* a_actor)
	{
		data.clear();

		auto list = a_actor->GetActiveEffectList();
		if (!list)
		{
			return;
		}

		for (auto& e : *list)
		{
			if (e)
			{
				data.emplace_back(e);
			}
		}
	}

	static actorActiveEffectInfo_t::formEntry_t make_entry(TESForm* a_form)
	{
		return { a_form->formID, a_form->formType, IFormCommon::GetFormName(a_form) };
	}

	actorActiveEffectInfo_t::entry_t::entry_t(ActiveEffect* a_effect)
	{
		duration  = a_effect->duration;
		elapsed   = a_effect->elapsedSeconds;
		magnitude = a_effect->magnitude;

		if (auto v = a_effect->effect)
		{
			if (auto mgef = v->mgef)
			{
				effect = make_entry(mgef);
			}
		}

		if (auto v = a_effect->spell)
		{
			spell = make_entry(v);
		}

		if (auto v = a_effect->source)
		{
			source = make_entry(v);
		}
	}
}