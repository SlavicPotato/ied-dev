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
	std::atomic_llong ActorObjectHolder::m_lfsc_delta{ 0ll };

	ActorObjectHolder::ActorObjectHolder(
		const stl::optional<Data::actorStateEntry_t>& a_playerState,
		Actor*                                        a_actor,
		NiNode*                                       a_root,
		NiNode*                                       a_npcroot,
		IObjectManager&                               a_owner,
		Game::ObjectRefHandle                         a_handle,
		bool                                          a_nodeOverrideEnabled,
		bool                                          a_nodeOverrideEnabledPlayer,
		Data::actorStateHolder_t&                     a_actorState) :
		m_owner(a_owner),
		m_handle(a_handle),
		m_actor(a_actor),
		m_root(a_root),
		m_npcroot(a_npcroot),
		m_formid(a_actor->formID),
		m_cellAttached(a_actor->IsParentCellAttached()),
		m_locData{
			a_actor->IsInInteriorCell(),
			a_actor->GetParentCellWorldspace()
		},
		m_currentPackage(a_actor->GetCurrentPackage()),
		m_created(IPerfCounter::Query())
	{
		m_lastLFStateCheck = m_created +
		                     m_lfsc_delta.fetch_add(
								 IPerfCounter::T(50000),
								 std::memory_order_relaxed) %
		                         IPerfCounter::T(1250000);

		m_skeletonCache = SkeletonCache::GetSingleton().Get(a_actor);

		if (auto npc = a_actor->GetActorBase())
		{
			m_female = npc->GetSex() == 1;
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
						GetCachedOrDefaultTransform(e->second.name));
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
				for (auto& e : NodeOverrideData::GetExtraNodes())
				{
					CreateExtraNodes(a_npcroot, m_female, e);
				}
			}

			for (auto& e : NodeOverrideData::GetExtraCopyNodes())
			{
				CreateExtraCopyNode(a_actor, a_npcroot, e);
			}
		}

		using enum_type = std::underlying_type_t<Data::ObjectSlot>;

		for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
		{
			m_entriesSlot[i].slotid   = static_cast<Data::ObjectSlot>(i);
			m_entriesSlot[i].slotidex = Data::ItemData::SlotToExtraSlot(
				static_cast<Data::ObjectSlot>(i));
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

		visit([&](auto& a_entry) {
			if (!a_entry.state)
			{
				return;
			}

			if (!handle)
			{
				handle = GetHandle();

				NiPointer<TESObjectREFR> refr;
				(void)handle->LookupZH(refr);
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

	bool ActorObjectHolder::IsActorNPC(Game::FormID a_npc) const
	{
		auto handle = GetHandle();

		NiPointer<TESObjectREFR> refr;
		if (handle.Lookup(refr))
		{
			if (auto actor = refr->As<Actor>())
			{
				if (auto npc = actor->GetActorBase())
				{
					return (npc->formID == a_npc);
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

	NiTransform ActorObjectHolder::GetCachedOrDefaultTransform(
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

	void ActorObjectHolder::CreateExtraNodes(
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

		m_cmeNodes.try_emplace(a_entry.name_cme, cme);
		m_movNodes.try_emplace(a_entry.name_mov, mov);
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

	void objectEntryBase_t::reset(
		Game::ObjectRefHandle a_handle,
		NiNode*               a_root)
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
					NiNode*                  a_root) :
					m_state(std::move(a_state)),
					m_handle(a_handle),
					m_root(a_root)
				{
				}

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
			state->CleanupObjects(a_handle);
			state.reset();
		}
	}

	void objectEntryBase_t::State::CleanupObjects(Game::ObjectRefHandle a_handle)
	{
		for (auto& e : groupObjects)
		{
			EngineExtensions::CleanupObjectImpl(
				a_handle,
				e.second.rootNode);
		}

		EngineExtensions::CleanupObjectImpl(
			a_handle,
			nodes.rootNode);
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

	void effectShaderData_t::clear()
	{
		if (!data.empty())
		{
			visit_nodes([](auto& a_entry, auto& a_prop) {
				if (a_prop->effectData == a_entry.shaderData)
				{
					a_prop->ClearEffectShaderData();
				}
			});

			data.clear();
		}

		tag.reset();
	}

	bool effectShaderData_t::UpdateIfChanged(
		NiNode*                                 a_object,
		const Data::configEffectShaderHolder_t& a_data)
	{
		if (tag == a_data)
		{
			return false;
		}
		else
		{
			Update(a_object, a_data);
			return true;
		}
	}

	void effectShaderData_t::Update(
		NiNode*                                 a_object,
		const Data::configEffectShaderHolder_t& a_data)
	{
		clear();

		for (auto& [i, e] : a_data.data)
		{
			Entry tmp;

			std::optional<std::set<BSFixedString>> tset;

			Util::Node::TraverseGeometry(a_object, [&](BSGeometry* a_geometry) {
				if (auto& effect = a_geometry->m_spEffectState)
				{
					if (auto shaderProp = NRTTI<BSShaderProperty>()(effect.get()))
					{
						if (!e.targetNodes.empty())
						{
							if (!tset)
							{
								tset.emplace();

								for (auto& f : e.targetNodes)
								{
									tset->emplace(f.c_str());
								}
							}

							if (!tset->contains(a_geometry->m_name))
							{
								return Util::Node::VisitorControl::kContinue;
							}
						}

						tmp.nodes.emplace_back(shaderProp);
					}
				}

				return Util::Node::VisitorControl::kContinue;
			});

			if (tmp.nodes.empty())
			{
				continue;
			}

			if (!e.create_shader_data(tmp.shaderData))
			{
				continue;
			}

			if (e.flags.test(Data::EffectShaderDataFlags::kForce))
			{
				tmp.flags.set(effectShaderData_t::EntryFlags::kForce);
			}

			data.emplace_back(std::move(tmp));
		}

		tag = a_data;
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

			return a_object->GetAsBSGeometry() ?
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
                               (a_object->GetAsBSGeometry() ?
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

}