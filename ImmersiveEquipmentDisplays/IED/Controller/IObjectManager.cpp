#include "pch.h"

#include "Controller.h"
#include "IED/EngineExtensions.h"
#include "IObjectManager.h"

#include <ext/Model.h>
#include <ext/Node.h>

namespace IED
{
	bool IObjectManager::RemoveObject(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		objectEntryBase_t&               a_objectEntry,
		const ActorObjectHolder&         a_data,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		if (!a_objectEntry.state)
		{
			return false;
		}

		if (
			m_playSound &&
			a_flags.test(ControllerUpdateFlags::kPlaySound) &&
			a_objectEntry.state->flags.test(ObjectEntryFlags::kPlaySound) &&
			a_actor &&
			a_actor->loadedState &&
			(a_actor == *g_thePlayer || m_playSoundNPC) &&
			a_objectEntry.state->nodes.rootNode->m_parent &&
			a_objectEntry.state->nodes.rootNode->IsVisible())
		{
			SoundPlay(
				a_objectEntry.state->form->formType,
				a_objectEntry.state->nodes.rootNode->m_parent,
				false);
		}

		if (!a_objectEntry.state->dbEntries.empty())
		{
			QueueDatabaseCleanup();
		}

		a_objectEntry.reset(a_handle, a_data.m_root);

		return true;
	}

	bool IObjectManager::RemoveActorImpl(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto it = m_objects.find(a_actor->formID);
		if (it == m_objects.end())
		{
			return false;
		}

		CleanupActorObjectsImpl(
			a_actor,
			a_handle,
			it->second,
			a_flags);

		m_objects.erase(it);

		return true;
	}

	bool IObjectManager::RemoveActorImpl(
		TESObjectREFR*                   a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto it = m_objects.find(a_actor->formID);
		if (it == m_objects.end())
		{
			return false;
		}

		auto handle = it->second.GetHandle();

		NiPointer<TESObjectREFR> ref;
		(void)handle.LookupZH(ref);

		CleanupActorObjectsImpl(
			a_actor,
			handle,
			it->second,
			a_flags);

		m_objects.erase(it);

		return true;
	}

	bool IObjectManager::RemoveActorImpl(
		Game::FormID                     a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto it = m_objects.find(a_actor);
		if (it == m_objects.end())
		{
			return false;
		}

		auto handle = it->second.GetHandle();

		NiPointer<TESObjectREFR> ref;
		(void)handle.LookupZH(ref);

		CleanupActorObjectsImpl(
			nullptr,
			handle,
			it->second,
			a_flags);

		m_objects.erase(it);

		return true;
	}

	void IObjectManager::CleanupActorObjectsImpl(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		ActorObjectHolder&               a_objects,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		if (a_actor && a_actor == *g_thePlayer)
		{
			m_playerState.insert(a_objects);
		}

		if (a_objects.m_actor->loadedState)
		{
			for (auto& e : a_objects.m_cmeNodes)
			{
				ResetNodeOverride(e.second);
			}

			for (auto& e : a_objects.m_weapNodes)
			{
				ResetNodePlacement(e, nullptr);
			}
		}

		a_objects.visit([&](objectEntryBase_t& a_object) {
			RemoveObject(
				a_actor,
				a_handle,
				a_object,
				a_objects,
				a_flags);
		});

		for (auto& e : a_objects.m_entriesCustom)
		{
			e.clear();
		}

		a_objects.m_cmeNodes.clear();
		a_objects.m_movNodes.clear();
		a_objects.m_weapNodes.clear();
		a_objects.m_monitorNodes.clear();
	}

	void IObjectManager::RemoveActorGear(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto it = m_objects.find(a_actor->formID);
		if (it != m_objects.end())
		{
			RemoveActorGear(a_actor, a_handle, it->second, a_flags);
		}
	}

	void IObjectManager::RemoveActorGear(
		TESObjectREFR*                   a_actor,
		Game::ObjectRefHandle            a_handle,
		ActorObjectHolder&               a_objects,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		a_objects.visit([&](objectEntryBase_t& a_object) {
			RemoveObject(
				a_actor,
				a_handle,
				a_object,
				a_objects,
				a_flags);
		});

		for (auto& e : a_objects.m_entriesCustom)
		{
			e.clear();
		}
	}

	bool IObjectManager::RemoveInvisibleObjects(
		ActorObjectHolder&    a_objects,
		Game::ObjectRefHandle a_handle)
	{
		bool result = false;

		for (auto& e : a_objects.m_entriesSlot)
		{
			if (e.state &&
			    !e.state->nodes.rootNode->IsVisible())
			{
				RemoveObject(
					nullptr,
					a_handle,
					e,
					a_objects,
					ControllerUpdateFlags::kNone);

				result = true;
			}
		}

		for (auto& e : a_objects.m_entriesCustom)
		{
			for (auto it1 = e.begin(); it1 != e.end();)
			{
				for (auto it2 = it1->second.begin(); it2 != it1->second.end();)
				{
					if (it2->second.state &&
					    !it2->second.state->nodes.rootNode->IsVisible())
					{
						RemoveObject(
							nullptr,
							a_handle,
							it2->second,
							a_objects,
							ControllerUpdateFlags::kNone);

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

	void IObjectManager::ClearObjectsImpl()
	{
		for (auto& e : m_objects)
		{
			auto handle = e.second.GetHandle();

			NiPointer<TESObjectREFR> ref;
			LookupREFRByHandle(handle, ref);

			CleanupActorObjectsImpl(
				nullptr,
				handle,
				e.second,
				ControllerUpdateFlags::kNone);
		}

		m_objects.clear();
	}

	bool IObjectManager::ConstructArmorNode(
		TESForm*                                          a_form,
		const std::vector<TESObjectARMA*>&                a_in,
		bool                                              a_isFemale,
		std::vector<ObjectDatabase::ObjectDatabaseEntry>& a_dbEntries,
		NiPointer<NiNode>&                                a_out)
	{
		bool result = false;

		for (auto& e : a_in)
		{
			auto texSwap = std::addressof(e->models[0][a_isFemale ? 1 : 0]);
			auto path    = texSwap->GetModelName();

			if (!path || path[0] == 0)
			{
				texSwap = std::addressof(e->models[0][a_isFemale ? 0 : 1]);
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

			char buffer[NODE_NAME_BUFFER_SIZE];

			stl::snprintf(
				buffer,
				"IED ARMA [%.8X/%.8X]",
				a_form->formID.get(),
				e->formID.get());

			object->m_name = buffer;

			EngineExtensions::ApplyTextureSwap(texSwap, object);

			a_out->AttachChild(object, true);

			if (entry)
			{
				a_dbEntries.emplace_back(std::move(entry));
			}

			result = true;
		}

		return result;
	}

	void IObjectManager::GetNodeName(
		TESForm*             a_form,
		const modelParams_t& a_params,
		char (&a_out)[NODE_NAME_BUFFER_SIZE])
	{
		switch (a_params.type)
		{
		case ModelType::kWeapon:
			GetWeaponNodeName(a_form->formID, a_out);
			break;
		case ModelType::kArmor:
			GetArmorNodeName(
				a_form->formID,
				a_params.arma ?
					a_params.arma->formID :
                    Game::FormID{},
				a_out);
			break;
		case ModelType::kMisc:
		case ModelType::kLight:
			GetMiscNodeName(a_form->formID, a_out);
			break;
		default:
			HALT("FIXME");
		}
	}

	bool IObjectManager::LoadAndAttach(
		processParams_t&                a_params,
		const Data::configBaseValues_t& a_config,
		objectEntryBase_t&              a_objectEntry,
		TESForm*                        a_form,
		TESForm*                        a_modelForm,
		bool                            a_leftWeapon,
		bool                            a_visible,
		bool                            a_disableHavok,
		bool                            a_enableDeadScatter)
	{
		RemoveObject(
			a_params.actor,
			a_params.handle,
			a_objectEntry,
			a_params.objects,
			a_params.flags);

		if (!a_config.targetNode)
		{
			return false;
		}

		if (a_form->formID.IsTemporary())
		{
			return false;
		}

		if (!a_modelForm)
		{
			a_modelForm = a_form;
		}

		modelParams_t modelParams;

		if (!GetModelParams(
				a_params.actor,
				a_modelForm,
				a_params.race,
				a_params.configSex == Data::ConfigSex::Female,
				a_config.flags.test(Data::BaseFlags::kLoad1pWeaponModel),
				a_config.flags.test(Data::BaseFlags::kUseWorldModel),
				modelParams))
		{
			Debug(
				"[%.8X] [race: %.8X] [item: %.8X] couldn't get model params",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_modelForm->formID.get());

			return false;
		}

		nodesRef_t targetNodes;

		if (!CreateTargetNode(
				a_config,
				a_config.targetNode,
				a_params.npcroot,
				targetNodes))
		{
			Debug(
				"[%.8X] [race: %.8X] [item: %.8X] couldn't get target node: %s",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_modelForm->formID.get(),
				a_config.targetNode.name.c_str());

			return false;
		}

		auto state = std::make_unique<objectEntryBase_t::State>();

		NiPointer<NiNode>   object;
		ObjectDatabaseEntry entry;

		if (!GetUniqueObject(modelParams.path, entry, object))
		{
			Warning(
				"[%.8X] [race: %.8X] [item: %.8X] failed to load model: %s",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_modelForm->formID.get(),
				modelParams.path);

			return false;
		}

		if (entry)
		{
			state->dbEntries.emplace_back(std::move(entry));
		}

		if (modelParams.swap)
		{
			EngineExtensions::ApplyTextureSwap(
				modelParams.swap,
				object);
		}

		object->m_localTransform = {};

		a_params.state.ResetEffectShaders(a_params.handle);

		char buffer[NODE_NAME_BUFFER_SIZE];

		GetNodeName(a_modelForm, modelParams, buffer);

		auto itemRoot = CreateAttachmentNode(buffer);

		state->UpdateData(a_config);
		UpdateObjectTransform(
			state->transform,
			itemRoot,
			targetNodes.ref);

		targetNodes.rootNode->AttachChild(itemRoot, true);
		UpdateDownwardPass(itemRoot);

		auto ar = EngineExtensions::AttachObject(
			a_params.actor,
			a_params.root,
			itemRoot,
			object,
			modelParams.type,
			a_enableDeadScatter && a_params.get_actor_dead(),
			a_leftWeapon,
			modelParams.isShield,
			a_config.flags.test(Data::BaseFlags::kDropOnDeath),
			a_config.flags.test(Data::BaseFlags::kRemoveScabbard),
			a_config.flags.test(Data::BaseFlags::kKeepTorchFlame),
			a_disableHavok);

		UpdateDownwardPass(itemRoot);

		FinalizeObjectState(
			state,
			a_form,
			itemRoot,
			object,
			targetNodes,
			a_config);

		if (a_config.flags.test(Data::BaseFlags::kPlayAnimation))
		{
			state->UpdateAndPlayAnimation(a_params.actor, a_config.niControllerSequence);
		}

		if (ar.test(AttachResultFlags::kScbLeft))
		{
			state->flags.set(ObjectEntryFlags::kScbLeft);
		}

		a_objectEntry.state = std::move(state);

		if (a_visible)
		{
			PlayObjectSound(
				a_params,
				a_config,
				a_objectEntry,
				true);
		}

		return true;
	}

	bool IObjectManager::LoadAndAttachGroup(
		processParams_t&                a_params,
		const Data::configBaseValues_t& a_config,
		const Data::configModelGroup_t& a_group,
		objectEntryBase_t&              a_objectEntry,
		TESForm*                        a_form,
		bool                            a_leftWeapon,
		bool                            a_visible,
		bool                            a_disableHavok,
		bool                            a_enableDeadScatter)
	{
		RemoveObject(
			a_params.actor,
			a_params.handle,
			a_objectEntry,
			a_params.objects,
			a_params.flags);

		if (!a_config.targetNode)
		{
			return false;
		}

		if (a_form->formID.IsTemporary())
		{
			return false;
		}

		auto it = a_group.entries.find({});
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
			modelParams_t                                          params;
			NiPointer<NiNode>                                      object;
			objectEntryBase_t::State::GroupObject*                 grpObject{ nullptr };
		};

		std::list<tmpdata_t> modelParams;

		for (auto& e : a_group.entries)
		{
			if (e.second.flags.test(Data::ConfigModelGroupEntryFlags::kDisabled))
			{
				continue;
			}

			auto form = e.second.form.get_form();
			if (!form)
			{
				continue;
			}

			if (form->formID.IsTemporary())
			{
				continue;
			}

			modelParams_t params;

			if (!GetModelParams(
					a_params.actor,
					form,
					a_params.race,
					a_params.configSex == Data::ConfigSex::Female,
					e.second.flags.test(Data::ConfigModelGroupEntryFlags::kLoad1pWeaponModel),
					a_config.flags.test(Data::BaseFlags::kUseWorldModel) ||
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

			modelParams.emplace_back(
				std::addressof(e),
				form,
				std::move(params));
		}

		if (modelParams.empty())
		{
			return false;
		}

		nodesRef_t targetNodes;

		if (!CreateTargetNode(
				a_config,
				a_config.targetNode,
				a_params.npcroot,
				targetNodes))
		{
			Debug(
				"[%.8X] [race: %.8X] [item: %.8X] couldn't get target node(s): %s",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_form->formID.get(),
				a_config.targetNode.name.c_str());

			return false;
		}

		auto state = std::make_unique<objectEntryBase_t::State>();

		bool loaded = false;

		for (auto& e : modelParams)
		{
			ObjectDatabaseEntry entry;

			if (!GetUniqueObject(e.params.path, entry, e.object))
			{
				Warning(
					"[%.8X] [race: %.8X] [item: %.8X] failed to load model: %s",
					a_params.actor->formID.get(),
					a_params.race->formID.get(),
					e.form->formID.get(),
					e.params.path);

				continue;
			}

			if (entry)
			{
				state->dbEntries.emplace_back(std::move(entry));
			}

			loaded = true;
		}

		if (!loaded)
		{
			return false;
		}

		a_params.state.ResetEffectShaders(a_params.handle);

		char buffer[NODE_NAME_BUFFER_SIZE];

		stl::snprintf(
			buffer,
			StringHolder::FMT_NINODE_IED_GROUP,
			a_form->formID.get());

		auto groupRoot = CreateAttachmentNode(buffer);

		state->UpdateData(a_config);
		UpdateObjectTransform(
			state->transform,
			groupRoot,
			targetNodes.ref);

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

			n.transform.Update(e.entry->second.transform);

			UpdateObjectTransform(
				n.transform,
				n.rootNode,
				nullptr);

			groupRoot->AttachChild(itemRoot, true);
			UpdateDownwardPass(itemRoot);

			EngineExtensions::AttachObject(
				a_params.actor,
				a_params.root,
				itemRoot,
				e.object,
				e.params.type,
				a_enableDeadScatter && a_params.get_actor_dead(),
				a_leftWeapon ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kLeftWeapon),
				e.params.isShield,
				a_config.flags.test(Data::BaseFlags::kDropOnDeath) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kDropOnDeath),
				a_config.flags.test(Data::BaseFlags::kRemoveScabbard) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kRemoveScabbard),
				a_config.flags.test(Data::BaseFlags::kKeepTorchFlame) ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kKeepTorchFlame),
				a_disableHavok ||
					e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kDisableHavok));

			//UpdateDownwardPass(e.object);

			e.grpObject = std::addressof(n);
		}

		UpdateDownwardPass(groupRoot);

		for (auto& e : modelParams)
		{
			if (!e.grpObject)
			{
				continue;
			}

			if (e.entry->second.flags.test(Data::ConfigModelGroupEntryFlags::kPlayAnimation))
			{
				e.grpObject->PlayAnimation(a_params.actor, e.entry->second.niControllerSequence);
			}
		}

		FinalizeObjectState(
			state,
			a_form,
			groupRoot,
			nullptr,
			targetNodes,
			a_config);

		state->flags.set(ObjectEntryFlags::kIsGroup);

		a_objectEntry.state = std::move(state);

		if (a_visible)
		{
			PlayObjectSound(
				a_params,
				a_config,
				a_objectEntry,
				true);
		}

		return true;
	}

	void IObjectManager::FinalizeObjectState(
		std::unique_ptr<objectEntryBase_t::State>& a_state,
		TESForm*                                   a_form,
		NiNode*                                    a_rootNode,
		NiNode*                                    a_objectNode,
		nodesRef_t&                                a_targetNodes,
		const Data::configBaseValues_t&            a_config)
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
	}

	void IObjectManager::PlayObjectSound(
		const processParams_t&          a_params,
		const Data::configBaseValues_t& a_config,
		const objectEntryBase_t&        a_objectEntry,
		bool                            a_equip)
	{
		if (a_objectEntry.state &&
		    a_params.flags.test(ControllerUpdateFlags::kPlaySound) &&
		    a_config.flags.test(Data::BaseFlags::kPlaySound) &&
		    m_playSound)
		{
			if (a_params.is_player() || m_playSoundNPC)
			{
				SoundPlay(
					a_objectEntry.state->form->formType,
					a_objectEntry.state->nodes.rootNode,
					a_equip);
			}
		}
	}

}