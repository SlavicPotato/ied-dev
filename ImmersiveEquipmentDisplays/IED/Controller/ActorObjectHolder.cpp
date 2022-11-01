#include "pch.h"

#include "ActorObjectHolder.h"

#include "IObjectManager.h"
#include "ObjectManagerData.h"

#include "IED/ActorState.h"
#include "IED/ExtraNodes.h"
#include "IED/StringHolder.h"

#include <ext/BSAnimationUpdateData.h>
#include <ext/Node.h>

namespace IED
{
	using namespace SkeletonExtensions;
	using namespace ::Util::Node;

	std::atomic_ullong ActorObjectHolder::m_lfsc_delta_lf{ 0ull };
	std::atomic_ullong ActorObjectHolder::m_lfsc_delta_hf{ 0ull };

	namespace detail
	{
		template <
			class T       = ObjectEntrySlot,
			std::size_t N = stl::underlying(Data::ObjectSlot::kMax)>
		constexpr auto make_object_slot_array(BipedSlotData::display_slot_data_type& a_1)
		{
			return stl::make_array<T, N>([&]<std::size_t I>() {
				constexpr auto slotid = static_cast<Data::ObjectSlot>(I);

				return std::forward_as_tuple(
					a_1[I],
					slotid,
					Data::ItemData::SlotToExtraSlot(slotid));
			});
		}

	}

	ActorObjectHolder::ActorObjectHolder(
		Actor*                a_actor,
		TESNPC*               a_npc,
		TESRace*              a_race,
		NiNode*               a_root,
		NiNode*               a_npcroot,
		IObjectManager&       a_owner,
		Game::ObjectRefHandle a_handle,
		bool                  a_nodeOverrideEnabled,
		bool                  a_nodeOverrideEnabledPlayer,
		bool                  a_syncToFirstPersonSkeleton,
		//bool                    a_animEventForwarding,
		const BipedSlotDataPtr& a_lastEquipped) :
		m_owner(a_owner),
		m_handle(a_handle),
		m_actor(a_actor),
		m_root(a_root),
		m_npcroot(a_npcroot),
		m_actorid(a_actor->formID),
		m_npcid(a_npc->formID),
		m_npcTemplateId(a_npc->GetFirstNonTemporaryOrThis()->formID),
		m_raceid(a_race->formID),
		m_female(a_npc->GetSex() == 1),
		m_player(a_actor == *g_thePlayer),
		//m_enableAnimEventForwarding(a_animEventForwarding),
		m_created(IPerfCounter::Query()),
		m_lastEquipped(a_lastEquipped),
		m_skeletonID(a_root),
		m_state(a_actor),
		m_entriesSlot{ detail::make_object_slot_array(a_lastEquipped->displays) }
	{
		auto interval = IPerfCounter::T(STATE_CHECK_INTERVAL_LOW);

		m_lastLFStateCheck = m_created +
		                     m_lfsc_delta_lf.fetch_add(
								 interval / 20,
								 std::memory_order_relaxed) %
		                         interval;

		interval = IPerfCounter::T(STATE_CHECK_INTERVAL_HIGH);

		m_lastHFStateCheck = m_created +
		                     m_lfsc_delta_hf.fetch_add(
								 IPerfCounter::T(interval / 20),
								 std::memory_order_relaxed) %
		                         IPerfCounter::T(interval);

		if (auto root1p = a_actor->Get3D1(true); root1p && root1p != a_root)
		{
			m_root1p = root1p->AsNode();
		}

		m_skeletonCache = SkeletonCache::GetSingleton().Get2(a_actor);

		if (m_root1p && a_syncToFirstPersonSkeleton)
		{
			m_skeletonCache1p = SkeletonCache::GetSingleton().Get2(a_actor, true);
		}

		m_humanoidSkeleton =
			NodeOverrideData::GetHumanoidSkeletonSignatures()
				.contains(m_skeletonID.signature());

		for (auto& e : NodeOverrideData::GetExtraCopyNodes())
		{
			CreateExtraCopyNode(a_npcroot, e);
		}

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

			auto const npcroot1p = m_root1p ?
			                           FindNode(
										   m_root1p,
										   BSStringHolder::GetSingleton()->m_npcroot) :
                                       nullptr;

			for (auto& e : NodeOverrideData::GetCMENodeData().getvec())
			{
				if (auto node = FindNode(a_npcroot, e->second.bsname))
				{
					auto r = m_cmeNodes.try_emplace(
						e->first,
						node,
						GetCachedOrZeroTransform(e->second.name));

					if (a_syncToFirstPersonSkeleton && npcroot1p)
					{
						if (node = FindNode(npcroot1p, e->second.bsname))
						{
							r.first->second.firstPerson = {
								node,
								GetCachedOrZeroTransform(e->second.name, true)
							};
						}
					}
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
						auto node1p = npcroot1p ?
						                  FindNode(npcroot1p, e->second.bsname) :
                                          nullptr;

						m_weapNodes.emplace_back(
							e->first,
							node,
							defParentNode,
							node1p,
							e->second.animSlot,
							e->second.nodeID,
							e->second.vanilla ?
								GetCachedTransform(e->first) :
                                std::optional<NiTransform>{});
					}
				}
			}

			CreateExtraMovNodes(a_npcroot);
		}

		for (auto& [i, e] : NodeOverrideData::GetNodeMonitorEntries())
		{
			if (!e.data.flags.test(Data::NodeMonitorFlags::kTargetAllSkeletons))
			{
				if (e.data.targetSkeletons.empty())
				{
					continue;
				}

				if (!e.data.targetSkeletons.test(m_skeletonID))
				{
					continue;
				}
			}

			if (auto parent = FindNode(a_npcroot, e.parent))
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

		/*if (m_player)
		{
			m_owner.StorePlayerState(*this);
		}*/

		m_lastEquipped->accessed = m_owner.IncrementCounter();

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

		if (EngineExtensions::ShouldDefer3DTask())
		{
			QueueDisposeAllObjectEntries(GetHandle());
		}
		else
		{
			std::optional<Game::ObjectRefHandle> handle;
			NiPointer<TESObjectREFR>             ref;

			visit([&](auto& a_entry) {
				if (!a_entry.data)
				{
					return;
				}

				if (!handle)
				{
					handle.emplace(GetHandle());
					(void)handle->LookupZH(ref);
				}

				a_entry.reset(*handle, m_root, m_root1p, m_owner);
			});
		}
	}

	bool ActorObjectHolder::AnySlotOccupied() const noexcept
	{
		for (auto& e : m_entriesSlot)
		{
			if (e.data.state)
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
			if (e.data.state)
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
					if (g.second.data.state)
					{
						r++;
					}
				}
			}
		}

		return r;
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
		const stl::fixed_string& a_name,
		bool                     a_firstPerson) const
	{
		if (auto& cache = GetSkeletonCache(a_firstPerson))
		{
			auto it = cache->find(a_name);
			if (it != cache->end())
			{
				return it->second.transform;
			}
		}

		return {};
	}

	std::optional<NiTransform> ActorObjectHolder::GetCachedTransform(
		const stl::fixed_string& a_name,
		bool                     a_firstPerson) const
	{
		if (auto& cache = GetSkeletonCache(a_firstPerson))
		{
			auto it = cache->find(a_name);
			if (it != cache->end())
			{
				return it->second.transform;
			}
		}

		return {};
	}

	void ActorObjectHolder::UpdateAllAnimationGraphs(
		const BSAnimationUpdateData& a_data) const
	{
		visit([&](auto& a_e) [[msvc::forceinline]] {
			if (auto& state = a_e.data.state)
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

	bool ActorObjectHolder::GetSheathNodes(
		Data::ObjectSlot             a_slot,
		std::pair<NiNode*, NiNode*>& a_out) const
	{
		GearNodeID id;

		switch (a_slot)
		{
		case Data::ObjectSlot::k1HSword:
			id = GearNodeID::k1HSword;
			break;
		case Data::ObjectSlot::k1HSwordLeft:
			id = GearNodeID::k1HSwordLeft;
			break;
		case Data::ObjectSlot::k1HAxe:
			id = GearNodeID::k1HAxe;
			break;
		case Data::ObjectSlot::k1HAxeLeft:
			id = GearNodeID::k1HAxeLeft;
			break;
		case Data::ObjectSlot::k2HSword:
		case Data::ObjectSlot::k2HAxe:
			id = GearNodeID::kTwoHanded;
			break;
		case Data::ObjectSlot::kDagger:
			id = GearNodeID::kDagger;
			break;
		case Data::ObjectSlot::kDaggerLeft:
			id = GearNodeID::kDaggerLeft;
			break;
		case Data::ObjectSlot::kMace:
			id = GearNodeID::kMace;
			break;
		case Data::ObjectSlot::kMaceLeft:
			id = GearNodeID::kMaceLeft;
			break;
		case Data::ObjectSlot::kStaff:
			id = GearNodeID::kStaff;
			break;
		case Data::ObjectSlot::kStaffLeft:
			id = GearNodeID::kStaffLeft;
			break;
		case Data::ObjectSlot::kBow:
		case Data::ObjectSlot::kCrossBow:
			id = GearNodeID::kBow;
			break;
		case Data::ObjectSlot::kShield:
			id = GearNodeID::kShield;
			break;
		case Data::ObjectSlot::kAmmo:
			id = GearNodeID::kQuiver;
			break;
		default:
			return false;
		}

		auto it = std::find_if(
			m_weapNodes.begin(),
			m_weapNodes.end(),
			[&](auto& a_v) {
				return a_v.gearNodeID == id;
			});

		if (it != m_weapNodes.end())
		{
			a_out = {
				it->node.get(),
				it->node1p.get()
			};

			return true;
		}
		else
		{
			return false;
		}
	}

	bool ActorObjectHolder::QueueDisposeAllObjectEntries(
		Game::ObjectRefHandle a_handle)
	{
		using list_type = stl::forward_list<ObjectEntryBase::ActiveData>;

		list_type list;

		visit([&](auto& a_entry) {
			if (a_entry.data)
			{
				list.emplace_front(std::move(a_entry.data));
			}
		});

		if (list.empty())
		{
			return false;
		}

		struct DisposeStatesTask :
			public TaskDelegate
		{
		public:
			DisposeStatesTask(
				list_type&&              a_list,
				Game::ObjectRefHandle    a_handle,
				const NiPointer<NiNode>& a_root,
				const NiPointer<NiNode>& a_root1p,
				ObjectDatabase&          a_db) :
				m_list(std::move(a_list)),
				m_handle(a_handle),
				m_root(a_root),
				m_root1p(a_root1p),
				m_db(a_db)
			{
			}

			virtual void Run() override
			{
				if (m_handle)
				{
					NiPointer<TESObjectREFR> ref;
					(void)m_handle.LookupZH(ref);
				}

				for (auto& e : m_list)
				{
					e.Cleanup(m_handle, m_root, m_root1p, m_db);
				}
			}

			virtual void Dispose() override
			{
				delete this;
			}

		private:
			list_type             m_list;
			Game::ObjectRefHandle m_handle;
			NiPointer<NiNode>     m_root;
			NiPointer<NiNode>     m_root1p;
			ObjectDatabase&       m_db;
		};

		ITaskPool::AddPriorityTask<DisposeStatesTask>(
			std::move(list),
			a_handle,
			m_root,
			m_root1p,
			m_owner);

		return true;
	}

	void ActorObjectHolder::CreateExtraMovNodes(
		NiNode* a_npcroot)
	{
		for (auto& v : NodeOverrideData::GetExtraMovNodes())
		{
			if (m_cmeNodes.contains(v.name_cme) ||
			    m_movNodes.contains(v.name_mov) ||
			    a_npcroot->GetObjectByName(v.bsname_node))
			{
				return;
			}

			auto target = FindNode(a_npcroot, v.name_parent);
			if (!target)
			{
				return;
			}

			auto it = std::find_if(
				v.skel.begin(),
				v.skel.end(),
				[&](auto& a_v) {
					return a_v.match.test(m_skeletonID);
				});

			if (it != v.skel.end())
			{
				auto result = AttachExtraNodes(target, v, *it);

				m_cmeNodes.try_emplace(v.name_cme, result.cme, result.cme->m_localTransform);
				m_movNodes.try_emplace(v.name_mov, result.mov, v.placementID);
			}
		}
	}

	void ActorObjectHolder::CreateExtraCopyNode(
		NiNode*                                       a_npcroot,
		const NodeOverrideData::extraNodeCopyEntry_t& a_entry) const
	{
		if (a_npcroot->GetObjectByName(a_entry.dst))
		{
			return;
		}

		auto source = FindNode(a_npcroot, a_entry.bssrc);
		if (!source)
		{
			return;
		}

		auto parent = source->m_parent;
		if (!parent)
		{
			return;
		}

		auto node = CreateAttachmentNode(a_entry.dst);

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

		UpdateDownwardPass(node);
	}

	void ActorObjectHolder::ApplyXP32NodeTransformOverrides(NiNode* a_root) const
	{
		auto& id = m_skeletonID.id();
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

		auto& ver = m_skeletonID.xp_version();
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
			if (auto node = FindNode(a_root, e.name))
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
				m_owner.QueueReSinkAnimationGraphs(m_actorid);
			}
			else
			{
				if (auto newAA = GetNewActiveAnimation(a_event))
				{
					if (m_activeAnimation.load() != newAA.value())
					{
						m_activeAnimation = newAA.value();

						m_owner.QueueRequestEvaluate(m_actorid, true, true);
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