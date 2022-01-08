#include "pch.h"

#include "INodeOverride.h"
#include "ObjectManagerData.h"

#include "IED/ActorState.h"
#include "IED/Data.h"
#include "IED/EngineExtensions.h"
#include "IED/ProcessParams.h"

namespace IED
{
	ActorObjectHolder::ActorObjectHolder(
		const SetObjectWrapper<Data::actorStateEntry_t>& a_playerState,
		Actor* a_actor,
		NiNode* a_root,
		NiNode* a_npcroot,
		IObjectManager& a_owner,
		Game::ObjectRefHandle a_handle,
		bool a_nodeOverrideEnabled,
		bool a_nodeOverrideEnabledPlayer,
		Data::actorStateHolder_t& a_actorState) :
		m_owner(a_owner),
		m_handle(a_handle),
		m_actor(a_actor),
		m_root(a_root),
		m_formid(a_actor->formID),
		m_created(IPerfCounter::Query())
	{
		if (a_nodeOverrideEnabled &&
		    (a_actor != *g_thePlayer ||
		     a_nodeOverrideEnabledPlayer))
		{
			for (auto& e : OverrideNodeInfo::GetMonitorNodeData())
			{
				if (auto obj = a_npcroot->GetObjectByName(e))
				{
					if (auto node = obj->GetAsNiNode())
					{
						m_monitorNodes.emplace_back(
							node,
							node->m_parent,
							node->m_children.m_size,
							node->IsVisible());
					}
				}
			}

			for (auto& e : OverrideNodeInfo::GetCMENodeData().getvec())
			{
				if (auto obj = a_npcroot->GetObjectByName(e->second.bsname))
				{
					if (auto node = obj->GetAsNiNode())
					{
						m_cmeNodes.try_emplace(
							e->first,
							node,
							node->m_localTransform,
							node->IsVisible());
					}
				}
			}

			for (auto& e : OverrideNodeInfo::GetWeaponNodeData().getvec())
			{
				m_weapNodes.emplace_back(
					e->first,
					e->second.bsname,
					e->second.defParent);
			}

			m_monitorNodes.shrink_to_fit();
			m_weapNodes.shrink_to_fit();
		}

		using enum_type = std::underlying_type_t<Data::ObjectSlot>;

		for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
		{
			auto& e = m_entriesSlot[i];

			auto slotid = static_cast<Data::ObjectSlot>(i);

			e.slotid = slotid;
		}

		if (a_actor == *g_thePlayer && a_playerState)
		{
			ApplyActorState(*a_playerState);
		}
		else
		{
			auto it = a_actorState.data.find(a_actor->formID);
			if (it != a_actorState.data.end())
			{
				ApplyActorState(it->second);
				a_actorState.data.erase(it);
			}
		}
	}

	ActorObjectHolder::~ActorObjectHolder()
	{
		SetObjectWrapper<Game::ObjectRefHandle> handle;

		*handle = GetHandle();

		visit([&](objectEntryBase_t& a_entry) {
			if (a_entry.state)
			{
				if (!handle)
				{
					NiPointer<TESObjectREFR> dummy;
					LookupREFRByHandle(*handle, dummy);
					handle.mark(true);
				}

				EngineExtensions::CleanupObject(
					*handle,
					a_entry.state->nodes.obj,
					m_root);

				if (!a_entry.state->dbEntries.empty())
				{
					for (auto& e : a_entry.state->dbEntries)
					{
						e->accessed = IPerfCounter::Query();
					}

					m_owner.QueueDatabaseCleanup();
				}
			}
		});

		for (auto& ce : m_cmeNodes)
		{
			INodeOverride::ResetNodeOverride(ce.second);
		}
	}

	bool ActorObjectHolder::AnySlotOccupied() const noexcept
	{
		for (auto& e : m_entriesSlot)
		{
			if (e.state)
			{
				return true;
			}
		}

		return false;
	}

	std::size_t ActorObjectHolder::GetNumOccupiedSlots() const noexcept
	{
		std::size_t result = 0;

		for (auto& e : m_entriesSlot)
		{
			if (e.state)
			{
				result++;
			}
		}

		return result;
	}

	std::size_t ActorObjectHolder::GetNumOccupiedCustom() const noexcept
	{
		std::size_t r = 0;

		for (auto& e : m_entriesCustom)
		{
			for (auto& f : e)
			{
				for (auto& g : f.second)
				{
					if (g.second.state)
					{
						r++;
					}
				}
			}
		}

		return r;
	}

	void ActorObjectHolder::ApplyActorState(const Data::actorStateEntry_t& a_data)
	{
		using enum_type = std::underlying_type_t<Data::ObjectSlot>;

		for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
		{
			auto& s = a_data.slots[i];
			auto& d = m_entriesSlot[i];

			d.lastEquipped = s.lastEquipped;
			d.lastSeenEquipped = s.lastSeenEquipped;
		}
	}

	bool ActorObjectHolder::IsActorNPC(Game::FormID a_npc) const
	{
		auto handle = GetHandle();

		NiPointer<TESObjectREFR> refr;
		if (handle.Lookup(refr))
		{
			if (auto actor = refr->As<Actor>())
			{
				if (auto baseForm = actor->baseForm)
				{
					if (auto npc = baseForm->As<TESNPC>())
					{
						return (npc->formID == a_npc);
					}
				}
			}
		}

		return false;
	}

	bool ActorObjectHolder::IsActorRace(Game::FormID a_race) const
	{
		auto handle = GetHandle();

		NiPointer<TESObjectREFR> refr;
		if (handle.Lookup(refr))
		{
			if (auto actor = refr->As<Actor>())
			{
				if (auto race = Game::GetActorRace(actor))
				{
					return (race->formID == a_race);
				}
			}
		}

		return false;
	}

}