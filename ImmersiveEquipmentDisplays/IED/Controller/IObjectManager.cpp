#include "pch.h"

#include "Controller.h"
#include "INodeOverride.h"
#include "IObjectManager.h"

#include "IED/EngineExtensions.h"

#include <ext/Model.h>
#include <ext/Node.h>

namespace IED
{
	using namespace ::Util::Node;

	bool IObjectManager::RemoveObject(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		ObjectEntryBase&                 a_objectEntry,
		ActorObjectHolder&               a_data,
		stl::flag<ControllerUpdateFlags> a_flags,
		bool                             a_defer) noexcept
	{
		if (auto& state = a_objectEntry.data.state)
		{
			if (auto& sc = state->simComponent)
			{
				a_data.RemoveSimComponent(sc);
			}

			if (
				!a_defer &&
				m_playSound &&
				a_flags.test(ControllerUpdateFlags::kPlaySound) &&
				state->flags.test(ObjectEntryFlags::kPlaySound) &&
				a_actor &&
				a_actor->loadedState &&
				(a_actor == *g_thePlayer || m_playSoundNPC) &&
				state->nodes.rootNode->m_parent &&
				state->nodes.rootNode->IsVisible())
			{
				SoundPlay(
					state->form->formType,
					state->nodes.rootNode->m_parent,
					false);
			}
		}

		/*if (a_objectEntry.state->weapAnimGraphManagerHolder)
		{
			a_data.UnregisterWeaponAnimationGraphManagerHolder(
				a_objectEntry.state->weapAnimGraphManagerHolder);
		}

		for (auto& e : a_objectEntry.state->groupObjects)
		{
			if (e.second.weapAnimGraphManagerHolder)
			{
				a_data.UnregisterWeaponAnimationGraphManagerHolder(
					e.second.weapAnimGraphManagerHolder);
			}
		}*/

		return a_objectEntry.reset(
			a_handle,
			a_data.m_root,
			a_data.m_root1p,
			*this,
			a_defer);
	}

	bool IObjectManager::RemoveActorImpl(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		auto it = m_objects.find(a_actor->formID);
		if (it != m_objects.end())
		{
			it->second.SetHandle(a_handle);

			if (a_flags.test(ControllerUpdateFlags::kDestroyed))
			{
				it->second.MarkDestroyed();
			}

			EraseActor(it);

			return true;
		}
		else
		{
			return false;
		}
	}

	bool IObjectManager::RemoveActorImpl(
		TESObjectREFR*                   a_actor,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		auto it = m_objects.find(a_actor->formID);
		if (it != m_objects.end())
		{
			it->second.SetHandle(a_actor->GetHandle());

			if (a_flags.test(ControllerUpdateFlags::kDestroyed))
			{
				it->second.MarkDestroyed();
			}

			EraseActor(it);

			return true;
		}
		else
		{
			return false;
		}
	}

	bool IObjectManager::RemoveActorImpl(
		Game::FormID                     a_actor,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			if (a_flags.test(ControllerUpdateFlags::kDestroyed))
			{
				it->second.MarkDestroyed();
			}

			EraseActor(it);

			return true;
		}
		else
		{
			return false;
		}
	}

	/*void IObjectManager::QueueReSinkAnimationGraphs(
		Game::FormID a_actor)
	{
		ITaskPool::AddPriorityTask([this, a_actor]() {
			const boost::lock_guard lock(m_lock);

			auto it = m_objects.find(a_actor);
			if (it != m_objects.end())
			{
				it->second.ReSinkAnimationGraphs();
			}
		});
	}*/

	void IObjectManager::RequestEvaluate(
		Game::FormID a_actor,
		bool         a_defer,
		bool         a_xfrmUpdate,
		bool         a_xfrmUpdateNoDefer) const noexcept
	{
		const boost::lock_guard lock(m_lock);

		auto it = m_objects.find(a_actor);
		if (it != m_objects.end())
		{
			if (a_defer)
			{
				it->second.RequestEvalDefer();
			}
			else
			{
				it->second.RequestEval();
			}

			if (a_xfrmUpdate)
			{
				if (a_xfrmUpdateNoDefer)
				{
					it->second.RequestTransformUpdate();
				}
				else
				{
					it->second.RequestTransformUpdateDefer();
				}
			}
		}
	}

	void IObjectManager::QueueRequestEvaluate(
		Game::FormID a_actor,
		bool         a_defer,
		bool         a_xfrmUpdate,
		bool         a_xfrmUpdateNoDefer) const noexcept
	{
		ITaskPool::AddTask(
			[this,
		     a_actor,
		     a_defer,
		     a_xfrmUpdate,
		     a_xfrmUpdateNoDefer]() {
				RequestEvaluate(
					a_actor,
					a_defer,
					a_xfrmUpdate,
					a_xfrmUpdateNoDefer);
			});
	}

	void IObjectManager::QueueRequestEvaluate(
		TESObjectREFR* a_actor,
		bool           a_defer,
		bool           a_xfrmUpdate,
		bool           a_xfrmUpdateNoDefer) const noexcept
	{
		if (IsActorValid(a_actor))
		{
			QueueRequestEvaluate(
				a_actor->formID,
				a_defer,
				a_xfrmUpdate,
				a_xfrmUpdateNoDefer);
		}
	}

	void IObjectManager::QueueClearVariablesOnAll(bool a_requestEval) noexcept
	{
		ITaskPool::AddPriorityTask([this, a_requestEval] {
			const boost::lock_guard lock(m_lock);

			ClearVariablesOnAll(a_requestEval);
		});
	}

	void IObjectManager::QueueClearVariables(
		Game::FormID a_handle,
		bool         a_requestEval) noexcept
	{
		ITaskPool::AddPriorityTask([this, a_handle, a_requestEval] {
			const boost::lock_guard lock(m_lock);

			ClearVariables(a_handle, a_requestEval);
		});
	}

	void IObjectManager::QueueRequestVariableUpdateOnAll() const noexcept
	{
		ITaskPool::AddPriorityTask([this] {
			const boost::lock_guard lock(m_lock);

			RequestVariableUpdateOnAll();
		});
	}

	void IObjectManager::QueueRequestVariableUpdate(Game::FormID a_handle) const noexcept
	{
		ITaskPool::AddPriorityTask([this, a_handle] {
			const boost::lock_guard lock(m_lock);

			RequestVariableUpdate(a_handle);
		});
	}

	void IObjectManager::RemoveActorGear(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		auto it = m_objects.find(a_actor->formID);
		if (it != m_objects.end())
		{
			RemoveActorGear(a_actor, a_handle, it->second, a_flags);
		}
	}

	bool IObjectManager::RemoveActorGear(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		ActorObjectHolder&               a_holder,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		bool result = false;

		if (EngineExtensions::ShouldDefer3DTask())
		{
			result = a_holder.QueueDisposeAllObjectEntries(a_handle);
		}
		else
		{
			a_holder.visit([&](auto& a_object) {
				result |= RemoveObject(
					a_actor,
					a_handle,
					a_object,
					a_holder,
					a_flags,
					false);
			});
		}

		for (auto& e : a_holder.m_entriesCustom)
		{
			e.clear();
		}

		/*assert(a_holder.m_animationUpdateList->Empty());
		assert(a_holder.m_animEventForwardRegistrations.Empty());*/

		return result;
	}

	bool IObjectManager::RemoveInvisibleObjects(
		ActorObjectHolder&    a_holder,
		Game::ObjectRefHandle a_handle) noexcept
	{
		const bool defer = EngineExtensions::ShouldDefer3DTask();

		bool result = false;

		for (auto& e : a_holder.m_entriesSlot)
		{
			if (!e.IsNodeVisible())
			{
				RemoveObject(
					nullptr,
					a_handle,
					e,
					a_holder,
					ControllerUpdateFlags::kNone,
					defer);

				result = true;
			}
		}

		for (auto& e : a_holder.m_entriesCustom)
		{
			for (auto it1 = e.begin(); it1 != e.end();)
			{
				for (auto it2 = it1->second.begin(); it2 != it1->second.end();)
				{
					if (!it2->second.IsNodeVisible())
					{
						RemoveObject(
							nullptr,
							a_handle,
							it2->second,
							a_holder,
							ControllerUpdateFlags::kNone,
							defer);

						result = true;

						it2 = it1->second.erase(it2);
					}
					else
					{
						++it2;
					}
				}

				if (it1->second.empty())
				{
					it1 = e.erase(it1);
				}
				else
				{
					++it1;
				}
			}
		}

		return result;
	}

	void IObjectManager::ClearObjectsImpl() noexcept
	{
		m_objects.clear();
	}

	/*bool IObjectManager::ConstructArmorNode(
		TESForm*                                          a_form,
		const stl::vector<TESObjectARMA*>&                a_in,
		bool                                              a_isFemale,
		stl::vector<ObjectDatabase::ObjectDatabaseEntry>& a_dbEntries,
		NiPointer<NiNode>&                                a_out)
	{
		bool result = false;

		for (auto& e : a_in)
		{
			auto texSwap = std::addressof(e->bipedModels[a_isFemale ? 1 : 0]);
			auto path    = texSwap->GetModelName();

			if (!path || path[0] == 0)
			{
				texSwap = std::addressof(e->bipedModels[a_isFemale ? 0 : 1]);
				path    = texSwap->GetModelName();

				if (!path || path[0] == 0)
				{
					continue;
				}
			}

			NiPointer<NiNode>   object;
			ObjectDatabaseEntry entry;

			if (!GetUniqueObject(path, entry, object))
			{
				continue;
			}

			if (!a_out)
			{
				a_out          = NiNode::Create(1);
				a_out->m_flags = NiAVObject::kFlag_SelectiveUpdate |
				                 NiAVObject::kFlag_SelectiveUpdateTransforms |
				                 NiAVObject::kFlag_kSelectiveUpdateController;
			}

			char buffer[INode::NODE_NAME_BUFFER_SIZE];

			stl::snprintf(
				buffer,
				"OBJECT ARMA [%.8X/%.8X]",
				a_form->formID.get(),
				e->formID.get());

			object->m_name = buffer;

			EngineExtensions::ApplyTextureSwap(texSwap, object.get());

			a_out->AttachChild(object, true);

			if (entry)
			{
				a_dbEntries.emplace_back(std::move(entry));
			}

			result = true;
		}

		return result;
	}*/

	void IObjectManager::GetNodeName(
		TESForm*                     a_form,
		const IModel::modelParams_t& a_params,
		char (&a_out)[INode::NODE_NAME_BUFFER_SIZE]) noexcept
	{
		switch (a_params.type)
		{
		case ModelType::kWeapon:
			INode::GetWeaponNodeName(a_form->formID, a_out);
			break;
		case ModelType::kArmor:
		case ModelType::kShield:
			INode::GetArmorNodeName(
				a_form->formID,
				a_params.arma ?
					a_params.arma->formID :
					Game::FormID{},
				a_out);
			break;
		case ModelType::kLight:
			INode::GetLightNodeName(a_form->formID, a_out);
			break;
		case ModelType::kProjectile:
		case ModelType::kMisc:
			INode::GetMiscNodeName(a_form->formID, a_out);
			break;
		case ModelType::kAmmo:
			INode::GetAmmoNodeName(a_form->formID, a_out);
			break;
		default:
			HALT("FIXME");
		}
	}

	bool IObjectManager::LoadAndAttach(
		processParams_t&                a_params,
		const Data::configBaseValues_t& a_activeConfig,
		const Data::configBase_t&       a_baseConfig,
		ObjectEntryBase&                a_objectEntry,
		TESForm*                        a_form,
		TESForm*                        a_modelForm,
		const bool                      a_leftWeapon,
		const bool                      a_visible,
		const bool                      a_disableHavok,
		const bool                      a_bhkAnims,
		const bool                      a_physics) noexcept
	{
		if (a_objectEntry.data.state)
		{
			return false;
		}

		if (!a_activeConfig.targetNode)
		{
			return false;
		}

		if (a_form->formID.IsTemporary())
		{
			return false;
		}

		const auto hasModelForm = static_cast<bool>(a_modelForm);

		if (!hasModelForm)
		{
			a_modelForm = a_form;
		}

		IModel::modelParams_t modelParams;

		if (!IModel::GetModelParams(
				a_params.actor,
				a_modelForm,
				a_params.race,
				a_params.configSex == Data::ConfigSex::Female,
				a_activeConfig.flags.test(Data::BaseFlags::kLoad1pWeaponModel),
				a_activeConfig.flags.test(Data::BaseFlags::kUseWorldModel),
				modelParams))
		{
			Debug(
				"[%.8X] [race: %.8X] [item: %.8X] couldn't get model params",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_modelForm->formID.get());

			return false;
		}

		targetNodes_t targetNodes;

		if (!INode::CreateTargetNode(
				a_activeConfig,
				a_activeConfig.targetNode,
				a_params.npcRoot,
				targetNodes))
		{
			Debug(
				"[%.8X] [race: %.8X] [item: %.8X] couldn't get target node: %s",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_modelForm->formID.get(),
				a_activeConfig.targetNode.name.c_str());

			return false;
		}

		auto state = std::make_unique<ObjectEntryBase::State>();

		NiPointer<NiNode>   object;
		ObjectDatabaseEntry dbentry;

		if (!GetUniqueObject(modelParams.path, dbentry, object))
		{
			Warning(
				"[%.8X] [race: %.8X] [item: %.8X] failed to load model: %s",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_modelForm->formID.get(),
				modelParams.path);

			return false;
		}

		if (dbentry)
		{
			state->dbEntry = std::move(dbentry);
		}

		a_params.ResetEffectShaders();

		if (modelParams.swap)
		{
			EngineExtensions::ApplyTextureSwap(
				modelParams.swap,
				object.get());
		}

		//object->m_localTransform = {};

		INode::UpdateObjectTransform(
			a_activeConfig.geometryTransform,
			object.get());

		state->currentGeomTransformTag = a_activeConfig.geometryTransform;

		char buffer[INode::NODE_NAME_BUFFER_SIZE];

		GetNodeName(a_modelForm, modelParams, buffer);

		auto itemRoot = CreateAttachmentNode(buffer);

		state->UpdateData(a_activeConfig);
		INode::UpdateObjectTransform(
			state->transform,
			itemRoot,
			targetNodes.ref);

		NiNode* objectAttachmentNode;

		if (a_physics)
		{
			objectAttachmentNode = CreateAttachmentNode(
				BSStringHolder::GetSingleton()->m_objectPhy);

			itemRoot->AttachChild(objectAttachmentNode, true);

			state->nodes.physics = objectAttachmentNode;
		}
		else
		{
			objectAttachmentNode = itemRoot;
		}

		targetNodes.rootNode->AttachChild(itemRoot, true);

		UpdateDownwardPass(itemRoot);

		const auto ar = EngineExtensions::AttachObject(
			a_params.actor,
			a_modelForm,
			a_params.root,
			objectAttachmentNode,
			object,
			modelParams.type,
			a_leftWeapon,
			a_activeConfig.flags.test(Data::BaseFlags::kDropOnDeath),
			a_activeConfig.flags.test(Data::BaseFlags::kRemoveScabbard),
			a_activeConfig.flags.test(Data::BaseFlags::kKeepTorchFlame),
			a_disableHavok || a_activeConfig.flags.test(Data::BaseFlags::kDisableHavok),
			a_activeConfig.flags.test(Data::BaseFlags::kRemoveProjectileTracers));
		/*a_activeConfig.flags.test(Data::BaseFlags::kAttachLight),
			state->light);*/

		//UpdateDownwardPass(itemRoot);

		if (a_visible && state->nodes.HasPhysicsNode())
		{
			if (auto& pv = a_activeConfig.physicsValues.data;
			    pv && !pv->valueFlags.test(Data::ConfigNodePhysicsFlags::kDisabled))
			{
				state->simComponent = a_params.objects.CreateAndAddSimComponent(
					state->nodes.physics.get(),
					state->nodes.physics->m_localTransform,
					*pv);
			}
		}

		if (modelParams.type == ModelType::kAmmo)
		{
			TryMakeArrowState(state, object);
		}

		FinalizeObjectState(
			state,
			a_form,
			itemRoot,
			object,
			targetNodes,
			a_activeConfig,
			a_params.actor);

		if (a_activeConfig.flags.test(Data::BaseFlags::kPlaySequence))
		{
			state->UpdateAndPlayAnimation(
				a_params.actor,
				a_activeConfig.niControllerSequence);
		}
		else if (
			a_bhkAnims &&
			modelParams.type == ModelType::kWeapon &&
			!a_activeConfig.flags.test(Data::BaseFlags::kDisableWeaponAnims))
		{
			if (EngineExtensions::CreateWeaponBehaviorGraph(
					object,
					state->weapAnimGraphManagerHolder,
					[&](const char* a_path) noexcept {
						return a_baseConfig.hkxFilter.empty() ?
				                   true :
				                   !a_baseConfig.hkxFilter.contains(a_path);
					}))
			{
				const auto& eventName = a_activeConfig.flags.test(Data::BaseFlags::kAnimationEvent) ?
				                            a_activeConfig.animationEvent :
				                            StringHolder::GetSingleton().weaponSheathe;

				state->UpdateAndSendAnimationEvent(eventName);

				/*a_params.objects.RegisterWeaponAnimationGraphManagerHolder(
					state->weapAnimGraphManagerHolder,
					!a_activeConfig.flags.test(Data::BaseFlags::kDisableAnimEventForwarding));*/
			}
		}

		if (ar.test(AttachResultFlags::kScbLeft))
		{
			state->flags.set(ObjectEntryFlags::kScbLeft);
		}

		if (hasModelForm)
		{
			state->modelForm = a_modelForm->formID;
		}

		a_objectEntry.data.state = std::move(state);

		if (a_visible)
		{
			PlayObjectSound(
				a_params,
				a_activeConfig,
				a_objectEntry,
				true);
		}

		return true;
	}

	bool IObjectManager::LoadAndAttachGroup(
		processParams_t&                a_params,
		const Data::configBaseValues_t& a_activeConfig,
		const Data::configBase_t&       a_baseConfig,
		const Data::configModelGroup_t& a_group,
		ObjectEntryBase&                a_objectEntry,
		TESForm*                        a_form,
		const bool                      a_leftWeapon,
		const bool                      a_visible,
		const bool                      a_disableHavok,
		const bool                      a_bhkAnims,
		const bool                      a_physics) noexcept
	{
		if (a_objectEntry.data.state)
		{
			return false;
		}

		if (!a_activeConfig.targetNode)
		{
			return false;
		}

		if (a_form->formID.IsTemporary())
		{
			return false;
		}

		const stl::fixed_string emptyString;

		auto it = a_group.entries.find(emptyString);
		if (it == a_group.entries.end())
		{
			return false;
		}

		if (it->second.form.get_id() != a_form->formID)
		{
			return false;
		}

		struct tmpdata_t
		{
			const Data::configModelGroup_t::data_type::value_type* entry{ nullptr };
			TESForm*                                               form{ nullptr };
			IModel::modelParams_t                                  params;
			NiPointer<NiNode>                                      object;
			ObjectEntryBase::State::GroupObject*                   grpObject{ nullptr };
			ObjectDatabaseEntry                                    dbEntry;
		};

		stl::forward_list<tmpdata_t> modelParams;

		for (auto& e : a_group.entries)
		{
			if (e.second.flags.test(Data::ConfigModelGroupEntryFlags::kDisabled))
			{
				continue;
			}

			const auto form = e.second.form.get_form();
			if (!form)
			{
				continue;
			}

			IModel::modelParams_t params;

			if (!IModel::GetModelParams(
					a_params.actor,
					form,
					a_params.race,
					a_params.configSex == Data::ConfigSex::Female,
					e.second.flags.test(Data::ConfigModelGroupEntryFlags::kLoad1pWeaponModel),
					a_activeConfig.flags.test(Data::BaseFlags::kUseWorldModel) ||
						e.second.flags.test(Data::ConfigModelGroupEntryFlags::kUseWorldModel),
					params))
			{
				Debug(
					"[%.8X] [race: %.8X] [item: %.8X] couldn't get model params",
					a_params.actor->formID.get(),
					a_params.race->formID.get(),
					form->formID.get());

				continue;
			}

			modelParams.emplace_front(
				std::addressof(e),
				form,
				std::move(params));
		}

		if (modelParams.empty())
		{
			return false;
		}

		targetNodes_t targetNodes;

		if (!INode::CreateTargetNode(
				a_activeConfig,
				a_activeConfig.targetNode,
				a_params.npcRoot,
				targetNodes))
		{
			Debug(
				"[%.8X] [race: %.8X] [item: %.8X] couldn't get target node: %s",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_form->formID.get(),
				a_activeConfig.targetNode.name.c_str());

			return false;
		}

		auto state = std::make_unique<ObjectEntryBase::State>();

		bool loaded = false;

		for (auto& e : modelParams)
		{
			if (!GetUniqueObject(e.params.path, e.dbEntry, e.object))
			{
				Warning(
					"[%.8X] [race: %.8X] [item: %.8X] failed to load model: %s",
					a_params.actor->formID.get(),
					a_params.race->formID.get(),
					e.form->formID.get(),
					e.params.path);

				continue;
			}

			loaded = true;
		}

		if (!loaded)
		{
			return false;
		}

		a_params.ResetEffectShaders();

		char buffer[INode::NODE_NAME_BUFFER_SIZE];

		stl::snprintf(
			buffer,
			StringHolder::FMT_NINODE_IED_GROUP,
			a_form->formID.get());

		auto groupRoot = CreateAttachmentNode(buffer);

		state->UpdateData(a_activeConfig);
		INode::UpdateObjectTransform(
			state->transform,
			groupRoot,
			targetNodes.ref);

		NiNode* objectAttachmentNode;

		if (a_physics)
		{
			objectAttachmentNode = CreateAttachmentNode(BSStringHolder::GetSingleton()->m_objectPhy);

			groupRoot->AttachChild(objectAttachmentNode, true);

			state->nodes.physics = objectAttachmentNode;
		}
		else
		{
			objectAttachmentNode = groupRoot;
		}

		targetNodes.rootNode->AttachChild(groupRoot, true);

		UpdateDownwardPass(groupRoot);

		for (auto& e : modelParams)
		{
			if (!e.object)
			{
				continue;
			}

			e.object->m_localTransform = {};

			if (e.params.swap)
			{
				EngineExtensions::ApplyTextureSwap(
					e.params.swap,
					e.object);
			}

			GetNodeName(e.form, e.params, buffer);

			auto itemRoot = CreateAttachmentNode(buffer);

			auto& n = state->groupObjects.try_emplace(
											 e.entry->first,
											 itemRoot,
											 e.object)
			              .first->second;

			if (e.dbEntry)
			{
				n.dbEntry = std::move(e.dbEntry);
			}

			n.transform.Update(e.entry->second.transform);

			INode::UpdateObjectTransform(
				n.transform,
				n.rootNode,
				nullptr);

			objectAttachmentNode->AttachChild(itemRoot, true);
			UpdateDownwardPass(itemRoot);

			EngineExtensions::AttachObject(
				a_params.actor,
				e.form,
				a_params.root,
				itemRoot,
				e.object,
				e.params.type,
				a_leftWeapon ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kLeftWeapon),
				a_activeConfig.flags.test(Data::BaseFlags::kDropOnDeath) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kDropOnDeath),
				a_activeConfig.flags.test(Data::BaseFlags::kRemoveScabbard) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kRemoveScabbard),
				a_activeConfig.flags.test(Data::BaseFlags::kKeepTorchFlame) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kKeepTorchFlame),
				a_disableHavok ||
					a_activeConfig.flags.test(Data::BaseFlags::kDisableHavok) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kDisableHavok),
				a_activeConfig.flags.test(Data::BaseFlags::kRemoveProjectileTracers) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kRemoveProjectileTracers));
			/*a_activeConfig.flags.test(Data::BaseFlags::kAttachLight) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kAttachLight),
				n.light);*/

			e.grpObject = std::addressof(n);
		}

		//UpdateDownwardPass(groupRoot);

		for (auto& e : modelParams)
		{
			if (!e.grpObject)
			{
				continue;
			}

			if (e.entry->second.flags.test(
					Data::ConfigModelGroupEntryFlags::kPlaySequence))
			{
				e.grpObject->PlayAnimation(
					a_params.actor,
					e.entry->second.niControllerSequence);
			}
			else if (
				a_bhkAnims &&
				e.params.type == ModelType::kWeapon &&
				!a_activeConfig.flags.test(Data::BaseFlags::kDisableWeaponAnims) &&
				!e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kDisableWeaponAnims))
			{
				if (EngineExtensions::CreateWeaponBehaviorGraph(
						e.grpObject->object,
						e.grpObject->weapAnimGraphManagerHolder,
						[&](const char* a_path) noexcept {
							return a_baseConfig.hkxFilter.empty() ?
					                   true :
					                   !a_baseConfig.hkxFilter.contains(a_path);
						}))
				{
					const auto& eventName =
						e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kAnimationEvent) ?
							e.entry->second.animationEvent :
							StringHolder::GetSingleton().weaponSheathe;

					e.grpObject->UpdateAndSendAnimationEvent(eventName);

					/*a_params.objects.RegisterWeaponAnimationGraphManagerHolder(
						e.grpObject->weapAnimGraphManagerHolder,
						!a_activeConfig.flags.test(Data::BaseFlags::kDisableAnimEventForwarding) &&
							!e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kDisableAnimEventForwarding));*/
				}
			}
		}

		if (a_visible && state->nodes.HasPhysicsNode())
		{
			if (auto& pv = a_activeConfig.physicsValues.data;
			    pv && !pv->valueFlags.test(Data::ConfigNodePhysicsFlags::kDisabled))
			{
				state->simComponent = a_params.objects.CreateAndAddSimComponent(
					state->nodes.physics.get(),
					state->nodes.physics->m_localTransform,
					*pv);
			}
		}

		FinalizeObjectState(
			state,
			a_form,
			groupRoot,
			nullptr,
			targetNodes,
			a_activeConfig,
			a_params.actor);

		state->flags.set(ObjectEntryFlags::kIsGroup);

		a_objectEntry.data.state = std::move(state);

		if (a_visible)
		{
			PlayObjectSound(
				a_params,
				a_activeConfig,
				a_objectEntry,
				true);
		}

		return true;
	}

	void IObjectManager::FinalizeObjectState(
		std::unique_ptr<ObjectEntryBase::State>& a_state,
		TESForm*                                 a_form,
		NiNode*                                  a_rootNode,
		const NiPointer<NiNode>&                 a_objectNode,
		targetNodes_t&                           a_targetNodes,
		const Data::configBaseValues_t&          a_config,
		Actor*                                   a_actor) noexcept
	{
		a_state->form           = a_form;
		a_state->formid         = a_form->formID;
		a_state->nodes.rootNode = a_rootNode;
		a_state->nodes.ref      = std::move(a_targetNodes.ref);
		a_state->nodes.object   = a_objectNode;
		a_state->nodeDesc       = a_config.targetNode;
		a_state->created        = IPerfCounter::Query();
		a_state->atmReference   = a_config.targetNode.managed() ||
		                        a_config.flags.test(Data::BaseFlags::kReferenceMode);

		a_state->owner = a_actor->formID;
	}

	void IObjectManager::TryMakeArrowState(
		std::unique_ptr<ObjectEntryBase::State>& a_state,
		NiNode*                                  a_object) noexcept
	{
		const auto sh = BSStringHolder::GetSingleton();

		if (const auto arrowQuiver = FindChildNode(a_object, sh->m_arrowQuiver))
		{
			a_state->arrowState =
				std::make_unique<ObjectEntryBase::QuiverArrowState>(arrowQuiver);
		}
	}

	void IObjectManager::PlayObjectSound(
		const processParams_t&          a_params,
		const Data::configBaseValues_t& a_config,
		const ObjectEntryBase&          a_objectEntry,
		bool                            a_equip) noexcept
	{
		if (a_objectEntry.data.state &&
		    a_params.flags.test(ControllerUpdateFlags::kPlaySound) &&
		    a_config.flags.test(Data::BaseFlags::kPlaySound) &&
		    m_playSound)
		{
			if (a_params.objects.IsPlayer() || m_playSoundNPC)
			{
				SoundPlay(
					a_objectEntry.data.state->form->formType,
					a_objectEntry.data.state->nodes.rootNode,
					a_equip);
			}
		}
	}

	bool IObjectManager::AttachNodeImpl(
		NiNode*                     a_root,
		const Data::NodeDescriptor& a_node,
		bool                        a_atmReference,
		ObjectEntryBase&            a_entry) noexcept
	{
		auto& state = a_entry.data.state;

		if (!state)
		{
			return false;
		}

		const bool result = INode::AttachObjectToTargetNode(
			a_node,
			a_atmReference,
			a_root,
			state->nodes.rootNode,
			state->nodes.ref);

		if (result)
		{
			state->nodeDesc     = a_node;
			state->atmReference = a_atmReference;

			state->flags.clear(ObjectEntryFlags::kRefSyncDisableFailedOrphan);
		}

		return result;
	}

}