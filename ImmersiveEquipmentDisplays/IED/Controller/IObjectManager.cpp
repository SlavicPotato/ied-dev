#include "pch.h"

#include "ActorObjectHolder.h"
#include "INode.h"
#include "INodeOverride.h"
#include "IObjectManager.h"
#include "QueuedModel.h"

#include "IED/AnimationUpdateController.h"
#include "IED/EngineExtensions.h"
#include "IED/ReferenceLightController.h"
#include "IED/StringHolder.h"
#include "IED/Util/Common.h"

namespace IED
{
	using namespace ::Util::Node;

	bool IObjectManager::RemoveObject(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		ObjectEntryBase&                 a_objectEntry,
		ActorObjectHolder&               a_holder,
		stl::flag<ControllerUpdateFlags> a_flags,
		bool                             a_defer,
		bool                             a_removeCloningTask) noexcept
	{
		if (auto& state = a_objectEntry.data.state)
		{
			if (const auto& sc = state->simComponent)
			{
				a_holder.RemoveSimComponent(sc);
			}

			state->UnregisterFromControllers(a_holder.GetActorFormID());

			for (auto& e : state->groupObjects)
			{
				e.second.UnregisterFromControllers(a_holder.GetActorFormID());
			}

			if (
				!a_defer &&
				m_playSound &&
				a_flags.test(ControllerUpdateFlags::kPlayEquipSound) &&
				state->flags.test(ObjectEntryFlags::kPlayEquipSound) &&
				a_actor &&
				a_actor->loadedState &&
				(a_actor == *g_thePlayer || m_playSoundNPC) &&
				state->commonNodes.rootNode->m_parent &&
				state->commonNodes.rootNode->IsVisible())
			{
				SoundPlay(
					state->form->formType,
					a_actor,
					false);
			}
		}

		return a_objectEntry.reset(
			a_handle,
			a_holder.m_root,
			a_holder.m_root1p,
			*this,
			a_defer,
			a_removeCloningTask);
	}

	bool IObjectManager::RemoveActorImpl(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		auto it = m_actorMap.find(a_actor->formID);
		if (it != m_actorMap.end())
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
		Game::FormID                     a_actor,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		auto it = m_actorMap.find(a_actor);
		if (it != m_actorMap.end())
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
			const stl::lock_guard lock(m_lock);

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
		const stl::lock_guard lock(m_lock);

		auto it = m_actorMap.find(a_actor);
		if (it != m_actorMap.end())
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

	void IObjectManager::RequestEvaluateLF(
		Game::FormID a_actor) const noexcept
	{
		auto it = m_actorMap.find(a_actor);
		if (it != m_actorMap.end())
		{
			it->second.m_wantLFUpdate = true;
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

	void IObjectManager::QueueRequestEvaluateLF(
		Game::FormID a_actor) const noexcept
	{
		ITaskPool::AddTask(
			[this,
		     a_actor]() {
				const stl::lock_guard lock(m_lock);

				RequestEvaluateLF(a_actor);
			});
	}

	void IObjectManager::QueueRequestEvaluateAll() const noexcept
	{
		ITaskPool::AddTask(
			[this] {
				const stl::lock_guard lock(m_lock);

				RequestEvaluateAll();
			});
	}

	void IObjectManager::QueueRequestEvaluateLFAll() const noexcept
	{
		ITaskPool::AddTask(
			[this] {
				const stl::lock_guard lock(m_lock);

				RequestLFEvaluateAll();
			});
	}

	void IObjectManager::QueueRequestEvaluate(
		TESObjectREFR* a_actor,
		bool           a_defer,
		bool           a_xfrmUpdate,
		bool           a_xfrmUpdateNoDefer) const noexcept
	{
		if (Util::Common::IsREFRValid(a_actor))
		{
			if (auto actor = a_actor->As<Actor>())
			{
				QueueRequestEvaluate(
					actor->formID,
					a_defer,
					a_xfrmUpdate,
					a_xfrmUpdateNoDefer);
			}
		}
	}

	void IObjectManager::QueueClearVariablesOnAll(bool a_requestEval) noexcept
	{
		ITaskPool::AddPriorityTask([this, a_requestEval] {
			const stl::lock_guard lock(m_lock);

			ClearVariablesOnAll(a_requestEval);
		});
	}

	void IObjectManager::QueueClearVariables(
		Game::FormID a_handle,
		bool         a_requestEval) noexcept
	{
		ITaskPool::AddPriorityTask([this, a_handle, a_requestEval] {
			const stl::lock_guard lock(m_lock);

			ClearVariables(a_handle, a_requestEval);
		});
	}

	void IObjectManager::QueueRequestVariableUpdateOnAll() const noexcept
	{
		ITaskPool::AddPriorityTask([this] {
			const stl::lock_guard lock(m_lock);

			RequestVariableUpdateOnAll();
		});
	}

	void IObjectManager::QueueRequestVariableUpdate(Game::FormID a_handle) const noexcept
	{
		ITaskPool::AddPriorityTask([this, a_handle] {
			const stl::lock_guard lock(m_lock);

			RequestVariableUpdate(a_handle);
		});
	}

	void IObjectManager::RemoveActorGear(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags) noexcept
	{
		auto it = m_actorMap.find(a_actor->formID);
		if (it != m_actorMap.end())
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
			a_holder.visit([&](auto& a_object) noexcept {
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
		m_actorMap.clear();
	}

	void IObjectManager::GetNodeName(
		TESForm*                   a_form,
		const IModel::ModelParams& a_params,
		char (&a_out)[INode::NODE_NAME_BUFFER_SIZE]) noexcept
	{
		switch (a_params.type)
		{
		case ModelType::kWeapon:
			INode::GetWeaponNodeName(a_form->formID, a_out);
			break;
		case ModelType::kArmor:
			INode::GetArmorNodeName(
				a_form->formID,
				a_params.arma ?
					a_params.arma->formID :
					Game::FormID{},
				a_out);
			break;
		case ModelType::kShield:
			INode::GetShieldNodeName(
				a_form->formID,
				a_params.arma ?
					a_params.arma->formID :
					Game::FormID{},
				a_out);
			break;
		case ModelType::kLight:
			INode::GetLightNodeName(a_form->formID, a_out);
			break;
		case ModelType::kAmmo:
			INode::GetAmmoNodeName(a_form->formID, a_out);
			break;
		default:
			INode::GetMiscNodeName(a_form->formID, a_out);
			break;
		}
	}

	bool IObjectManager::ShouldBackgroundClone(
		const ProcessParams&       a_params,
		const IModel::ModelParams& a_modelParams) noexcept
	{
		switch (GetBackgroundCloneLevel(a_params.is_player()))
		{
		case BackgroundCloneLevel::kTexSwap:

			return a_modelParams.texSwap &&
			       a_modelParams.texSwap->numAlternateTextures > 0;

		case BackgroundCloneLevel::kClone:

			return true;

		default:

			return false;
		}
	}

	AttachObjectResult IObjectManager::LoadAndAttach(
		ProcessParams&                  a_params,
		const Data::configBaseValues_t& a_activeConfig,
		const Data::configBase_t&       a_baseConfig,
		ObjectEntryBase&                a_objectEntry,
		TESForm*                        a_form,
		TESForm*                        a_modelForm,
		const bool                      a_leftWeapon,
		const bool                      a_visible,
		const bool                      a_disableHavok,
		const bool                      a_physics) noexcept
	{
		if (a_objectEntry.data.state)
		{
			return AttachObjectResult::kFailed;
		}

		if (!a_activeConfig.targetNode)
		{
			return AttachObjectResult::kFailed;
		}

		if (a_form->formID.IsTemporary())
		{
			return AttachObjectResult::kFailed;
		}

		const auto hasModelForm = static_cast<bool>(a_modelForm);

		if (hasModelForm)
		{
			if (a_modelForm->formID.IsTemporary())
			{
				return AttachObjectResult::kFailed;
			}
		}
		else
		{
			a_modelForm = a_form;
		}

		IModel::ModelParams modelParams;

		if (!IModel::GetModelParams(
				a_params.actor,
				a_modelForm,
				a_params.race,
				a_params.objects.IsFemale(),
				a_activeConfig.flags.test(Data::BaseFlags::kLoad1pWeaponModel),
				a_activeConfig.flags.test(Data::BaseFlags::kUseWorldModel),
				modelParams))
		{
			Debug(
				"[%.8X] [race: %.8X] [item: %.8X] couldn't get model params",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_modelForm->formID.get());

			return AttachObjectResult::kFailed;
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

			return AttachObjectResult::kFailed;
		}

		ObjectDatabaseEntry dbentry;

		const bool backgroundClone = ShouldBackgroundClone(
			a_params,
			modelParams);

		const auto odbResult = GetModel(
			modelParams.path,
			dbentry);

		switch (odbResult)
		{
		case ObjectLoadResult::kFailed:

			Error(
				"[%.8X] [race: %.8X] [item: %.8X] failed to load model: %s",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_modelForm->formID.get(),
				modelParams.path);

			return AttachObjectResult::kFailed;

		case ObjectLoadResult::kPending:

			a_params.objects.AddQueuedModel(dbentry);

			if (!backgroundClone)
			{
				return AttachObjectResult::kFailed;
			}

			break;
		}

		NiPointer<NiNode> object;

		const auto colScale =
			a_activeConfig.flags.test(Data::BaseFlags::kGeometryScaleCollider) &&
					a_activeConfig.geometryTransform.scale ?
				*a_activeConfig.geometryTransform.scale :
				1.0f;

		if (auto& ct = a_objectEntry.data.cloningTask)
		{
			if (ct->GetDBEntry() != dbentry ||
			    ct->GetSwap() != modelParams.texSwap ||
			    ct->GetColliderScale() != colScale ||
			    ct->get_task_state() == ObjectCloningTask::State::kCancelled)
			{
				ct->try_cancel_task();
				ct.reset();

				if (backgroundClone)
				{
					ct = DispatchCloningTask(
						a_params,
						dbentry,
						modelParams.texSwap,
						colScale);

					return AttachObjectResult::kPending;
				}
				else
				{
					ObjectCloningTask::CloneAndApplyTexSwap(
						dbentry,
						modelParams.texSwap,
						colScale,
						object);
				}
			}
			else
			{
				switch (ct->get_task_state())
				{
				case ObjectCloningTask::State::kCompleted:

					object = ct->GetClone();

					if (!object)
					{
						Error(
							"[%.8X] [race: %.8X] [item: %.8X] background clone task failed",
							a_params.actor->formID.get(),
							a_params.race->formID.get(),
							a_modelForm->formID.get(),
							modelParams.path);

						return AttachObjectResult::kFailed;
					}

					break;

				case ObjectCloningTask::State::kPending:
				case ObjectCloningTask::State::kProcessing:

					return AttachObjectResult::kPending;

				default:

					return AttachObjectResult::kFailed;
				}
			}
		}
		else
		{
			if (backgroundClone)
			{
				ct = DispatchCloningTask(
					a_params,
					dbentry,
					modelParams.texSwap,
					colScale);

				return AttachObjectResult::kPending;
			}
			else
			{
				ObjectCloningTask::CloneAndApplyTexSwap(
					dbentry,
					modelParams.texSwap,
					colScale,
					object);
			}
		}

		auto state = std::make_unique<ObjectEntryBase::State>(std::move(dbentry));

		if (a_activeConfig.flags.test(Data::BaseFlags::kGeometryScaleCollider))
		{
			state->colliderScale = a_activeConfig.geometryTransform.scale;
			state->flags.set(ObjectEntryFlags::kHasCollisionObjectScale);
		}

		a_params.SuspendReferenceEffectShaders();

		INode::UpdateObjectTransform(a_activeConfig.geometryTransform, object.get());
		object->SetVisible(true);

		//NiAVObject_unk39_col(object.get(), 4, true, true, 1ui8);

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

			state->physicsNode = objectAttachmentNode;
		}
		else
		{
			objectAttachmentNode = itemRoot;
		}

		targetNodes.rootNode->AttachChild(itemRoot, true);

		UpdateDownwardPass(itemRoot);

		TESObjectLIGH* lightForm = nullptr;

		if (a_activeConfig.flags.test(Data::BaseFlags::kAttachLight))
		{
			lightForm = GetLightFormForAttach(a_modelForm);

			TryCreatePointLight(
				a_params.actor,
				object,
				lightForm,
				a_activeConfig.extraLightConfig.data,
				state->light);
		}

		const auto ar = AttachObject(
			a_params.actor,
			a_modelForm,
			a_params.root,
			objectAttachmentNode,
			object,
			modelParams.type,
			a_leftWeapon,
			a_activeConfig.flags.test(Data::BaseFlags::kDropOnDeath),
			a_activeConfig.flags.test(Data::BaseFlags::kRemoveScabbard),
			state->light || a_activeConfig.flags.test(Data::BaseFlags::kKeepTorchFlame),
			a_disableHavok || a_activeConfig.flags.test(Data::BaseFlags::kDisableHavok),
			a_activeConfig.flags.test(Data::BaseFlags::kRemoveProjectileTracers),
			a_activeConfig.flags.test(Data::BaseFlags::kRemoveEditorMarker));

		if (state->light)
		{
			//state->currentExtraLightTag.emplace(a_activeConfig.extraLightConfig);

			ReferenceLightController::GetSingleton().AddLight(
				a_params.actor->formID,
				lightForm,
				*state->light);
		}

		state->sound.form = GetSoundDescriptor(a_modelForm);

		if (a_activeConfig.flags.test(Data::BaseFlags::kPlayLoopSound))
		{
			TryInitializeAndPlayLoopSound(
				a_params.actor,
				state->sound);
		}

		//UpdateDownwardPass(itemRoot);

		if (a_visible && state->HasPhysicsNode())
		{
			if (auto& pv = a_activeConfig.physicsValues.data;
			    pv && !pv->valueFlags.test(Data::ConfigNodePhysicsFlags::kDisabled))
			{
				state->simComponent = a_params.objects.CreateAndAddSimComponent(
					state->physicsNode.get(),
					state->physicsNode->m_localTransform,
					*pv);
			}
		}

		if (modelParams.type == ModelType::kAmmo)
		{
			TryMakeArrowState(state, object);
		}

		if (const bool wantHidden = a_activeConfig.flags.test(Data::BaseFlags::kHideGeometry);
		    wantHidden != object->IsHidden())
		{
			object->SetHidden(wantHidden);
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
			state->UpdateAndPlayAnimationSequence(
				a_params.actor,
				a_activeConfig.niControllerSequence);
		}
		else if (
			AnimationUpdateController::GetSingleton().IsInitialized() &&
			!a_activeConfig.flags.test(Data::BaseFlags::kDisableBehaviorGraphAnims))
		{
			bool result;

			const auto filterFunc = [&](const char* a_path) noexcept {
				return a_baseConfig.hkxFilter.empty() ?
				           true :
				           !a_baseConfig.hkxFilter.contains(a_path);
			};

			const auto& ct = a_objectEntry.data.cloningTask;

			if (ct && ct->HasGraphHolder())
			{
				result = AnimationUpdateController::CreateWeaponBehaviorGraph(
					object,
					*ct,
					state->anim.holder,
					filterFunc);
			}
			else
			{
				result = AnimationUpdateController::CreateWeaponBehaviorGraph(
					object,
					state->anim.holder,
					filterFunc);
			}

			if (result)
			{
				/*RE::BSAnimationGraphManagerPtr agm;
				if (state->anim.holder->GetAnimationGraphManagerImpl(agm))
				{
					for (auto& e : agm->graphs)
					{
						if (e)
						{
							e->AddEventSink(std::addressof(state->anim));
							break;
						}
					}
				}*/

				if (a_activeConfig.flags.test(Data::BaseFlags::kAttachSubGraphs))
				{
					state->anim.holder->AttachSubGraphs(*a_params.actor);
					state->anim.subGraphsAttached = true;
				}

				if (a_activeConfig.flags.test(Data::BaseFlags::kAnimationEvent))
				{
					state->anim.UpdateAndSendAnimationEvent(a_activeConfig.animationEvent);
				}

				AnimationUpdateController::GetSingleton().AddObject(
					a_params.objects.GetActorFormID(),
					state->anim.holder);
			}
		}

		if (ar.test(AttachResultFlags::kScbLeft))
		{
			state->flags.set(ObjectEntryFlags::kScbLeft);
		}

		if (hasModelForm)
		{
			state->modelForm = a_modelForm;
		}

		a_objectEntry.data.state = std::move(state);
		a_objectEntry.data.cloningTask.reset();

		if (a_visible)
		{
			PlayEquipObjectSound(
				a_params,
				a_activeConfig,
				a_objectEntry,
				true);
		}

		return AttachObjectResult::kSucceeded;
	}

	AttachObjectResult IObjectManager::LoadAndAttachGroup(
		ProcessParams&                  a_params,
		const Data::configBaseValues_t& a_activeConfig,
		const Data::configBase_t&       a_baseConfig,
		const Data::configModelGroup_t& a_group,
		ObjectEntryBase&                a_objectEntry,
		TESForm*                        a_form,
		const bool                      a_leftWeapon,
		const bool                      a_visible,
		const bool                      a_disableHavok,
		const bool                      a_physics) noexcept
	{
		if (a_objectEntry.data.state)
		{
			return AttachObjectResult::kFailed;
		}

		if (!a_activeConfig.targetNode)
		{
			return AttachObjectResult::kFailed;
		}

		if (a_form->formID.IsTemporary())
		{
			return AttachObjectResult::kFailed;
		}

		auto it = a_group.entries.find(stl::fixed_string());
		if (it == a_group.entries.end())
		{
			return AttachObjectResult::kFailed;
		}

		if (it->second.form.get_id() != a_form->formID)
		{
			return AttachObjectResult::kFailed;
		}

		struct tmpdata_t
		{
			const Data::configModelGroup_t::data_type::value_type* entry{ nullptr };
			TESForm*                                               form{ nullptr };
			IModel::ModelParams                                    params;
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

			IModel::ModelParams params;

			if (!IModel::GetModelParams(
					a_params.actor,
					form,
					a_params.race,
					a_params.objects.IsFemale(),
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
			return AttachObjectResult::kFailed;
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

			return AttachObjectResult::kFailed;
		}

		enum class ModelLoadStatus : std::uint8_t
		{
			kNone = 0,

			kHasLoaded  = 1ui8 << 0,
			kHasPending = 1ui8 << 1,
		};

		stl::flag<ModelLoadStatus> status{
			ModelLoadStatus::kNone
		};

		for (auto& e : modelParams)
		{
			ObjectDatabaseEntry dbEntry;

			const auto odbResult = GetModel(
				e.params.path,
				dbEntry);

			switch (odbResult)
			{
			case ObjectLoadResult::kSuccess:

				e.dbEntry = std::move(dbEntry);

				status.set(ModelLoadStatus::kHasLoaded);

				break;

			case ObjectLoadResult::kFailed:

				Warning(
					"[%.8X] [race: %.8X] [item: %.8X] failed to load model: %s",
					a_params.actor->formID.get(),
					a_params.race->formID.get(),
					e.form->formID.get(),
					e.params.path);

				break;

			case ObjectLoadResult::kPending:

				a_params.objects.AddQueuedModel(std::move(dbEntry));

				status.set(ModelLoadStatus::kHasPending);

				break;
			}
		}

		if (!status.test(ModelLoadStatus::kHasLoaded) ||
		    status.test(ModelLoadStatus::kHasPending))
		{
			return AttachObjectResult::kFailed;
		}

		for (auto& e : modelParams)
		{
			if (e.dbEntry)
			{
				e.object = CreateClone(e.dbEntry->object.get(), 1.0f);
			}
		}

		auto state = std::make_unique_for_overwrite<ObjectEntryBase::State>();

		a_params.SuspendReferenceEffectShaders();

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

			state->physicsNode = objectAttachmentNode;
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
			e.object->SetVisible(true);

			//NiAVObject_unk39_col(e.object.get(), 4, true, true, 1ui8);

			if (e.params.texSwap)
			{
				ApplyTextureSwap(e.params.texSwap, e.object);
			}

			GetNodeName(e.form, e.params, buffer);

			auto itemRoot = CreateAttachmentNode(buffer);

			auto& n = state->groupObjects.try_emplace(
											 e.entry->first,
											 e.form,
											 itemRoot,
											 e.object.get(),
											 std::move(e.dbEntry))
			              .first->second;

			n.transform.Update(e.entry->second.transform);

			INode::UpdateObjectTransform(
				n.transform,
				n.commonNodes.rootNode,
				nullptr);

			objectAttachmentNode->AttachChild(itemRoot, true);
			UpdateDownwardPass(itemRoot);

			TESObjectLIGH* lightForm = nullptr;

			if (a_activeConfig.flags.test(Data::BaseFlags::kAttachLight) ||
			    e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kAttachLight))
			{
				lightForm = GetLightFormForAttach(e.form);

				TryCreatePointLight(
					a_params.actor,
					e.object,
					lightForm,
					e.entry->second.extraLightConfig.data,
					n.light);
			}

			AttachObject(
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
				n.light ||
					a_activeConfig.flags.test(Data::BaseFlags::kKeepTorchFlame) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kKeepTorchFlame),
				a_disableHavok ||
					a_activeConfig.flags.test(Data::BaseFlags::kDisableHavok) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kDisableHavok),
				a_activeConfig.flags.test(Data::BaseFlags::kRemoveProjectileTracers) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kRemoveProjectileTracers),
				a_activeConfig.flags.test(Data::BaseFlags::kRemoveEditorMarker) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kRemoveEditorMarker));

			if (n.light)
			{
				ReferenceLightController::GetSingleton().AddLight(
					a_params.actor->formID,
					lightForm,
					*n.light);
			}

			n.sound.form = GetSoundDescriptor(e.form);

			if (a_activeConfig.flags.test(Data::BaseFlags::kPlayLoopSound))
			{
				TryInitializeAndPlayLoopSound(
					a_params.actor,
					n.sound);
			}

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
				AnimationUpdateController::GetSingleton().IsInitialized() &&
				!a_activeConfig.flags.test(Data::BaseFlags::kDisableBehaviorGraphAnims) &&
				!e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kDisableBehaviorGraphAnims))
			{
				if (AnimationUpdateController::CreateWeaponBehaviorGraph(
						e.grpObject->commonNodes.object,
						e.grpObject->anim.holder,
						[&](const char* a_path) noexcept {
							return a_baseConfig.hkxFilter.empty() ?
					                   true :
					                   !a_baseConfig.hkxFilter.contains(a_path);
						}))
				{
					if (a_activeConfig.flags.test(Data::BaseFlags::kAttachSubGraphs) ||
					    e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kAttachSubGraphs))
					{
						e.grpObject->anim.holder->AttachSubGraphs(*a_params.actor);
						e.grpObject->anim.subGraphsAttached = true;
					}

					if (e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kAnimationEvent))
					{
						e.grpObject->anim.UpdateAndSendAnimationEvent(e.entry->second.animationEvent);
					}

					AnimationUpdateController::GetSingleton().AddObject(
						a_params.objects.GetActorFormID(),
						e.grpObject->anim.holder);
				}
			}
		}

		if (a_visible && state->HasPhysicsNode())
		{
			if (auto& pv = a_activeConfig.physicsValues.data;
			    pv && !pv->valueFlags.test(Data::ConfigNodePhysicsFlags::kDisabled))
			{
				state->simComponent = a_params.objects.CreateAndAddSimComponent(
					state->physicsNode.get(),
					state->physicsNode->m_localTransform,
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
		a_objectEntry.data.cloningTask.reset();

		if (a_visible)
		{
			PlayEquipObjectSound(
				a_params,
				a_activeConfig,
				a_objectEntry,
				true);
		}

		return AttachObjectResult::kSucceeded;
	}

	void IObjectManager::FinalizeObjectState(
		const std::unique_ptr<ObjectEntryBase::State>& a_state,
		TESForm*                                       a_form,
		NiNode*                                        a_rootNode,
		const NiPointer<NiNode>&                       a_objectNode,
		targetNodes_t&                                 a_targetNodes,
		const Data::configBaseValues_t&                a_config,
		Actor*                                         a_actor) noexcept
	{
		a_state->form                 = a_form;
		a_state->commonNodes.rootNode = a_rootNode;
		a_state->commonNodes.object   = a_objectNode;
		a_state->refNode              = std::move(a_targetNodes.ref);
		a_state->nodeDesc             = a_config.targetNode;
		a_state->atmReference         = a_config.targetNode.managed() || a_config.flags.test(Data::BaseFlags::kReferenceMode);
		a_state->owner                = a_actor->formID;
	}

	void IObjectManager::TryMakeArrowState(
		const std::unique_ptr<ObjectEntryBase::State>& a_state,
		NiNode*                                        a_object) noexcept
	{
		const auto sh = BSStringHolder::GetSingleton();

		if (const auto arrowQuiver = GetNodeByName(a_object, sh->m_arrowQuiver))
		{
			a_state->arrowState =
				std::make_unique<ObjectEntryBase::QuiverArrowState>(arrowQuiver);
		}
	}

	void IObjectManager::TryCreatePointLight(
		Actor*                        a_actor,
		NiNode*                       a_object,
		TESObjectLIGH*                a_lightForm,
		const Data::ExtraLightData&   a_config,
		std::unique_ptr<ObjectLight>& a_out) noexcept
	{
		if (a_lightForm)
		{
			a_out = ReferenceLightController::CreateAndAttachPointLight(
				a_lightForm,
				a_actor,
				a_object,
				a_config);
		}
	}

	TESObjectLIGH* IObjectManager::GetLightFormForAttach(TESForm* a_modelForm) noexcept
	{
		if (!ReferenceLightController::GetSingleton().GetEnabled())
		{
			return nullptr;
		}

		switch (a_modelForm->formType)
		{
		case TESObjectLIGH::kTypeID:

			return static_cast<TESObjectLIGH*>(a_modelForm);

		case BGSHazard::kTypeID:

			return static_cast<BGSHazard*>(a_modelForm)->data.light;

		default:

			return nullptr;
		}
	}

	void IObjectManager::TryInitializeAndPlayLoopSound(
		Actor*       a_actor,
		ObjectSound& a_sound) noexcept
	{
		if (!a_sound.form)
		{
			return;
		}

		auto& handle = a_sound.handle;

		if (handle.IsValid())
		{
			handle.StopAndReleaseNow();
		}

		const auto audioManager = BSAudioManager::GetSingleton();
		if (!audioManager)
		{
			return;
		}

		if (audioManager->BuildSoundDataFromDescriptor(
				handle,
				a_sound.form))
		{
			handle.SetPosition(a_actor->pos.x, a_actor->pos.y, a_actor->pos.z);
			if (auto followObject = a_actor->Get3D1(false))
			{
				handle.SetObjectToFollow(followObject);
			}
			handle.Play();
		}
	}

	BGSSoundDescriptorForm* IObjectManager::GetSoundDescriptor(
		const TESForm* a_modelForm) noexcept
	{
		switch (a_modelForm->formType)
		{
		case TESObjectLIGH::kTypeID:

			return static_cast<const TESObjectLIGH*>(a_modelForm)->sound;

		case BGSHazard::kTypeID:
			{
				const auto hazard = static_cast<const BGSHazard*>(a_modelForm);

				if (const auto soundForm = hazard->data.sound)
				{
					return soundForm;
				}
				else if (const auto light = hazard->data.light)
				{
					return light->sound;
				}
			}
			break;

		case TESObjectACTI::kTypeID:

			return static_cast<const TESObjectACTI*>(a_modelForm)->soundLoop;

		case BGSProjectile::kTypeID:

			return static_cast<const BGSProjectile*>(a_modelForm)->data.sound;

		case TESObjectDOOR::kTypeID:

			return static_cast<const TESObjectDOOR*>(a_modelForm)->loopSound;

		case BGSMovableStatic::kTypeID:

			return static_cast<const BGSMovableStatic*>(a_modelForm)->soundLoop;

		case TESObjectWEAP::kTypeID:

			return static_cast<const TESObjectWEAP*>(a_modelForm)->idleSound;
		}

		return nullptr;
	}

	void IObjectManager::PlayEquipObjectSound(
		const ProcessParams&            a_params,
		const Data::configBaseValues_t& a_config,
		const ObjectEntryBase&          a_objectEntry,
		bool                            a_equip) noexcept
	{
		if (a_objectEntry.data.state &&
		    a_params.flags.test(ControllerUpdateFlags::kPlayEquipSound) &&
		    a_config.flags.test(Data::BaseFlags::kPlayEquipSound) &&
		    m_playSound)
		{
			if (a_params.objects.IsPlayer() || m_playSoundNPC)
			{
				SoundPlay(
					a_objectEntry.data.state->form->formType,
					a_params.actor,
					a_equip);
			}
		}
	}

	bool IObjectManager::AttachNodeImpl(
		NiNode*                     a_root,
		const Data::NodeDescriptor& a_node,
		bool                        a_atmReference,
		const ObjectEntryBase&      a_entry) noexcept
	{
		auto& state = a_entry.data.state;

		if (!state)
		{
			return false;
		}

		if (a_node.managed())
		{
			a_atmReference = true;
		}

		const bool result = INode::AttachObjectToTargetNode(
			a_node,
			a_atmReference,
			a_root,
			state->commonNodes.rootNode,
			state->refNode);

		if (result)
		{
			state->nodeDesc     = a_node;
			state->atmReference = a_atmReference;

			state->flags.clear(ObjectEntryFlags::kRefSyncDisableFailedOrphan);
		}

		return result;
	}

	NiPointer<ObjectCloningTask> IObjectManager::DispatchCloningTask(
		const ProcessParams&       a_params,
		const ObjectDatabaseEntry& a_entry,
		TESModelTextureSwap*       a_textureSwap,
		float                      a_colliderScale) noexcept
	{
		const auto thrd = RE::BackgroundProcessThread::GetSingleton();

		ASSERT(thrd != nullptr);

		return thrd->QueueTask<ObjectCloningTask>(
			*this,
			a_params.objects.GetActorFormID(),
			a_entry,
			a_textureSwap,
			a_colliderScale);
	}

	bool IObjectManager::RemoveAllChildren(
		NiNode*              a_object,
		const BSFixedString& a_name) noexcept
	{
		bool result = false;

		std::uint32_t maxiter = 1000;

		while (NiPointer object = GetObjectByName(a_object, a_name, true))
		{
			const auto parent = object->m_parent;

			if (!parent)
			{
				break;
			}

			parent->DetachChild2(object);
			result = true;

			ShrinkToSize(a_object);

			if (!--maxiter)
			{
				break;
			}
		}

		return result;
	}

	bool IObjectManager::RemoveObjectByName(
		NiNode*              a_object,
		const BSFixedString& a_name) noexcept
	{
		if (NiPointer object = GetObjectByName(a_object, a_name, true))
		{
			if (auto parent = object->m_parent)
			{
				parent->DetachChild2(object);

				ShrinkToSize(a_object);

				return true;
			}
		}

		return false;
	}

	BSXFlags* IObjectManager::GetBSXFlags(NiObjectNET* a_object) noexcept
	{
		return a_object->GetExtraDataSafe<BSXFlags>(BSStringHolder::GetSingleton()->m_bsx);
	}

	auto IObjectManager::AttachObject(
		Actor*      a_actor,
		TESForm*    a_modelForm,
		BSFadeNode* a_root,
		NiNode*     a_targetNode,
		NiNode*     a_object,
		ModelType   a_modelType,
		bool        a_leftWeapon,
		bool        a_dropOnDeath,
		bool        a_removeScabbards,
		bool        a_keepTorchFlame,
		bool        a_disableHavok,
		bool        a_removeTracers,
		bool        a_removeEditorMarker) noexcept
		-> stl::flag<AttachResultFlags>
	{
		stl::flag<AttachResultFlags> result{
			AttachResultFlags::kNone
		};

		if (const auto* const bsxFlags = GetBSXFlags(a_object))
		{
			const stl::flag<BSXFlags::Flag> flags(bsxFlags->m_data);

			if (flags.test(BSXFlags::Flag::kAddon))
			{
				AttachAddonNodes(a_object);
			}

			if (!flags.test(BSXFlags::Flag::kEditorMarker))
			{
				a_removeEditorMarker = false;
			}
		}
		else
		{
			a_removeEditorMarker = false;
		}

		AttachAddonParticles(a_object);

		if (auto fadeNode = a_object->AsFadeNode())
		{
			fadeNode->unk153 = (fadeNode->unk153 & 0xF0) | 0x7;
		}

		SetShaderPropsFadeNode(a_object, a_root);

		a_targetNode->AttachChild(a_object, true);

		NiAVObject::ControllerUpdateContext ctx{
			static_cast<float>(*Game::g_gameRuntimeMS),
			0
		};

		a_object->UpdateDownwardPass(ctx, 0);

		a_object->IncRef();

		const auto ssn = RE::ShadowSceneNode::GetSingleton();

		fUnk12ba3e0(ssn, a_object);
		fUnk12b99f0(ssn, a_object);

		a_object->DecRef();

		const auto sh = BSStringHolder::GetSingleton();

		a_object->m_name = sh->m_object;

		switch (a_modelType)
		{
		case ModelType::kWeapon:
			{
				const NiPointer scbNode     = GetObjectByName(a_object, sh->m_scb, true);
				const NiPointer scbLeftNode = GetObjectByName(a_object, sh->m_scbLeft, true);

				if (a_removeScabbards)
				{
					if (scbNode || scbLeftNode)
					{
						if (scbNode)
						{
							scbNode->m_parent->DetachChild2(scbNode);
						}

						if (scbLeftNode)
						{
							scbLeftNode->m_parent->DetachChild2(scbLeftNode);
						}

						ShrinkToSize(a_object);
					}
				}
				else
				{
					NiAVObject* scbAttach;
					NiAVObject* scbRemove;

					if (a_leftWeapon && scbLeftNode)
					{
						scbAttach = scbLeftNode;
						scbRemove = scbNode;

						scbLeftNode->ClearHidden();

						result.set(AttachResultFlags::kScbLeft);
					}
					else
					{
						scbAttach = scbNode;
						scbRemove = scbLeftNode;
					}

					if (scbAttach || scbRemove)
					{
						if (scbAttach)
						{
							a_targetNode->AttachChild(scbAttach, true);

							fUnkDC6140(a_targetNode, true);
						}

						if (scbRemove)
						{
							scbRemove->m_parent->DetachChild2(scbRemove);
						}

						ShrinkToSize(a_object);
					}
				}
			}

			break;

		case ModelType::kLight:

			if (a_modelForm->IsTorch())
			{
				if (!a_keepTorchFlame)
				{
					if (RemoveObjectByName(a_object, sh->m_torchFire))
					{
						result.set(AttachResultFlags::kTorchFlameRemoved);
					}

#if !defined(IED_DISABLE_ENB_LIGHT_STRIPPING)

					for (auto& e : sh->m_enbLightAttachNodes)
					{
						if (RemoveAllChildren(a_object, e))
						{
							result.set(AttachResultFlags::kTorchCustomRemoved);
						}
					}
#endif
				}
			}

			break;

		case ModelType::kProjectile:

			if (a_removeTracers)
			{
				RemoveObjectByName(a_object, sh->m_tracerRoot);
			}

			break;
		}

		if (a_removeEditorMarker)
		{
			RemoveObjectByName(a_object, sh->m_editorMarker);
		}

		if (a_disableHavok)  // maybe just force this for ammo
		{
			StripCollision(a_object, true, true);
		}

		fUnk1CD130(a_object, std::uint32_t(0x0));

		QueueAttachHavok(
			BSTaskPool::GetSingleton(),
			a_object,
			(a_disableHavok || a_dropOnDeath) ? 4 : 0,
			true);

		if (const auto cell = a_actor->GetParentCell())
		{
			if (const auto world = cell->GetHavokWorld())
			{
				NiPointer<Actor> mountedActor;

				const bool isMounted = a_actor->GetMountedActor(mountedActor);

				unks_01 tmp;

				auto& r = fUnk5EBD90(isMounted ? mountedActor.get() : a_actor, tmp);
				fUnk5C39F0(BSTaskPool::GetSingleton(), a_object, world, r.p2);
			}
		}

		SetShaderPropsFadeNode(a_targetNode, a_root);

		a_actor->UpdateAlpha();

		return result;
	}

	void IObjectManager::OnAsyncModelClone(
		const NiPointer<ObjectCloningTask>& a_task)
	{
	}

	void IObjectManager::OnAsyncModelLoad(
		const NiPointer<QueuedModel>& a_task)
	{
		auto& entry = a_task->GetEntry();

		if (entry->loadState.load() > ODBEntryLoadState::kLoading)
		{
			const stl::lock_guard lock(m_lock);

			for (const auto& e : GetActorMap().getvec())
			{
				if (e->second.EraseQueuedModel(entry))
				{
					e->second.RequestEval();
				}
			}
		}
	}

}