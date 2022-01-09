#include "pch.h"

#include "IActorInfo.h"

#include "IED/FormCommon.h"

#include <ext/GameCommon.h>

namespace IED
{
	void IActorInfo::FillActorInfoEntry(
		Actor* a_actor,
		actorInfoEntry_t& a_out,
		bool a_updateNPC)
	{
		a_out.name = IFormCommon::GetFormName(a_actor);

		if (auto race = Game::GetActorRace(a_actor))
		{
			a_out.race = race->formID;
		}
		else
		{
			a_out.race = {};
		}

		a_out.weight = a_actor->GetWeight();

		auto npc = a_actor->baseForm ?
                       a_actor->baseForm->As<TESNPC>() :
                       nullptr;

		if (npc)
		{
			auto it = m_npcInfo.find(npc->formID);
			if (it == m_npcInfo.end())
			{
				a_out.npc = std::make_unique<npcInfoEntry_t>();

				FillNPCInfoEntry(npc, *a_out.npc);

				m_npcInfo.emplace(npc->formID, a_out.npc);
			}
			else
			{
				if (a_updateNPC)
				{
					FillNPCInfoEntry(npc, *it->second);
				}

				a_out.npc = it->second;
			}
		}
	}

	void IActorInfo::FillNPCInfoEntry(
		TESNPC* a_npc,
		npcInfoEntry_t& a_out)
	{
		a_out.name = IFormCommon::GetFormName(a_npc);
		a_out.form = a_npc->formID;
		a_out.flags = a_npc->flags;
		a_out.female = a_npc->GetSex() == 1;
		a_out.race = a_npc->race.race ? a_npc->race.race->formID : 0;
		a_out.weight = Game::GetNPCWeight(a_npc);
	}

	void IActorInfo::AddExtraActorEntry(Game::FormID a_formid)
	{
		auto actor = a_formid.As<Actor>();
		if (!actor)
		{
			return;
		}

		auto e = m_actorInfo.try_emplace(a_formid);
		if (!e.second)
		{
			return;
		}

		e.first->second.active = false;

		FillActorInfoEntry(actor, e.first->second);
	}

	void IActorInfo::AddExtraNPCEntry(Game::FormID a_formid)
	{
		auto npc = a_formid.As<TESNPC>();
		if (!npc)
		{
			return;
		}

		auto it = m_npcInfo.find(npc->formID);
		if (it != m_npcInfo.end())
		{
			return;
		}

		auto e = m_npcInfo.emplace(npc->formID, std::make_unique<npcInfoEntry_t>());

		e.first->second->active = false;

		FillNPCInfoEntry(npc, *e.first->second);
	}

	void IActorInfo::UpdateActorInfo(
		const ActorObjectMap& a_cache)
	{
		m_actorInfo.clear();
		m_crosshairRef.clear();

		for (auto& e : a_cache)
		{
			UpdateActorInfo(e.second);
		}

		Game::AIProcessVisitActors([this](Actor* a_actor) {
			auto r = m_actorInfo.try_emplace(a_actor->formID);

			r.first->second.active = true;

			FillActorInfoEntry(a_actor, r.first->second);
		});

		auto& cfgStore = AIGetConfigStore();

		for (auto& e : cfgStore.slot.GetActorData())
		{
			AddExtraActorEntry(e.first);
		}

		for (auto& e : cfgStore.custom.GetActorData())
		{
			AddExtraActorEntry(e.first);
		}

		for (auto& e : cfgStore.transforms.GetActorData())
		{
			AddExtraActorEntry(e.first);
		}

		for (auto& e : cfgStore.slot.GetNPCData())
		{
			AddExtraNPCEntry(e.first);
		}

		for (auto& e : cfgStore.custom.GetNPCData())
		{
			AddExtraNPCEntry(e.first);
		}

		for (auto& e : cfgStore.transforms.GetNPCData())
		{
			AddExtraNPCEntry(e.first);
		}

		NiPointer<TESObjectREFR> ref;

		if (LookupCrosshairRef(ref))
		{
			if (auto actor = ref->As<Actor>())
			{
				m_crosshairRef = actor->formID;
			}
		}

		m_actorInfoUpdateID++;
	}

	bool IActorInfo::LookupCrosshairRef(
		NiPointer<TESObjectREFR>& a_out)
	{
		auto refHolder = CrosshairRefHandleHolder::GetSingleton();
		if (!refHolder)
		{
			return false;
		}

		auto &handle = refHolder->CrosshairRefHandle();
		if (!handle || !handle.IsValid())
		{
			return false;
		}

		return handle.Lookup(a_out);
	}

	void IActorInfo::UpdateActorInfo(
		const ActorObjectHolder& a_objectHolder)
	{
		auto handle = a_objectHolder.GetHandle();

		NiPointer<TESObjectREFR> ref;
		if (!handle.Lookup(ref))
		{
			return;
		}

		auto actor = ref->As<Actor>();
		if (!actor)
		{
			return;
		}

		auto r = m_actorInfo.try_emplace(actor->formID);

		r.first->second.active = true;

		FillActorInfoEntry(actor, r.first->second, true);
	}

	void IActorInfo::UpdateActorInfo(
		const ActorObjectMap& a_cache,
		Game::FormID a_actor)
	{
		auto it = a_cache.find(a_actor);
		if (it != a_cache.end())
		{
			UpdateActorInfo(it->second);
		}
	}

	bool IActorInfo::UpdateNPCInfo(Game::FormID a_npc)
	{
		auto npc = a_npc.As<TESNPC>();
		if (!npc)
		{
			return false;
		}

		auto e = m_npcInfo.emplace(npc->formID, std::make_unique<npcInfoEntry_t>());

		FillNPCInfoEntry(npc, *e.first->second);

		return true;
	}

	bool IActorInfo::UpdateActorInfo(Game::FormID a_actor)
	{
		auto actor = a_actor.As<Actor>();
		if (!actor)
		{
			return false;
		}

		auto r = m_actorInfo.try_emplace(actor->formID);

		FillActorInfoEntry(actor, r.first->second, true);

		return true;
	}

	void IActorInfo::ClearActorInfo()
	{
		m_actorInfo.clear();
		m_npcInfo.clear();
	}
}