#include "pch.h"

#include "../EngineExtensions.h"
#include "IObjectManager.h"

#include <ext/Model.h>

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

		if (a_objectEntry.state->dbEntry)
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
		ActorObjectHolder& a_data,
		stl::flag<ControllerUpdateFlags> a_flags)
	{
		if (a_actor == *g_thePlayer)
		{
			m_playerState = Data::actorStateEntry_t(a_data);
		}

		for (auto& ce : a_data.m_entriesSlot)
		{
			RemoveObject(a_actor, a_handle, ce, a_data, a_flags);
		}

		for (auto& ce : a_data.m_entriesCustom)
		{
			for (auto& cf : ce)
			{
				for (auto& cg : cf.second)
				{
					RemoveObject(a_actor, a_handle, cg.second, a_data, a_flags);
				}
			}
		}

		for (auto& ce : a_data.m_cmeNodes)
		{
			ResetNodeOverride(ce.second);
		}
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
		for (auto& ce : a_objects.m_entriesSlot)
		{
			RemoveObject(a_actor, a_handle, ce, a_objects, a_flags);
		}

		for (auto& ce : a_objects.m_entriesCustom)
		{
			for (auto& cf : ce)
			{
				for (auto& cg : cf.second)
				{
					RemoveObject(a_actor, a_handle, cg.second, a_objects, a_flags);
				}
			}

			ce.clear();
		}
	}

	/*void IObjectManager::RemoveActorByHandleImpl(
		Game::ObjectRefHandle a_mhandle,
		Game::ObjectRefHandle a_rhandle)
	{
		for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
		{
			if (it->second.GetHandle() == a_mhandle)
			{
				__debugbreak();
				_DMESSAGE("bh ; %X : %X | %u | %s", it->second.m_actor->formID, a_mhandle, it->second.m_root->m_uiRefCount, it->second.m_actor->GetDisplayName());

				CleanupActorObjectsImpl(it->second, a_rhandle);

				m_objects.erase(it);
				break;
			}
		}
	}*/

	void IObjectManager::CleanupActorObjectsImpl(
		ActorObjectHolder& a_objects,
		Game::ObjectRefHandle a_rhandle)
	{
		for (auto& ce : a_objects.m_entriesSlot)
		{
			RemoveObject(
				nullptr,
				a_rhandle,
				ce,
				a_objects,
				ControllerUpdateFlags::kNone);
		}

		for (auto& ce : a_objects.m_entriesCustom)
		{
			for (auto& cf : ce)
			{
				for (auto& cg : cf.second)
				{
					RemoveObject(
						nullptr,
						a_rhandle,
						cg.second,
						a_objects,
						ControllerUpdateFlags::kNone);
				}
			}
		}

		for (auto& ce : a_objects.m_cmeNodes)
		{
			ResetNodeOverride(ce.second);
		}
	}

	bool IObjectManager::RemoveInvisibleObjects(
		ActorObjectHolder& a_objects,
		Game::ObjectRefHandle a_handle)
	{
		bool result = false;

		for (auto& ce : a_objects.m_entriesSlot)
		{
			if (ce.state && !ce.state->nodes.obj->IsVisible())
			{
				RemoveObject(
					nullptr,
					a_handle,
					ce,
					a_objects,
					ControllerUpdateFlags::kNone);

				result = true;
			}
		}

		for (auto& ce : a_objects.m_entriesCustom)
		{
			for (auto& cf : ce)
			{
				for (auto& cg : cf.second)
				{
					if (cg.second.state && !cg.second.state->nodes.obj->IsVisible())
					{
						RemoveObject(
							nullptr,
							a_handle,
							cg.second,
							a_objects,
							ControllerUpdateFlags::kNone);

						result = true;
					}
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

			CleanupActorObjectsImpl(e.second, handle);
		}

		m_objects.clear();
	}

	bool IObjectManager::LoadAndAttach(
		processParams_t& a_params,
		const Data::configBaseValues_t& a_config,
		const Data::NodeDescriptor& a_node,
		objectEntryBase_t& a_objectEntry,
		TESForm* a_form,
		bool a_leftWeapon,
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
			/*a_objectEntry.status.set(
				ObjectEntryStatusCode::kTempForm,
				a_params,
				a_form->formID);*/

			return false;
		}

		modelParams_t modelParams;

		if (!GetModelParams(
				a_params.actor,
				a_form,
				a_params.race,
				a_params.configSex == Data::ConfigSex::Female,
				a_config.flags.test(Data::FlagsBase::kLoad1pWeaponModel),
				modelParams))
		{
			/*a_objectEntry.status.set(
				ObjectEntryStatusCode::kModelParamsError,
				a_params,
				a_form->formID);*/

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
			/*a_objectEntry.status.set(
				ObjectEntryStatusCode::kTargetNodeError,
				a_params,
				a_form->formID);*/

			Debug(
				"[%.8X] [race: %.8X] [item: %.8X] failed creating target node: %s",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_form->formID.get(),
				a_node.name.c_str());

			return false;
		}

		NiPointer<NiNode> object;
		ObjectDatabaseEntry dbEntry;

		if (!GetUniqueObject(modelParams.path, dbEntry, object))
		{
			/*a_objectEntry.status.set(
				ObjectEntryStatusCode::kModelLoadError,
				a_params,
				a_form->formID);*/

			Warning(
				"[%.8X] [race: %.8X] [item: %.8X] failed loading model: %s",
				a_params.actor->formID.get(),
				a_params.race->formID.get(),
				a_form->formID.get(),
				modelParams.path);

			return false;
		}

		object->m_localTransform = {};

		if (modelParams.swap)
		{
			EngineExtensions::ApplyTextureSwap(modelParams.swap, object);
		}

		if (!a_params.state.effectShadersReset)
		{
			auto pl = Game::ProcessLists::GetSingleton();
			pl->ResetEffectShaders(a_params.handle);

			a_params.state.effectShadersReset = true;
		}

		char buffer[NODE_NAME_BUFFER_SIZE];

		switch (modelParams.type)
		{
		case ModelType::kWeapon:
			GetWeaponNodeName(a_form->formID, buffer);
			break;
		case ModelType::kArmor:
			GetArmorNodeName(a_form->formID, modelParams.arma, buffer);
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
			targetNodes.ref,
			false);

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
			a_config.flags.test(Data::FlagsBase::kRemoveScabbard));

		a_objectEntry.state->item = a_form->formID;
		a_objectEntry.state->nodes.obj = itemNodeRoot;
		a_objectEntry.state->nodes.ref = std::move(targetNodes.ref);
		a_objectEntry.state->form = a_form;
		a_objectEntry.state->itemType = a_form->formType;
		a_objectEntry.state->nodeDesc = a_node;
		a_objectEntry.state->atmReference = a_node.managed() ||
		                                    a_config.flags.test(Data::FlagsBase::kReferenceMode);
		a_objectEntry.state->dbEntry = std::move(dbEntry);

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