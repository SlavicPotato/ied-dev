#include "pch.h"

#include "ActorObjectHolder.h"

#include "INodeOverride.h"
#include "IObjectManager.h"
#include "ObjectManagerData.h"

#include "IED/ActorState.h"
#include "IED/AnimationUpdateController.h"
#include "IED/EngineExtensions.h"
#include "IED/ExtraNodes.h"
#include "IED/ReferenceLightController.h"
#include "IED/StringHolder.h"
#include "IED/TransformOverrides.h"

#include <ext/BSAnimationUpdateData.h>
#include <ext/Node.h>

namespace IED
{
	using namespace SkeletonExtensions;
	using namespace ::Util::Node;

	std::atomic_ullong ActorObjectHolder::m_lfsc_delta_lf{ 0ull };
	std::atomic_ullong ActorObjectHolder::m_lfsc_delta_mf{ 0ull };
	std::atomic_ullong ActorObjectHolder::m_lfsc_delta_hf{ 0ull };

	namespace detail
	{
		SKMP_143_CONSTEXPR auto make_object_slot_array(
			BipedSlotData::display_slot_data_type& a_1) noexcept
		{
			return stl::make_array<
				ObjectEntrySlot,
				stl::underlying(Data::ObjectSlot::kMax)>([&]<std::size_t I>() noexcept {
				constexpr auto slotid = static_cast<Data::ObjectSlot>(I);

				return ObjectEntrySlot::tuple_init_type(
					a_1[I],
					slotid,
					Data::ItemData::SlotToExtraSlot(slotid));
			});
		}
	}

	ActorObjectHolder::ActorObjectHolder(
		Actor*                  a_actor,
		TESNPC*                 a_npc,
		TESRace*                a_race,
		NiNode*                 a_root,
		NiNode*                 a_npcroot,
		IObjectManager&         a_owner,
		Game::ObjectRefHandle   a_handle,
		bool                    a_nodeOverrideEnabled,
		bool                    a_nodeOverrideEnabledPlayer,
		bool                    a_syncToFirstPersonSkeleton,
		const BipedSlotDataPtr& a_slotCache) noexcept :
		m_owner(a_owner),
		m_handle(a_handle),
		m_actor(a_actor),
		m_root(a_root),
		m_npcroot(a_npcroot),
		m_actorid(a_actor->formID),
		m_npcid(a_npc->formID),
		m_npcTemplateId(a_npc->GetFirstNonTemporaryOrThis()->formID),
		m_raceid(a_race->formID),
		m_created(IPerfCounter::Query()),
		m_slotCache(a_slotCache),
		m_skeletonID(a_root),
		m_state(a_actor),
		m_entriesSlot{ detail::make_object_slot_array(a_slotCache->displays) },
		m_temp(std::make_unique_for_overwrite<ActorTempData>())
	{
		m_flags.set(ActorObjectHolderFlags::kIsFemale, a_npc->GetSex() == 1);
		m_flags.set(ActorObjectHolderFlags::kIsPlayer, a_actor == *g_thePlayer);

		auto interval = IPerfCounter::T(STATE_CHECK_INTERVAL_LOW);

		m_nextLFStateCheck = m_created +
		                     m_lfsc_delta_lf.fetch_add(
								 interval / 20,
								 std::memory_order_relaxed) %
		                         interval;

		interval = IPerfCounter::T(STATE_CHECK_INTERVAL_MH);

		m_nextMFStateCheck = m_created +
		                     m_lfsc_delta_mf.fetch_add(
								 IPerfCounter::T(interval / 20),
								 std::memory_order_relaxed) %
		                         IPerfCounter::T(interval);

		interval = IPerfCounter::T(STATE_CHECK_INTERVAL_HIGH);

		m_nextHFStateCheck = m_created +
		                     m_lfsc_delta_hf.fetch_add(
								 IPerfCounter::T(interval / 20),
								 std::memory_order_relaxed) %
		                         IPerfCounter::T(interval);

		if (auto root1p = a_actor->Get3D1(true); root1p && root1p != a_root)
		{
			m_root1p = root1p->AsNode();
		}

		auto                      skeletonCache = SkeletonCache::GetSingleton().Get(a_actor);
		SkeletonCache::ActorEntry skeletonCache1p;

		if (m_root1p && a_syncToFirstPersonSkeleton)
		{
			skeletonCache1p = SkeletonCache::GetSingleton().Get(a_actor, true);
		}

		if (NodeOverrideData::GetHumanoidSkeletonSignatures()
		        .contains(m_skeletonID.signature()))
		{
			m_flags.set(ActorObjectHolderFlags::kHumanoidSkeleton);
		}

		for (auto& e : NodeOverrideData::GetExtraCopyNodes())
		{
			CreateExtraCopyNode(skeletonCache, a_npcroot, e);
		}

		if (a_nodeOverrideEnabled &&
		    (a_nodeOverrideEnabledPlayer ||
		     a_actor != *g_thePlayer))
		{
			const auto npcroot1p =
				m_root1p ?
					GetNodeByName(
						m_root1p,
						BSStringHolder::GetSingleton()->m_npcroot) :
					nullptr;

			if (!EngineExtensions::HasEarly3DLoadHooks())
			{
				CreateExtraMovNodes(a_npcroot, m_skeletonID);
				if (npcroot1p)
				{
					SkeletonID skelid1p(npcroot1p);

					CreateExtraMovNodes(npcroot1p, skelid1p);
				}
			}

			for (auto& e : NodeOverrideData::GetMonitorNodeData())
			{
				if (auto node = GetNodeByName(a_npcroot, e))
				{
					m_monitorNodes.emplace_back(
						node,
						node->m_parent,
						node->m_children.size(),
						node->IsVisible());
				}
			}

			for (auto& e : NodeOverrideData::GetCMENodeData().getvec())
			{
				if (auto node = GetNodeByName(a_npcroot, e->second.bsname))
				{
					m_cmeNodes.try_emplace(
								  e->first,
								  node,
								  a_syncToFirstPersonSkeleton ? npcroot1p : nullptr,
								  skeletonCache,
								  skeletonCache1p,
								  e->second.name,
								  e->second.bsname)
						.first->second;
				}
			}

			for (auto& e : NodeOverrideData::GetMOVNodeData().getvec())
			{
				if (auto node = GetNodeByName(a_npcroot, e->second.bsname))
				{
					m_movNodes.try_emplace(
						e->first,
						node,
						a_syncToFirstPersonSkeleton ? npcroot1p : nullptr,
						skeletonCache,
						skeletonCache1p,
						e->second.name,
						e->second.bsname,
						e->second.placementID);
				}
			}

			for (auto& e : NodeOverrideData::GetWeaponNodeData().getvec())
			{
				if (auto node = GetNodeByName(a_npcroot, e->second.bsname))
				{
					NiNode* defParentNode1p = nullptr;
					NiNode* node1p          = nullptr;

					if (npcroot1p)
					{
						node1p = GetNodeByName(npcroot1p, e->second.bsname);

						if (node1p && a_syncToFirstPersonSkeleton)
						{
							defParentNode1p = GetNodeByName(npcroot1p, e->second.bsdefParent);
						}
					}

					m_weapNodes.emplace_back(
						e->first,
						node,
						GetNodeByName(a_npcroot, e->second.bsdefParent),
						node1p,
						defParentNode1p,
						e->second.animSlot,
						e->second.nodeID);
				}
			}
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

			if (auto parent = GetNodeByName(a_npcroot, e.parent))
			{
				auto& r = m_nodeMonitorEntries.try_emplace(
												  i,
												  parent,
												  e)
				              .first->second;

				r.Update();
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

	ActorObjectHolder::~ActorObjectHolder() noexcept
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

		m_slotCache->accessed = m_owner.IncrementCounter();

		const bool defer = EngineExtensions::ShouldDefer3DTask();

		/*if (!defer)
		{
			gLog.Debug("%s: immediate cleanup: %X ||| %p | %d", __FUNCTION__, m_actorid.get(), m_actor->loadedState, m_flags.test(ActorObjectHolderFlags::kDestroyed));
		}
		else
		{
			gLog.Debug("%s: DEFERRED cleanup: %X ||| %p | %d", __FUNCTION__, m_actorid.get(), m_actor->loadedState, m_flags.test(ActorObjectHolderFlags::kDestroyed));
		}*/

		if (m_actor->loadedState && !m_flags.test(ActorObjectHolderFlags::kDestroyed))
		{
			for (const auto& e : m_cmeNodes)
			{
				INodeOverride::ResetNodeOverride(e.second, defer);
			}

			for (const auto& e : m_weapNodes)
			{
				INodeOverride::ResetNodePlacement(e, nullptr, defer);
			}
		}

		m_simNodeList.clear();

		ReferenceLightController::GetSingleton().RemoveActor(m_actorid);
		AnimationUpdateController::GetSingleton().RemoveActor(m_actorid);

		if (defer)
		{
			QueueDisposeMOVSimComponents();
			QueueDisposeAllObjectEntries(GetHandle());
		}
		else
		{
			std::optional<Game::ObjectRefHandle> handle;
			NiPointer<TESObjectREFR>             ref;

			visit([&](auto& a_entry) noexcept {
				if (!a_entry.data)
				{
					return;
				}

				if (!handle)
				{
					handle.emplace(GetHandle());
					(void)handle->LookupZH(ref);
				}

				a_entry.reset(
					*handle,
					m_root,
					m_root1p,
					m_owner,
					false);
			});
		}
	}

	bool ActorObjectHolder::IsAnySlotOccupied() const noexcept
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
		std::size_t result = 0;

		for (auto& e : m_entriesCustom)
		{
			for (auto& f : e)
			{
				for (auto& g : f.second)
				{
					if (g.second.data.state)
					{
						result++;
					}
				}
			}
		}

		return result;
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

	float ActorObjectHolder::GetRandomPercent(const luid_tag& a_luid) noexcept
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

		const auto value = m_owner.GetRandomPercent();

		r.first->second = value;

		return value;
	}

	void ActorObjectHolder::ClearRPC() noexcept
	{
		m_rpc.clear();
	}

	bool ActorObjectHolder::UpdateNodeMonitorEntries() noexcept
	{
		bool result = false;

		for (const auto& e : m_nodeMonitorEntries.getvec())
		{
			result |= e->second.Update();
		}

		return result;
	}

	bool ActorObjectHolder::GetNodeMonitorResult(std::uint32_t a_uid) const noexcept
	{
		auto it = m_nodeMonitorEntries.find(a_uid);
		return it != m_nodeMonitorEntries.end() ?
		           it->second.IsPresent() :
		           false;
	}

	bool ActorObjectHolder::GetSheathNodes(
		Data::ObjectSlot             a_slot,
		std::pair<NiNode*, NiNode*>& a_out) const noexcept
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
			[&](auto& a_v) [[msvc::forceinline]] {
				return a_v.gearNodeID == id;
			});

		if (it != m_weapNodes.end())
		{
			a_out = {
				it->node3p.node.get(),
				it->node1p.node.get()
			};

			return true;
		}
		else
		{
			return false;
		}
	}

	void ActorObjectHolder::QueueDisposeMOVSimComponents() noexcept
	{
		for (auto& e : m_movNodes)
		{
			if (auto& sc = e.second.thirdPerson.simComponent)
			{
				ITaskPool::AddPriorityTask<
					ITaskPool::SimpleDisposeTask<
						std::remove_cvref_t<
							decltype(sc)>>>(std::move(sc));
			}

			if (auto& sc = e.second.firstPerson.simComponent)
			{
				ITaskPool::AddPriorityTask<
					ITaskPool::SimpleDisposeTask<
						std::remove_cvref_t<
							decltype(sc)>>>(std::move(sc));
			}
		}
	}

	bool ActorObjectHolder::QueueDisposeAllObjectEntries(
		Game::ObjectRefHandle a_handle) noexcept
	{
		using list_type = stl::forward_list<ObjectEntryBase::ObjectEntryData>;

		list_type list;

		visit([&](auto& a_entry) noexcept {
			if (a_entry.data)
			{
				if (auto& state = a_entry.data.state)
				{
					if (auto& sc = state->simComponent)
					{
						RemoveSimComponent(sc);
					}
				}

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
				IObjectManager&          a_db) noexcept :
				m_list(std::move(a_list)),
				m_handle(a_handle),
				m_root(a_root),
				m_root1p(a_root1p),
				m_db(a_db)
			{
			}

			virtual void Run() noexcept override
			{
				if (m_handle)
				{
					NiPointer<TESObjectREFR> ref;
					(void)m_handle.LookupZH(ref);
				}

				stl::lock_guard lock(m_db.GetLock());

				for (auto& e : m_list)
				{
					e.Cleanup(m_handle, m_root, m_root1p, m_db);
				}
			}

			virtual void Dispose() noexcept override
			{
				delete this;
			}

		private:
			list_type             m_list;
			Game::ObjectRefHandle m_handle;
			NiPointer<NiNode>     m_root;
			NiPointer<NiNode>     m_root1p;
			IObjectManager&       m_db;
		};

		ITaskPool::AddPriorityTask<DisposeStatesTask>(
			std::move(list),
			a_handle,
			m_root,
			m_root1p,
			m_owner);

		return true;
	}

	void ActorObjectHolder::SimReadTransforms(float a_step) const noexcept
	{
		for (auto& e : m_simNodeList)
		{
			e->ReadTransforms(a_step);
		}
	}

	void ActorObjectHolder::SimWriteTransforms() const noexcept
	{
		for (auto& e : m_simNodeList)
		{
			e->WriteTransforms();
		}
	}

	void ActorObjectHolder::SimUpdate(float a_step) const noexcept
	{
		const btVector3 step(_mm_set_ps1(a_step));

		for (auto& e : m_simNodeList)
		{
			e->UpdateMotion(step);
		}
	}

	std::size_t ActorObjectHolder::GetSimComponentListSize() const noexcept
	{
		return m_simNodeList.size();
	}

	std::size_t ActorObjectHolder::GetNumAnimObjects() const noexcept
	{
		std::size_t i = 0;

		visit([&](auto& a_e) {
			if (auto& state = a_e.data.state)
			{
				for (auto& e : state->groupObjects)
				{
					if (e.second.anim.holder)
					{
						i++;
					}
				}

				if (state->anim.holder)
				{
					i++;
				}
			}
		});

		return i;
	}

	void ActorObjectHolder::RemoveSimComponent(
		const stl::smart_ptr<PHYSimComponent>& a_sc) noexcept
	{
		std::erase(m_simNodeList, a_sc);
	}

	void ActorObjectHolder::RemoveAndDestroySimComponent(
		stl::smart_ptr<PHYSimComponent>& a_sc) noexcept
	{
		std::erase(m_simNodeList, a_sc);
		a_sc.reset();
	}

	void ActorObjectHolder::CreateExtraCopyNode(
		const SkeletonCache::ActorEntry&              a_sc,
		NiNode*                                       a_npcroot,
		const NodeOverrideData::extraNodeCopyEntry_t& a_entry) const noexcept
	{
		auto source = GetNodeByName(a_npcroot, a_entry.bssrc);
		if (!source)
		{
			return;
		}

		if (a_npcroot->GetObjectByName(a_entry.dst))
		{
			return;
		}

		auto parent = source->m_parent;
		if (!parent)
		{
			return;
		}

		auto node = CreateAttachmentNode(a_entry.dst);

		if (a_sc)
		{
			auto it = a_sc->find(a_entry.src);

			node->m_localTransform = it != a_sc->end() ?
			                             it->second :
			                             source->m_localTransform;
		}
		else
		{
			node->m_localTransform = source->m_localTransform;
		}

		parent->AttachChild(node, true);

		UpdateDownwardPass(node);
	}

	void ActorObjectHolder::ApplyXP32NodeTransformOverrides() const noexcept
	{
		SkeletonExtensions::ApplyXP32NodeTransformOverrides(m_npcroot.get(), m_skeletonID);
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