#include "pch.h"

#include "../EngineExtensions.h"
#include "IObjectManager.h"

#include <ext/Model.h>
#include <ext/Node.h>

namespace IED
{
	bool IObjectManager::RemoveObject(
		TESObjectREFR* a_actor,
		Game::ObjectRefHandle a_handle,
		objectEntryBase_t& a_objectEntry,
		const ActorObjectHolder& a_data,
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
			a_objectEntry.state->nodes.obj->m_parent &&
			a_objectEntry.state->nodes.obj->IsVisible())
		{
			SoundPlay(
				a_objectEntry.state->itemType,
				a_objectEntry.state->nodes.obj->m_parent,
				false);
		}

		EngineExtensions::CleanupObject(
			a_handle,
			a_objectEntry.state->nodes.obj,
			a_data.m_root);

		if (!a_objectEntry.state->dbEntries.empty())
		{
			QueueDatabaseCleanup();
		}

		a_objectEntry.Reset();

		return true;
	}

	bool IObjectManager::RemoveActorImpl(
		TESObjectREFR* a_actor,
		Game::ObjectRefHandle a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto it = m_objects.find(a_actor->formID);
		if (it == m_objects.end())
		{
			return false;
		}

		CleanupActorObjectsImpl(a_actor, a_handle, it->second, a_flags);

		m_objects.erase(it);

		return true;
	}

	bool IObjectManager::RemoveActorImpl(
		TESObjectREFR* a_actor,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto it = m_objects.find(a_actor->formID);
		if (it == m_objects.end())
		{
			return false;
		}

		auto handle = it->second.GetHandle();

		NiPointer<TESObjectREFR> ref;
		LookupREFRByHandle(handle, ref);

		CleanupActorObjectsImpl(a_actor, handle, it->second, a_flags);

		m_objects.erase(it);

		return true;
	}

	void IObjectManager::CleanupActorObjectsImpl(
		TESObjectREFR* a_actor,
		Game::ObjectRefHandle a_handle,
		ActorObjectHolder& a_objects,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		if (a_actor == *g_thePlayer)
		{
			m_playerState = Data::actorStateEntry_t(a_objects);
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

		for (auto& e : a_objects.m_cmeNodes)
		{
			ResetNodeOverride(e.second);
		}

		a_objects.m_cmeNodes.clear();
	}

	void IObjectManager::RemoveActorGear(
		TESObjectREFR* a_actor,
		Game::ObjectRefHandle a_handle,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		auto it = m_objects.find(a_actor->formID);
		if (it != m_objects.end())
		{
			RemoveActorGear(a_actor, a_handle, it->second, a_flags);
		}
	}

	void IObjectManager::RemoveActorGear(
		TESObjectREFR* a_actor,
		Game::ObjectRefHandle a_handle,
		ActorObjectHolder& a_objects,
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
		ActorObjectHolder& a_objects,
		Game::ObjectRefHandle a_handle)
	{
		bool result = false;

		for (auto& e : a_objects.m_entriesSlot)
		{
			if (e.state && !e.state->nodes.obj->IsVisible())
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
					if (it2->second.state && !it2->second.state->nodes.obj->IsVisible())
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
		m_objects.clear();
		QueueDatabaseCleanup();
	}

	bool IObjectManager::ConstructArmorNode(
		TESForm* a_form,
		const std::vector<TESObjectARMA*>& a_in,
		bool a_isFemale,
		std::vector<ObjectDatabase::ObjectDatabaseEntry>& a_dbEntries,
		NiPointer<NiNode>& a_out)
	{
		bool result = false;

		for (auto& e : a_in)
		{
			auto texSwap = std::addressof(e->models[0][a_isFemale ? 1 : 0]);
			auto path = texSwap->GetModelName();

			if (!path || path[0] == 0)
			{
				texSwap = std::addressof(e->models[0][a_isFemale ? 0 : 1]);
				path = texSwap->GetModelName();

				if (!path || path[0] == 0)
				{
					continue;
				}
			}

			NiPointer<NiNode> object;
			ObjectDatabaseEntry entry;

			if (!GetUniqueObject(path, entry, object))
			{
				continue;
			}

			if (!a_out)
			{
				a_out = NiNode::Create(1);
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

			object->m_name.Set(buffer);

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

	bool IObjectManager::LoadAndAttach(
		processParams_t& a_params,
		const Data::configBaseValues_t& a_config,
		const Data::NodeDescriptor& a_node,
		objectEntryBase_t& a_objectEntry,
		TESForm* a_form,
		bool a_leftWeapon,
		bool a_loadArma,
		bool a_visible)
	{
		RemoveObject(
			a_params.actor,
			a_params.handle,
			a_objectEntry,
			a_params.objects,
			a_params.flags);

		if (!a_node)
		{
			return false;
		}

		if (a_form->formID.IsTemporary())
		{
			return false;
		}

		modelParams_t modelParams;

		if (!GetModelParams(
				a_params.actor,
				a_form,
				a_params.race,
				a_params.configSex == Data::ConfigSex::Female,
				a_config.flags.test(Data::FlagsBase::kLoad1pWeaponModel),
				a_loadArma,
				modelParams))
		{
			Debug(
				"[%.8X] [race: %.8X] [item: %.8X] couldn't get model params",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_form->formID.get());

			return false;
		}

		nodesRef_t targetNodes;

		if (!CreateTargetNode(
				a_config,
				a_node,
				a_params.npcroot,
				targetNodes))
		{
			Debug(
				"[%.8X] [race: %.8X] [item: %.8X] failed to create target node: %s",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_form->formID.get(),
				a_node.name.c_str());

			return false;
		}

		ASSERT(a_objectEntry.state->dbEntries.empty());

		NiPointer<NiNode> object;

		if (!modelParams.armas)
		{
			ObjectDatabaseEntry entry;

			if (!GetUniqueObject(modelParams.path, entry, object))
			{
				Warning(
					"[%.8X] [race: %.8X] [item: %.8X] failed to load model: %s",
					a_params.actor->formID.get(),
					a_params.race->formID.get(),
					a_form->formID.get(),
					modelParams.path);

				return false;
			}

			if (entry)
			{
				a_objectEntry.state->dbEntries.emplace_back(std::move(entry));
			}

			if (modelParams.swap)
			{
				EngineExtensions::ApplyTextureSwap(modelParams.swap, object);
			}
		}
		else
		{
			if (!ConstructArmorNode(
					a_form,
					*modelParams.armas,
					a_params.configSex == Data::ConfigSex::Female,
					a_objectEntry.state->dbEntries,
					object))
			{
				Warning(
					"[%.8X] [race: %.8X] [item: %.8X] failed to construct armor model",
					a_params.actor->formID.get(),
					a_params.race->formID.get(),
					a_form->formID.get());

				return false;
			}
		}

		object->m_localTransform = {};

		if (!a_params.state.flags.test(ProcessStateUpdateFlags::kEffectShadersReset))
		{
			auto pl = Game::ProcessLists::GetSingleton();
			pl->ResetEffectShaders(a_params.handle);

			a_params.state.flags.set(ProcessStateUpdateFlags::kEffectShadersReset);
		}

		char buffer[NODE_NAME_BUFFER_SIZE];

		switch (modelParams.type)
		{
		case ModelType::kWeapon:
			GetWeaponNodeName(a_form->formID, buffer);
			break;
		case ModelType::kArmor:
			GetArmorNodeName(
				a_form->formID,
				modelParams.arma ? modelParams.arma->formID : 0,
				buffer);
			break;
		case ModelType::kMisc:
		case ModelType::kLight:
			GetMiscNodeName(a_form->formID, buffer);
			break;
		default:
			HALT("FIXME");
		}

		auto itemNodeRoot = CreateNode(buffer);

		targetNodes.obj->AttachChild(itemNodeRoot, true);

		a_objectEntry.UpdateData(a_config);
		UpdateObjectTransform(
			a_objectEntry.state->transform,
			itemNodeRoot,
			targetNodes.ref);

		UpdateDownwardPass(targetNodes.obj);

		auto ar = EngineExtensions::AttachObject(
			a_params.actor,
			a_params.root,
			itemNodeRoot,
			object,
			modelParams.type,
			modelParams.isShield,
			a_leftWeapon,
			a_config.flags.test(Data::FlagsBase::kDropOnDeath),
			a_config.flags.test(Data::FlagsBase::kRemoveScabbard),
			a_config.flags.test(Data::FlagsBase::kKeepTorchFlame));

		a_objectEntry.state->item = a_form->formID;
		a_objectEntry.state->nodes.obj = itemNodeRoot;
		a_objectEntry.state->nodes.ref = std::move(targetNodes.ref);
		a_objectEntry.state->form = a_form;
		a_objectEntry.state->itemType = a_form->formType;
		a_objectEntry.state->nodeDesc = a_node;
		a_objectEntry.state->atmReference = a_node.managed() ||
		                                    a_config.flags.test(Data::FlagsBase::kReferenceMode);

		if (ar.test(AttachResultFlags::kScbLeft))
		{
			a_objectEntry.state->flags.set(ObjectEntryFlags::kScbLeft);
		}

		a_objectEntry.state.mark(true);

		if (a_visible)
		{
			PlayObjectSound(a_params, a_config, a_objectEntry, true);
		}

		return true;
	}

	void IObjectManager::PlayObjectSound(
		const processParams_t& a_params,
		const Data::configBaseValues_t& a_config,
		const objectEntryBase_t& a_objectEntry,
		bool a_equip)
	{
		if (a_objectEntry.state &&
		    a_params.flags.test(ControllerUpdateFlags::kPlaySound) &&
		    a_config.flags.test(Data::FlagsBase::kPlaySound) &&
		    m_playSound)
		{
			if (a_params.actor == *g_thePlayer || m_playSoundNPC)
			{
				SoundPlay(
					a_objectEntry.state->itemType,
					a_objectEntry.state->nodes.obj,
					a_equip);
			}
		}
	}

}  // namespace IED