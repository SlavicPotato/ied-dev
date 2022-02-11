#include "pch.h"

#include "INodeOverride.h"
#include "ObjectManagerData.h"

#include "IED/ActorState.h"
#include "IED/Controller/INode.h"
#include "IED/Data.h"
#include "IED/EngineExtensions.h"
#include "IED/ProcessParams.h"

#include <ext/Node.h>

namespace IED
{
	ActorObjectHolder::ActorObjectHolder(
		const stl::optional<Data::actorStateEntry_t>& a_playerState,
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
		m_npcroot(a_npcroot),
		m_formid(a_actor->formID),
		m_cellAttached(a_actor->IsParentCellAttached()),
		m_inInterior(a_actor->IsInInteriorCell()),
		m_created(IPerfCounter::Query())
	{
		if (a_nodeOverrideEnabled &&
		    (a_actor != *g_thePlayer ||
		     a_nodeOverrideEnabledPlayer))
		{
			for (auto& e : NodeOverrideData::GetMonitorNodeData())
			{
				if (auto node = ::Util::Node::FindNode(a_npcroot, e))
				{
					m_monitorNodes.emplace_back(
						node,
						node->m_parent,
						node->m_children.m_size,
						node->IsVisible());
				}
			}

			for (auto& e : NodeOverrideData::GetCMENodeData().getvec())
			{
				if (auto node = ::Util::Node::FindNode(a_npcroot, e->second.bsname))
				{
					m_cmeNodes.try_emplace(
						e->first,
						node);
				}
			}

			for (auto& e : NodeOverrideData::GetMOVNodeData().getvec())
			{
				if (auto node = ::Util::Node::FindNode(a_npcroot, e->second.bsname))
				{
					m_movNodes.try_emplace(
						e->first,
						node);
				}
			}

			for (auto& e : NodeOverrideData::GetWeaponNodeData().getvec())
			{
				if (auto node = ::Util::Node::FindNode(a_npcroot, e->second.bsname); node && node->m_parent)
				{
					if (auto defParentNode = ::Util::Node::FindNode(a_npcroot, e->second.bsdefParent))
					{
						m_weapNodes.emplace_back(
							e->first,
							node,
							defParentNode);
					}
				}
			}

			if (!m_weapNodes.empty() &&
			    !m_cmeNodes.empty() &&
			    !m_movNodes.empty())
			{
				if (auto npc = Game::GetActorBase(a_actor))
				{
					bool female = npc->GetSex() == 1;

					for (auto& e : NodeOverrideData::GetExtraNodes())
					{
						CreateExtraNodes(a_npcroot, female, e);
					}
				}
			}
		}

		using enum_type = std::underlying_type_t<Data::ObjectSlot>;

		for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
		{
			auto& e = m_entriesSlot[i];

			e.slotid = static_cast<Data::ObjectSlot>(i);
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
		for (auto& e : m_cmeNodes)
		{
			INodeOverride::ResetNodeOverride(e.second);
		}

		for (auto& e : m_weapNodes)
		{
			INodeOverride::ResetNodePlacement(e);
		}

		stl::optional<Game::ObjectRefHandle> handle;

		visit([&](objectEntryBase_t& a_entry) {
			if (!a_entry.state)
			{
				return;
			}

			if (!handle)
			{
				handle = GetHandle();

				NiPointer<TESObjectREFR> refr;
				LookupREFRByHandle(*handle, refr);
			}

			if (!a_entry.state->dbEntries.empty())
			{
				m_owner.QueueDatabaseCleanup();
			}

			a_entry.reset(*handle, m_root);
		});
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

			d.slotState = s;
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

	void ActorObjectHolder::CreateExtraNodes(
		NiNode* a_npcroot,
		bool a_female,
		const NodeOverrideData::extraNodeEntry_t& a_entry)
	{
		if (m_cmeNodes.contains(a_entry.name_cme) ||
		    m_movNodes.contains(a_entry.name_mov))
		{
			return;
		}

		auto target = ::Util::Node::FindNode(a_npcroot, a_entry.name_parent);
		if (!target)
		{
			return;
		}

		auto cme = INode::CreateAttachmentNode(a_entry.bsname_cme);
		target->AttachChild(cme, true);

		auto mov = INode::CreateAttachmentNode(a_entry.bsname_mov);

		mov->m_localTransform = a_female ?
                                    a_entry.transform_f :
                                    a_entry.transform_m;

		cme->AttachChild(mov, true);

		INode::UpdateDownwardPass(cme);

		m_cmeNodes.try_emplace(a_entry.name_cme, cme);
		m_movNodes.try_emplace(a_entry.name_mov, mov);
	}

	void objectEntryBase_t::reset(
		Game::ObjectRefHandle a_handle,
		NiNode* a_root)
	{
		if (!state)
		{
			return;
		}

		for (auto& e : state->dbEntries)
		{
			e->accessed = IPerfCounter::Query();
		}

		if (EngineExtensions::SceneRendering() ||
		    !ITaskPool::IsRunningOnCurrentThread())
		{
			struct DisposeStateTask :
				public TaskDelegate
			{
			public:
				DisposeStateTask(
					std::unique_ptr<State>&& a_state,
					Game::ObjectRefHandle a_handle,
					NiNode* a_root) :
					m_state(std::move(a_state)),
					m_handle(a_handle),
					m_root(a_root)
				{}

				virtual void Run() override
				{
					m_state->CleanupObjects(m_handle);

					m_state.reset();
					m_root.reset();
				}

				virtual void Dispose() override
				{
					delete this;
				}

			private:
				std::unique_ptr<State> m_state;
				Game::ObjectRefHandle m_handle;
				NiPointer<NiNode> m_root;
			};

			ITaskPool::AddPriorityTask<DisposeStateTask>(
				std::move(state),
				a_handle,
				a_root);
		}
		else
		{
			state->CleanupObjects(a_handle);
			state.reset();
		}
	}

	void objectEntryBase_t::State::CleanupObjects(Game::ObjectRefHandle a_handle)
	{
		for (auto& e : groupObjects)
		{
			EngineExtensions::CleanupNodeImpl(
				a_handle,
				e.second.object);
		}

		EngineExtensions::CleanupNodeImpl(
			a_handle,
			nodes.obj);
	}

}