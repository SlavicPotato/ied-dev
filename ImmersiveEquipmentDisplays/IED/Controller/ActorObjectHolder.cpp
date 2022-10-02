#include "pch.h"

#include "ActorObjectHolder.h"

#include "IObjectManager.h"
#include "ObjectManagerData.h"

#include "IED/ActorState.h"
#include "IED/StringHolder.h"

#include <ext/BSAnimationUpdateData.h>
#include <ext/Node.h>

namespace IED
{
	std::atomic_ullong ActorObjectHolder::m_lfsc_delta_lf{ 0ull };
	std::atomic_ullong ActorObjectHolder::m_lfsc_delta_hf{ 0ull };

	ActorObjectHolder::ActorObjectHolder(
		Actor*                a_actor,
		NiNode*               a_root,
		NiNode*               a_npcroot,
		IObjectManager&       a_owner,
		Game::ObjectRefHandle a_handle,
		bool                  a_nodeOverrideEnabled,
		bool                  a_nodeOverrideEnabledPlayer,
		//bool                    a_animEventForwarding,
		const BipedSlotDataPtr& a_lastEquipped) :
		m_owner(a_owner),
		m_handle(a_handle),
		m_actor(a_actor),
		m_root(a_root),
		m_npcroot(a_npcroot),
		m_formid(a_actor->formID),
		//m_enableAnimEventForwarding(a_animEventForwarding),
		m_created(IPerfCounter::Query()),
		m_lastEquipped(a_lastEquipped),
		m_skeletonID(a_root),
		m_state(a_actor)
	{
		m_lastLFStateCheck = m_created +
		                     m_lfsc_delta_lf.fetch_add(
								 IPerfCounter::T(50000),
								 std::memory_order_relaxed) %
		                         IPerfCounter::T(1250000);

		m_lastHFStateCheck = m_created +
		                     m_lfsc_delta_hf.fetch_add(
								 IPerfCounter::T(4000),
								 std::memory_order_relaxed) %
		                         IPerfCounter::T(100000);

		if (auto r = SkeletonCache::GetSingleton().Get(a_actor))
		{
			m_skeletonCache = r->second;
		}

		if (auto& id = m_skeletonID.get_id())
		{
			auto& ids          = NodeOverrideData::GetHumanoidSkeletonIDs();
			m_humanoidSkeleton = std::find(ids.begin(), ids.end(), id.value()) != ids.end();
		}

		if (auto npc = a_actor->GetActorBase())
		{
			m_female = npc->GetSex() == 1;
		}

		for (auto& e : NodeOverrideData::GetExtraCopyNodes())
		{
			CreateExtraCopyNode(a_actor, a_npcroot, e);
		}

		using namespace ::Util::Node;

		if (a_nodeOverrideEnabled &&
		    (a_nodeOverrideEnabledPlayer ||
		     a_actor != *g_thePlayer))
		{
			for (auto& e : NodeOverrideData::GetMonitorNodeData())
			{
				if (auto node = FindNode(a_npcroot, e))
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
				if (auto node = FindNode(a_npcroot, e->second.bsname))
				{
					m_cmeNodes.try_emplace(
						e->first,
						node,
						GetCachedOrZeroTransform(e->second.name));
				}
			}

			for (auto& e : NodeOverrideData::GetMOVNodeData().getvec())
			{
				if (auto node = FindNode(a_npcroot, e->second.bsname))
				{
					m_movNodes.try_emplace(
						e->first,
						node,
						e->second.placementID);
				}
			}

			for (auto& e : NodeOverrideData::GetWeaponNodeData().getvec())
			{
				if (auto node = FindNode(a_npcroot, e->second.bsname); node && node->m_parent)
				{
					if (auto defParentNode = FindNode(a_npcroot, e->second.bsdefParent))
					{
						m_weapNodes.emplace_back(
							e->first,
							node,
							defParentNode,
							e->second.animSlot,
							e->second.vanilla ?
								GetCachedTransform(e->first) :
                                std::optional<NiTransform>{});
					}
				}
			}

			for (auto& e : NodeOverrideData::GetExtraMovNodes())
			{
				CreateExtraMovNodes(a_npcroot, e);
			}
		}

		using enum_type = std::underlying_type_t<Data::ObjectSlot>;

		for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
		{
			m_entriesSlot[i].slotid   = static_cast<Data::ObjectSlot>(i);
			m_entriesSlot[i].slotidex = Data::ItemData::SlotToExtraSlot(
				static_cast<Data::ObjectSlot>(i));
		}

		/*if (m_humanoidSkeleton)
		{
			auto sh = BSStringHolder::GetSingleton();

			m_spine2      = ::Util::Node::FindNode(a_npcroot, sh->m_npcSpine2);
			m_animObjectR = ::Util::Node::FindNode(a_npcroot, sh->m_animObjectR);
			m_rightHand   = ::Util::Node::FindNode(a_npcroot, sh->m_npcRhand);
		}*/

		for (auto& [i, e] : NodeOverrideData::GetNodeMonitorEntries())
		{
			if (!e.data.flags.test(Data::NodeMonitorFlags::kTargetAllSkeletons))
			{
				auto& id = m_skeletonID.get_id();

				auto it = std::find(
					e.data.targetSkeletons.begin(),
					e.data.targetSkeletons.end(),
					id.value());

				if (it == e.data.targetSkeletons.end())
				{
					continue;
				}
			}

			if (auto parent = ::Util::Node::FindNode(a_npcroot, e.parent))
			{
				auto r = m_nodeMonitorEntries.try_emplace(i, parent, e);
				r.first->second.Update();
			}
		}

		/*RE::BSAnimationGraphManagerPtr agm;
		if (a_actor->GetAnimationGraphManagerImpl(agm))
		{
			if (!agm->graphs.empty())
			{
				if (auto& e = agm->graphs.front())
				{
					e->AddEventSink(this);
				}
			}
		}*/
	}

	ActorObjectHolder::~ActorObjectHolder()
	{
		/*RE::BSAnimationGraphManagerPtr agm;
		if (m_actor->GetAnimationGraphManagerImpl(agm))
		{
			for (auto& e : agm->graphs)
			{
				if (e)
				{
					e->RemoveEventSink(this);
				}
			}
		}*/

		if (m_actor.get() == *g_thePlayer)
		{
			m_owner.StorePlayerState(*this);
		}

		if (m_actor->loadedState)
		{
			for (const auto& e : m_cmeNodes)
			{
				INodeOverride::ResetNodeOverride(e.second);
			}

			for (const auto& e : m_weapNodes)
			{
				INodeOverride::ResetNodePlacement(e, nullptr);
			}
		}

		m_owner.IncrementCounter();
		m_lastEquipped->accessed = m_owner.GetCounterValue();

		std::optional<Game::ObjectRefHandle> handle;

		bool cleanupdb = false;

		visit([&](auto& a_entry) {
			if (!a_entry.state)
			{
				return;
			}

			if (!handle)
			{
				handle = GetHandle();

				NiPointer<TESObjectREFR> ref;
				(void)handle->LookupZH(ref);
			}

			if (!a_entry.state->dbEntries.empty())
			{
				cleanupdb = true;
			}

			a_entry.reset(*handle, m_root);
		});

		if (cleanupdb)
		{
			m_owner.QueueDatabaseCleanup();
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
			m_entriesSlot[i].slotState = a_data.slots[i];
		}
	}

	bool ActorObjectHolder::IsActorNPCOrTemplate(Game::FormID a_npc) const
	{
		auto handle = GetHandle();

		NiPointer<TESObjectREFR> refr;
		if (handle.Lookup(refr))
		{
			if (auto actor = refr->As<Actor>())
			{
				if (auto npc = actor->GetActorBase())
				{
					return (npc->GetFirstNonTemporaryOrThis()->formID == a_npc);
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
				if (auto race = actor->GetRace())
				{
					return (race->formID == a_race);
				}
			}
		}

		return false;
	}

	NiTransform ActorObjectHolder::GetCachedOrZeroTransform(
		const stl::fixed_string& a_name) const
	{
		if (m_skeletonCache)
		{
			auto it = m_skeletonCache->find(a_name);
			if (it != m_skeletonCache->end())
			{
				return it->second.transform;
			}
		}

		return {};
	}

	std::optional<NiTransform> ActorObjectHolder::GetCachedTransform(
		const stl::fixed_string& a_name) const
	{
		if (m_skeletonCache)
		{
			auto it = m_skeletonCache->find(a_name);
			if (it != m_skeletonCache->end())
			{
				return it->second.transform;
			}
		}

		return {};
	}

	void ActorObjectHolder::UpdateAllAnimationGraphs(
		const BSAnimationUpdateData& a_data) const
	{
		visit([&](auto& a_e) {
			if (auto& state = a_e.state)
			{
				state->UpdateAnimationGraphs(a_data);
			}
		});
	}

	float ActorObjectHolder::GetRandomPercent(const luid_tag& a_luid)
	{
		if (m_rpc.size() > MAX_RPC_SIZE)
		{
			m_rpc.clear();
		}

		auto r = m_rpc.try_emplace(a_luid);

		if (!r.second)
		{
			return r.first->second;
		}

		float value = m_owner.GetRandomPercent();

		r.first->second = value;

		return value;
	}

	bool ActorObjectHolder::UpdateNodeMonitorEntries()
	{
		bool result = false;

		for (auto& e : m_nodeMonitorEntries)
		{
			result |= e.second.Update();
		}

		return result;
	}

	bool ActorObjectHolder::GetNodeMonitorResult(std::uint32_t a_uid)
	{
		auto it = m_nodeMonitorEntries.find(a_uid);
		return it != m_nodeMonitorEntries.end() ?
		           it->second.IsPresent() :
                   false;
	}

	void ActorObjectHolder::CreateExtraMovNodes2(
		NiAVObject* a_root)
	{
		if (!a_root)
		{
			return;
		}

		auto sh = BSStringHolder::GetSingleton();
		if (!sh)
		{
			return;
		}

		auto root = a_root->AsNode();
		if (!root)
		{
			return;
		}

		auto npcroot = ::Util::Node::FindNode(root, sh->m_npcroot);
		if (!npcroot)
		{
			return;
		}

		SkeletonID id(root);

		if (!id.get_id())
		{
			return;
		}

		//_DMESSAGE("%X: %u", a_actor->formID, id.get_id());

		for (auto& v : NodeOverrideData::GetExtraMovNodes())
		{
			if (npcroot->GetObjectByName(v.bsname_cme) ||
			    npcroot->GetObjectByName(v.bsname_mov) ||
			    npcroot->GetObjectByName(v.bsname_node))
			{
				continue;
			}

			auto target = ::Util::Node::FindNode(npcroot, v.name_parent);
			if (!target)
			{
				continue;
			}

			for (auto& e : v.skel)
			{
				if (!e.ids.contains(*id.get_id()))
				{
					continue;
				}

				AttachExtraNodes(target, *id.get_id(), v, e);

				break;
			}
		}
	}

	void ActorObjectHolder::CreateExtraMovNodes(
		NiNode*                                   a_npcroot,
		const NodeOverrideData::extraNodeEntry_t& a_entry)
	{
		auto& id = m_skeletonID.get_id();
		if (!id)
		{
			return;
		}

		if (m_cmeNodes.contains(a_entry.name_cme) ||
		    m_movNodes.contains(a_entry.name_mov) ||
		    a_npcroot->GetObjectByName(a_entry.bsname_node))
		{
			return;
		}

		auto target = ::Util::Node::FindNode(a_npcroot, a_entry.name_parent);
		if (!target)
		{
			return;
		}

		for (auto& e : a_entry.skel)
		{
			if (!e.ids.contains(*id))
			{
				continue;
			}

			auto result = AttachExtraNodes(target, *id, a_entry, e);

			m_cmeNodes.try_emplace(a_entry.name_cme, result.cme, result.cme->m_localTransform);
			m_movNodes.try_emplace(a_entry.name_mov, result.mov, a_entry.placementID);

			break;
		}
	}

	auto ActorObjectHolder::AttachExtraNodes(
		NiNode*                                       a_target,
		std::int32_t                                  a_skeletonID,
		const NodeOverrideData::extraNodeEntry_t&     a_entry,
		const NodeOverrideData::extraNodeEntrySkel_t& a_skelEntry)
		-> attachExtraNodesResult_t
	{
		auto cme = INode::CreateAttachmentNode(a_entry.bsname_cme);
		a_target->AttachChild(cme, true);

		auto mov = INode::CreateAttachmentNode(a_entry.bsname_mov);

		mov->m_localTransform = a_skelEntry.transform_mov;

		cme->AttachChild(mov, true);

		auto node = INode::CreateAttachmentNode(a_entry.bsname_node);

		node->m_localTransform = a_skelEntry.transform_node;

		mov->AttachChild(node, true);

		INode::UpdateDownwardPass(cme);

		return { mov, cme };
	}

	void ActorObjectHolder::CreateExtraCopyNode(
		Actor*                                        a_actor,
		NiNode*                                       a_npcroot,
		const NodeOverrideData::extraNodeCopyEntry_t& a_entry) const
	{
		auto source = ::Util::Node::FindNode(a_npcroot, a_entry.bssrc);
		if (!source)
		{
			return;
		}

		auto parent = source->m_parent;
		if (!parent)
		{
			return;
		}

		if (::Util::Node::FindChildObject(parent, a_entry.dst))
		{
			return;
		}

		auto node = INode::CreateAttachmentNode(a_entry.dst);

		if (auto& cache = GetSkeletonCache())
		{
			auto it = cache->find(a_entry.src);

			node->m_localTransform = it != cache->end() ?
			                             it->second.transform :
                                         source->m_localTransform;
		}
		else
		{
			node->m_localTransform = source->m_localTransform;
		}

		parent->AttachChild(node, true);

		INode::UpdateDownwardPass(node);
	}

	void ActorObjectHolder::ApplyXP32NodeTransformOverrides(NiNode* a_root) const
	{
		auto& id = m_skeletonID.get_id();
		if (!id)
		{
			return;
		}

		// apply only to XPMSSE skeleton

		if (*id != 628145516 &&  // female
		    *id != 1361955)      // male
		{
			return;
		}

		auto& ver = m_skeletonID.get_version();
		if (!ver)
		{
			return;
		}

		if (*ver < 3.6f)
		{
			return;
		}

		for (auto& e : NodeOverrideData::GetTransformOverrideData())
		{
			if (auto node = ::Util::Node::FindNode(a_root, e.name))
			{
				node->m_localTransform.rot = e.rot;
			}
		}
	}

	/*EventResult ActorObjectHolder::ReceiveEvent(
		const BSAnimationGraphEvent*           a_event,
		BSTEventSource<BSAnimationGraphEvent>* a_eventSource)
	{
		if (a_event)
		{
			auto sh = BSStringHolder::GetSingleton();

			if (a_event->tag == sh->m_graphDeleting)
			{
				m_owner.QueueReSinkAnimationGraphs(m_formid);
			}
			else
			{
				if (auto newAA = GetNewActiveAnimation(a_event))
				{
					if (m_activeAnimation.load() != newAA.value())
					{
						m_activeAnimation = newAA.value();

						m_owner.QueueRequestEvaluate(m_formid, true, true);
					}
				}
			}
		}

		return EventResult::kContinue;
	}*/

	/*std::optional<ActiveActorAnimation> ActorObjectHolder::GetNewActiveAnimation(
		const BSAnimationGraphEvent* a_event) const
	{
		auto sh = BSStringHolder::GetSingleton();

		ActiveActorAnimation current;

		if (a_event->tag == sh->m_animObjectDraw)
		{
			if (a_event->payload == sh->m_animObjectLute)
			{
				current = ActiveActorAnimation::kLute;
			}
			else
			{
				current = ActiveActorAnimation::kNone;
			}
		}
		else if (a_event->tag == sh->m_animObjectUnequip)
		{
			current = ActiveActorAnimation::kNone;
		}
		else
		{
			return {};
		}

		return { current };
	}*/

	/*void ActorObjectHolder::ReSinkAnimationGraphs()
	{
		RE::BSAnimationGraphManagerPtr agm;
		if (m_actor->GetAnimationGraphManagerImpl(agm))
		{
			if (!agm->graphs.empty())
			{
				for (auto& e : agm->graphs)
				{
					if (e)
					{
						e->RemoveEventSink(this);
					}
				}

				if (auto& e = agm->graphs.front())
				{
					e->AddEventSink(this);
				}
			}
		}
	}*/

	/*void ActorObjectHolder::RegisterWeaponAnimationGraphManagerHolder(
		RE::WeaponAnimationGraphManagerHolderPtr& a_ptr,
		bool                                      a_forward)
	{*/
	/*if (a_forward && m_enableAnimEventForwarding)
		{
			m_animEventForwardRegistrations.Add(a_ptr);
		}*/

	//m_animationUpdateList.Add(a_ptr);

	//_DMESSAGE("reg %p", a_ptr.get());
	//}

	/*void ActorObjectHolder::UnregisterWeaponAnimationGraphManagerHolder(
		RE::WeaponAnimationGraphManagerHolderPtr& a_ptr)
	{*/
	/*if (m_enableAnimEventForwarding)
		{
			m_animEventForwardRegistrations.Remove(a_ptr);
		}*/

	//m_animationUpdateList.Remove(a_ptr);

	//_DMESSAGE("unreg %p", a_ptr.get());
	//}

}