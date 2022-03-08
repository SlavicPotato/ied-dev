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
			m_entriesSlot[i].slotid = static_cast<Data::ObjectSlot>(i);
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
				if (auto npc = Game::GetActorBase(actor))
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
				if (auto race = Game::GetActorRace(actor))
				{
					return (race->formID == a_race);
				}
			}
		}

		return false;
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
			EngineExtensions::CleanupNodeImpl(
				a_handle,
				e.second.object);
		}

		EngineExtensions::CleanupNodeImpl(
			a_handle,
			nodes.obj);
	}

	void objectEntryBase_t::State::GroupObject::PlayAnimation(
		Actor*                   a_actor,
		const stl::fixed_string& a_sequence)
	{
		if (a_sequence.empty())
		{
			return;
		}

		if (!main)
		{
			return;
		}

		if (auto controller = main->GetControllers())
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

		if (!nodes.main)
		{
			return;
		}

		if (auto controller = nodes.main->GetControllers())
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
			visit_nodes([](auto& a_e1, auto& a_e2) {
				a_e2.first->ClearEffectShaderData();
			});

			data.clear();
		}

		tag.reset();
	}

	void effectShaderData_t::Update(
		NiNode*                                 a_object,
		const uuid_tag&                         a_tag,
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
					auto shaderProp = ni_cast(effect.get(), BSShaderProperty);

					if (shaderProp && shaderProp->AcceptsEffectData())
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

						tmp.nodes.emplace_back(shaderProp, a_geometry);
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

			data.emplace(i, std::move(tmp));
		}

		tag = a_tag;
	}

}