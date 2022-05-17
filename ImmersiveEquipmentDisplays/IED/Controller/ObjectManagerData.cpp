#include "pch.h"

#include "INodeOverride.h"
#include "ObjectManagerData.h"

#include "Controller.h"

#include "IED/ActorState.h"
#include "IED/Data.h"
#include "IED/EngineExtensions.h"
#include "IED/ProcessParams.h"

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
		bool                  a_animEventForwarding) :
		m_owner(a_owner),
		m_handle(a_handle),
		m_actor(a_actor),
		m_root(a_root),
		m_npcroot(a_npcroot),
		m_formid(a_actor->formID),
		m_enableAnimEventForwarding(a_animEventForwarding),
		m_animationUpdateList(std::make_unique<AnimationGraphManagerHolderList>()),
		m_cellAttached(a_actor->IsParentCellAttached()),
		m_locData{
			a_actor->IsInInteriorCell(),
			a_actor->GetParentCellWorldspace()
		},
		m_currentPackage(a_actor->GetCurrentPackage()),
		m_inCombat(Game::GetActorInCombat(a_actor)),
		m_cflags1(a_actor->flags1 & ACTOR_CHECK_FLAGS_1),
		m_cflags2(a_actor->flags2 & ACTOR_CHECK_FLAGS_2),
		m_created(IPerfCounter::Query())
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

		/*PerfTimer pt;
		pt.Start();*/

		m_skeletonCache = SkeletonCache::GetSingleton().Get(a_actor);

		//_DMESSAGE("%.8X: %f", a_actor->formID.get(), pt.Stop());

		if (auto npc = a_actor->GetActorBase())
		{
			m_female = npc->GetSex() == 1;
		}

		for (auto& e : NodeOverrideData::GetExtraCopyNodes())
		{
			CreateExtraCopyNode(
				a_actor,
				a_npcroot,
				e);
		}

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
						node,
						GetCachedOrZeroTransform(e->second.name));
				}
			}

			for (auto& e : NodeOverrideData::GetMOVNodeData().getvec())
			{
				if (auto node = ::Util::Node::FindNode(a_npcroot, e->second.bsname))
				{
					m_movNodes.try_emplace(
						e->first,
						node,
						e->second.placementID);
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
							defParentNode,
							e->second.animSlot,
							e->second.vanilla ?
								GetCachedTransform(e->first) :
                                std::optional<NiTransform>{});
					}
				}
			}
		}

		using enum_type = std::underlying_type_t<Data::ObjectSlot>;

		for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
		{
			m_entriesSlot[i].slotid   = static_cast<Data::ObjectSlot>(i);
			m_entriesSlot[i].slotidex = Data::ItemData::SlotToExtraSlot(
				static_cast<Data::ObjectSlot>(i));
		}

		if (m_enableAnimEventForwarding)
		{
			RE::BSAnimationGraphManagerPtr agm;
			if (a_actor->GetAnimationGraphManagerImpl(agm))
			{
				if (!agm->graphs.empty())
				{
					if (auto& e = agm->graphs.front())
					{
						e->AddEventSink(this);
					}
				}
			}
		}
	}

	ActorObjectHolder::~ActorObjectHolder()
	{
		if (m_enableAnimEventForwarding)
		{
			RE::BSAnimationGraphManagerPtr agm;
			if (m_actor->GetAnimationGraphManagerImpl(agm))
			{
				for (auto& e : agm->graphs)
				{
					if (e)
					{
						e->RemoveEventSink(this);
					}
				}
			}

			m_animEventForwardRegistrations.Clear();
		}

		m_animationUpdateList->Clear();

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

		stl::optional<Game::ObjectRefHandle> handle;

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

	void ActorObjectHolder::CreateExtraMovNodes(
		NiNode*                                   a_npcroot,
		bool                                      a_female,
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

		m_cmeNodes.try_emplace(a_entry.name_cme, cme, cme->m_localTransform);
		m_movNodes.try_emplace(a_entry.name_mov, mov, a_entry.placementID);
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

	void ActorObjectHolder::ApplyNodeTransformOverrides(NiNode* a_root) const
	{
		auto skelIDExtraData = a_root->GetExtraData<NiIntegerExtraData>(
			NodeOverrideData::GetSkelIDExtraDataName());

		if (!skelIDExtraData)
		{
			return;
		}

		if (skelIDExtraData->m_data != 628145516 &&  // female
		    skelIDExtraData->m_data != 1361955)      // male
		{
			return;
		}

		auto npcNode = ::Util::Node::FindChildNode(a_root, NodeOverrideData::GetNPCNodeName());
		if (!npcNode)
		{
			return;
		}

		auto xpExtraData = npcNode->GetExtraData<NiFloatExtraData>(
			NodeOverrideData::GetXPMSEExtraDataName());

		if (!xpExtraData)
		{
			return;
		}

		if (xpExtraData->m_data < 3.6f)
		{
			return;
		}

		for (auto& e : NodeOverrideData::GetTransformOverrideData())
		{
			if (auto node = ::Util::Node::FindNode(npcNode, e.name))
			{
				node->m_localTransform.rot = e.rot;
			}
		}
	}

	EventResult ActorObjectHolder::ReceiveEvent(
		const BSAnimationGraphEvent*           a_event,
		BSTEventSource<BSAnimationGraphEvent>* a_eventSource)
	{
		if (a_event)
		{
			/*PerfTimer pt;
			pt.Start();*/

			auto sh = BSStringHolder::GetSingleton();

			if (a_event->tag == sh->m_graphDeleting)
			{
				m_owner.QueueReSinkAnimationGraphs(m_formid);
			}
			else
			{
				if (!sh->m_animEventFilter.contains(a_event->tag))
				{
					m_animEventForwardRegistrations.Notify(a_event->tag);
				}
			}

			//_DMESSAGE("%f | %X : %s | %s", pt.Stop(), m_formid.get(), a_event->payload.c_str(), a_event->tag.c_str());
		}

		return EventResult::kContinue;
	}

	void ActorObjectHolder::ReSinkAnimationGraphs()
	{
		if (!m_enableAnimEventForwarding)
		{
			return;
		}

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
	}

	void ActorObjectHolder::RegisterWeaponAnimationGraphManagerHolder(
		RE::WeaponAnimationGraphManagerHolderPtr& a_ptr,
		bool                                      a_forward)
	{
		if (a_forward && m_enableAnimEventForwarding)
		{
			m_animEventForwardRegistrations.Add(a_ptr);
		}

		m_animationUpdateList->Add(a_ptr);

		//_DMESSAGE("reg %p", a_ptr.get());
	}

	void ActorObjectHolder::UnregisterWeaponAnimationGraphManagerHolder(
		RE::WeaponAnimationGraphManagerHolderPtr& a_ptr)
	{
		if (m_enableAnimEventForwarding)
		{
			m_animEventForwardRegistrations.Remove(a_ptr);
		}

		m_animationUpdateList->Remove(a_ptr);

		//_DMESSAGE("unreg %p", a_ptr.get());
	}

	void objectEntryBase_t::reset(
		Game::ObjectRefHandle a_handle,
		NiPointer<NiNode>&    a_root)
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
					Game::ObjectRefHandle    a_handle,
					NiPointer<NiNode>&       a_root) :
					m_state(std::move(a_state)),
					m_handle(a_handle),
					m_root(a_root)
				{
				}

				virtual void Run() override
				{
					m_state->Cleanup(m_handle);

					m_state.reset();
					m_root.reset();
				}

				virtual void Dispose() override
				{
					delete this;
				}

			private:
				std::unique_ptr<State> m_state;
				Game::ObjectRefHandle  m_handle;
				NiPointer<NiNode>      m_root;
			};

			ITaskPool::AddPriorityTask<DisposeStateTask>(
				std::move(state),
				a_handle,
				a_root);
		}
		else
		{
			state->Cleanup(a_handle);
			state.reset();
		}
	}

	void objectEntryBase_t::State::Cleanup(Game::ObjectRefHandle a_handle)
	{
		for (auto& e : groupObjects)
		{
			EngineExtensions::CleanupObjectImpl(
				a_handle,
				e.second.rootNode);

			if (e.second.weapAnimGraphManagerHolder)
			{
				EngineExtensions::CleanupWeaponBehaviorGraph(
					e.second.weapAnimGraphManagerHolder);
			}
		}

		EngineExtensions::CleanupObjectImpl(
			a_handle,
			nodes.rootNode);

		if (weapAnimGraphManagerHolder)
		{
			EngineExtensions::CleanupWeaponBehaviorGraph(
				weapAnimGraphManagerHolder);
		}
	}

	void objectEntryBase_t::State::GroupObject::PlayAnimation(
		Actor*                   a_actor,
		const stl::fixed_string& a_sequence)
	{
		if (a_sequence.empty())
		{
			return;
		}

		if (!object)
		{
			return;
		}

		if (auto controller = object->GetControllers())
		{
			if (auto manager = controller->AsNiControllerManager())
			{
				if (auto nseq = manager->GetSequenceByName(a_sequence.c_str()))
				{
					a_actor->PlayAnimation(
						manager,
						nseq,
						nseq);
				}
			}
		}
	}

	void objectEntryBase_t::State::UpdateAndPlayAnimation(
		Actor*                   a_actor,
		const stl::fixed_string& a_sequence)
	{
		if (a_sequence.empty())
		{
			return;
		}

		if (a_sequence == currentSequence)
		{
			return;
		}

		if (!nodes.object)
		{
			return;
		}

		if (auto controller = nodes.object->GetControllers())
		{
			if (auto manager = controller->AsNiControllerManager())
			{
				if (auto nseq = manager->GetSequenceByName(a_sequence.c_str()))
				{
					auto cseq = !currentSequence.empty() ?
					                manager->GetSequenceByName(currentSequence.c_str()) :
                                    nullptr;

					a_actor->PlayAnimation(
						manager,
						nseq,
						cseq ? cseq : nseq);

					currentSequence = a_sequence;
				}
			}
		}
	}

	bool cmeNodeEntry_t::find_visible_geometry(
		NiAVObject*           a_object,
		const BSStringHolder* a_sh) noexcept
	{
		using namespace ::Util::Node;

		auto result = Traverse(a_object, [a_sh](NiAVObject* a_object) {
			if (a_object->IsHidden())
			{
				return VisitorControl::kSkip;
			}

			if (a_sh)
			{
				if (a_object->m_name == a_sh->m_scb ||
				    a_object->m_name == a_sh->m_scbLeft)
				{
					return VisitorControl::kSkip;
				}
			}

			return a_object->AsGeometry() ?
			           VisitorControl::kStop :
                       VisitorControl::kContinue;
		});

		return result == VisitorControl::kStop;
	}

	bool cmeNodeEntry_t::has_visible_geometry(
		const BSStringHolder* a_sh) const noexcept
	{
		return find_visible_geometry(node, a_sh);
	}

	bool cmeNodeEntry_t::has_visible_object(
		NiAVObject* a_findObject) const noexcept
	{
		using namespace ::Util::Node;

		struct
		{
			NiAVObject* findObject;
			bool        result{ false };
		} args{
			a_findObject
		};

		Traverse(node, [&args](NiAVObject* a_object) {
			if (args.findObject == a_object)
			{  // object found, verify is has visible geometry

				auto visitorResult = Traverse(a_object, [](NiAVObject* a_object) {
					return a_object->IsHidden() ?
					           VisitorControl::kSkip :
                               (a_object->AsGeometry() ?
					                VisitorControl::kStop :
                                    VisitorControl::kContinue);
				});

				args.result = (visitorResult == VisitorControl::kStop);

				return VisitorControl::kStop;
			}
			else
			{
				return a_object->IsHidden() ?
				           VisitorControl::kSkip :
                           VisitorControl::kContinue;
			}
		});

		return args.result;
	}

	void ObjectManagerData::ApplyActorState(
		ActorObjectHolder& a_holder)
	{
		if (a_holder.m_actor == *g_thePlayer && m_playerState)
		{
			a_holder.ApplyActorState(*m_playerState);
		}
		else
		{
			auto it = m_storedActorStates.data.find(a_holder.m_actor->formID);
			if (it != m_storedActorStates.data.end())
			{
				a_holder.ApplyActorState(it->second);

				m_storedActorStates.data.erase(it);
			}
		}
	}

	void objectEntryBase_t::AnimationState::UpdateAndSendAnimationEvent(
		const stl::fixed_string& a_event)
	{
		assert(weapAnimGraphManagerHolder);

		if (a_event.empty())
		{
			return;
		}

		if (a_event != currentAnimationEvent)
		{
			currentAnimationEvent = a_event;
			weapAnimGraphManagerHolder->NotifyAnimationGraph(a_event.c_str());
		}
	}

	void AnimationGraphManagerHolderList::Update(const BSAnimationUpdateData& a_data) const
	{
		stl::scoped_lock lock(m_lock);

		for (auto& e : m_data)
		{
			EngineExtensions::UpdateAnimationGraph(e.get(), a_data);
		}
	}

}