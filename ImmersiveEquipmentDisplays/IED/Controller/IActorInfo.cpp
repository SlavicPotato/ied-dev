#include "pch.h"

#include "IActorInfo.h"

#include "IED/FormCommon.h"

#include <ext/GameCommon.h>

namespace IED
{
	void IActorInfo::FillActorInfoEntry(
		Actor*            a_actor,
		actorInfoEntry_t& a_out,
		bool              a_updateNPC)
	{
		a_out.name           = IFormCommon::GetFormName(a_actor);
		a_out.dead           = a_actor->IsDead();
		a_out.flags          = a_actor->flags;
		a_out.hasLoadedState = static_cast<bool>(a_actor->loadedState);

		if (auto race = a_actor->GetRace())
		{
			a_out.race = race->formID;
		}
		else
		{
			a_out.race = {};
		}

		if (auto worldspace = a_actor->GetWorldspace())
		{
			a_out.worldspace = worldspace->formID;
		}
		else
		{
			a_out.worldspace = {};
		}

		if (auto location = a_actor->GetCurrentLocation())
		{
			a_out.location = {
				location->formID,
				IFormCommon::GetFormName(location)
			};
		}
		else
		{
			a_out.location = {};
		}

		if (auto cell = a_actor->GetParentCell())
		{
			a_out.cell = {
				cell->formID,
				IFormCommon::GetFormName(cell)
			};
		}
		else
		{
			a_out.cell = {};
		}

		if (auto skin = a_actor->GetSkin())
		{
			a_out.skin = {
				skin->formID,
				IFormCommon::GetFormName(skin)
			};
		}
		else
		{
			a_out.skin = {};
		}

		if (auto idle = a_actor->GetFurnitureIdle())
		{
			a_out.idle = {
				idle->formID,
				IFormCommon::GetFormName(idle)
			};
		}
		else
		{
			a_out.idle = {};
		}

		if (auto package = a_actor->GetCurrentPackage())
		{
			a_out.package = package->formID;
		}
		else
		{
			a_out.package = {};
		}

		if (auto furniture = a_actor->GetOccupiedFurniture())
		{
			a_out.furniture = {
				furniture->formID,
				IFormCommon::GetFormName(furniture)
			};
		}
		else
		{
			a_out.furniture = {};
		}

		if (auto extraOutfit = a_actor->extraData.Get<ExtraOutfitItem>())  // ??
		{
			a_out.outfit.first  = extraOutfit->id;
			a_out.outfit.second = false;
		}
		else
		{
			a_out.outfit.second = true;

			if (auto npc = a_actor->GetActorBase())
			{
				a_out.outfit.first = npc->defaultOutfit ?
				                         npc->defaultOutfit->formID :
				                         Game::FormID{};
			}
			else
			{
				a_out.outfit.first = {};
			}
		}

		if (auto rightHand = a_actor->GetEquippedObject(false))
		{
			a_out.equipped.first      = rightHand->formID;
			a_out.equippedNames.first = IFormCommon::GetFormName(rightHand);
			a_out.equippedTypes.first = rightHand->formType;
		}
		else
		{
			a_out.equipped.first = {};
			a_out.equippedNames.first.clear();
			a_out.equippedTypes.first = 0;
		}

		if (auto leftHand = a_actor->GetEquippedObject(true))
		{
			a_out.equipped.second      = leftHand->formID;
			a_out.equippedNames.second = IFormCommon::GetFormName(leftHand);
			a_out.equippedTypes.second = leftHand->formType;
		}
		else
		{
			a_out.equipped.second = {};
			a_out.equippedNames.second.clear();
			a_out.equippedTypes.second = 0;
		}

		a_out.weight = a_actor->GetWeight();

		auto containerChanges = a_actor->extraData.Get<ExtraContainerChanges>();

		if (containerChanges && containerChanges->data)
		{
			a_out.inventoryWeight.emplace(
				containerChanges->data->totalWeight,
				containerChanges->data->armorWeight);
		}
		else
		{
			a_out.inventoryWeight.reset();
		}

		a_out.attached = a_actor->IsParentCellAttached();
		a_out.pos      = a_actor->pos;
		a_out.rot      = a_actor->rot;
		a_out.state1   = a_actor->actorState1;
		a_out.state2   = a_actor->actorState2;

		a_out.ts = IPerfCounter::Query();

		if (auto npc = a_actor->GetActorBase())
		{
			if (auto it = m_npcInfo.find(npc->formID);
			    it == m_npcInfo.end())
			{
				a_out.npc = std::make_shared<npcInfoEntry_t>();

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

			if (auto templ = npc->GetFirstNonTemporaryOrThis(); templ && templ != npc)
			{
				if (auto it = m_npcInfo.find(templ->formID);
				    it == m_npcInfo.end())
				{
					auto t = std::make_shared<npcInfoEntry_t>();

					FillNPCInfoEntry(templ, *t);

					m_npcInfo.emplace(templ->formID, std::move(t));
				}
				else
				{
					if (a_updateNPC)
					{
						FillNPCInfoEntry(templ, *it->second);
					}
				}
			}
		}
		else
		{
			a_out.npc.reset();
		}

		if (a_out.npc)
		{
			a_out.npc->active = a_out.active;
		}
	}

	void IActorInfo::FillNPCInfoEntry(
		TESNPC*         a_npc,
		npcInfoEntry_t& a_out)
	{
		auto templ = a_npc->GetTemplate();

		a_out.name    = IFormCommon::GetFormName(a_npc);
		a_out.form    = a_npc->formID;
		a_out.templ   = templ ? templ->formID : Game::FormID{};
		a_out.nontemp = a_npc->GetFirstNonTemporaryOrThis()->formID;
		a_out.flags   = a_npc->flags;
		a_out.female  = a_npc->GetSex() == 1;
		a_out.race    = a_npc->race ?
		                    a_npc->race->formID :
		                    Game::FormID{};
		a_out.weight  = a_npc->GetWeight();

		a_out.ts = IPerfCounter::Query();
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

		auto e = m_npcInfo.emplace(
			npc->formID,
			std::make_shared<npcInfoEntry_t>());

		e.first->second->active = false;

		FillNPCInfoEntry(npc, *e.first->second);
	}

	std::optional<Game::ObjectRefHandle> IActorInfo::GetTargetActortHandle()
	{
		if (auto refHolder = CrosshairRefHandleHolder::GetSingleton())
		{
			auto& handle = refHolder->CrosshairRefHandle();
			if (handle && handle.IsValid())
			{
				return handle;
			}
		}

		if (auto tm = MenuTopicManager::GetSingleton())
		{
			if (tm->talkingHandle && tm->talkingHandle.IsValid())
			{
				return tm->talkingHandle;
			}
		}

		return {};
	}

	void IActorInfo::UpdateActorInfo(
		const ActorObjectMap& a_cache)
	{
		m_actorInfo.clear();
		m_npcInfo.clear();
		m_crosshairRef.reset();

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
				m_crosshairRef.emplace(actor->formID);
			}
		}

		m_actorInfoUpdateID++;
	}

	bool IActorInfo::LookupCrosshairRef(
		NiPointer<TESObjectREFR>& a_out)
	{
		auto handle = GetTargetActortHandle();
		if (!handle)
		{
			return false;
		}

		return handle->Lookup(a_out);
	}

	void IActorInfo::UpdateActorInfo(
		const ActorObjectHolder& a_objectHolder)
	{
		const auto handle = a_objectHolder.GetHandle();

		NiPointer<TESObjectREFR> ref;
		if (!handle.Lookup(ref))
		{
			return;
		}

		const auto actor = ref->As<Actor>();
		if (!actor)
		{
			return;
		}

		const auto r = m_actorInfo.try_emplace(actor->formID);

		r.first->second.active = true;

		FillActorInfoEntry(actor, r.first->second, true);
	}

	void IActorInfo::UpdateActorInfo(
		const ActorObjectMap& a_cache,
		Game::FormID          a_actor)
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

		const auto r = m_npcInfo.emplace(
			npc->formID,
			std::make_shared<npcInfoEntry_t>());

		FillNPCInfoEntry(npc, *r.first->second);

		return true;
	}

	bool IActorInfo::UpdateActorInfo(Game::FormID a_actor)
	{
		auto actor = a_actor.As<Actor>();
		if (!actor)
		{
			return false;
		}

		const auto r = m_actorInfo.try_emplace(actor->formID);

		FillActorInfoEntry(actor, r.first->second, true);

		return true;
	}

	void IActorInfo::ClearActorInfo()
	{
		m_actorInfo.clear();
		m_npcInfo.clear();
	}
}