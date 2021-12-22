#include "pch.h"

#include "Controller/ControllerCommon.h"
#include "EngineExtensions.h"
#include "Util/Logging.h"

#include <ext/IHook.h>
#include <ext/JITASM.h>
#include <ext/VFT.h>

#include <ext/Node.h>

namespace IED
{
	std::unique_ptr<EngineExtensions> EngineExtensions::m_Instance;

	EngineExtensions::EngineExtensions(
		const std::shared_ptr<Controller>& a_controller,
		const std::shared_ptr<Config>& a_config)
	{
		Patch_RemoveAllBipedParts();
		Patch_GarbageCollector();
		Patch_Actor_Resurrect();
		Patch_Actor_3DEvents();

		if (a_config->m_nodeOverrideEnabled)
		{
			m_conf.weaponAdjustDisable = a_config->m_weaponAdjustDisable;
			m_conf.nodeOverridePlayerEnabled = a_config->m_nodeOverridePlayerEnabled;

			Patch_Armor_Update();
			Patch_CreateWeaponNodes();  // not strictly necessary, prevents delays in transform updates
		}

		if (m_conf.weaponAdjustDisable ||
		    a_config->m_weaponAdjustFix)
		{
			m_conf.weaponAdjustFix = a_config->m_weaponAdjustFix;

			Patch_SetWeapAdjAnimVar();
		}

		m_controller = a_controller;
	}

	void EngineExtensions::Patch_RemoveAllBipedParts()
	{
		struct Assembly : JITASM::JITASM
		{
			Assembly(std::uintptr_t targetAddr) :
				JITASM(ISKSE::GetLocalTrampoline())
			{
				Xbyak::Label callLabel;
				Xbyak::Label retnLabel;

				auto& reg = IAL::IsAE() ? rsi : rdi;

				mov(rcx, reg);
				call(ptr[rip + callLabel]);
				lea(rbx, ptr[reg + 0x13C0]);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(targetAddr + 0x7);

				L(callLabel);
				dq(std::uintptr_t(RemoveAllBipedParts_Hook));
			}
		};

		LogPatchBegin(__FUNCTION__);
		{
			Assembly code(m_removeAllBipedParts_a);
			ISKSE::GetBranchTrampoline().Write6Branch(
				m_removeAllBipedParts_a,
				code.get());
		}
		LogPatchEnd(__FUNCTION__);
	}

	void EngineExtensions::Patch_GarbageCollector()
	{
		auto result = Hook::Call5(
			ISKSE::GetBranchTrampoline(),
			m_garbageCollectorREFR_a,
			std::uintptr_t(GarbageCollectorReference_Hook),
			m_garbageCollectorReference_o);

		ASSERT_STR(result, "failed to install garbage collector hook");

		Debug("[%s] Installed garbage collector hook", __FUNCTION__);
	}

	void EngineExtensions::Patch_Actor_Resurrect()
	{
		if (VTable::Detour2(
				m_vtblCharacter_a,
				0xAB,
				Character_Resurrect_Hook,
				&m_characterResurrect_o))
		{
			Debug("[%s] Detoured Character::Resurrect @0xAB", __FUNCTION__);
		}
		else
		{
			Error("[%s] Failed to install Character::Resurrect vtbl hook", __FUNCTION__);
		}

		if (Hook::Call5(
				ISKSE::GetBranchTrampoline(),
				m_reanimActorStateUpdate_a,
				std::uintptr_t(ReanimateActorStateUpdate_Hook),
				m_ReanimActorStateUpd_o))
		{
			Debug("[%s] Installed reanimate hook", __FUNCTION__);
		}
		else
		{
			Error("[%s] Failed to install state update hook", __FUNCTION__);
		}
	}

	void EngineExtensions::Patch_Actor_3DEvents()
	{
		if (VTable::Detour2(
				m_vtblCharacter_a,
				0x6B,
				Character_Release3D_Hook,
				&m_characterRelease3D_o))
		{
			Debug("[%s] Detoured Character::Release3D @0x6B", __FUNCTION__);
		}
		else
		{
			Error("[%s] Failed to install Character::Release3D vtbl hook", __FUNCTION__);
		}

		if (VTable::Detour2(
				m_vtblActor_a,
				0x6B,
				Actor_Release3D_Hook,
				&m_actorRelease3D_o))
		{
			Debug("[%s] Detoured Actor::Release3D @0x6B", __FUNCTION__);
		}
		else
		{
			Error("[%s] Failed to install Actor::Release3D vtbl hook", __FUNCTION__);
		}
	}

	void EngineExtensions::Patch_Armor_Update()
	{
		if (Hook::Call5(
				ISKSE::GetBranchTrampoline(),
				m_armorUpdate_a,
				std::uintptr_t(ArmorUpdate_Hook),
				m_ArmorChange_o))
		{
			Debug("[%s] Installed armor update hook", __FUNCTION__);
		}
		else
		{
			Error("[%s] Failed to install armor update hook", __FUNCTION__);
		}
	}

	void EngineExtensions::Patch_SetWeapAdjAnimVar()
	{
		struct Assembly : JITASM::JITASM
		{
			Assembly(std::uintptr_t targetAddr) :
				JITASM(ISKSE::GetLocalTrampoline())
			{
				Xbyak::Label callLabel;
				Xbyak::Label retnLabel;

				mov(rcx, rsi);
				mov(r9, r13);  // Biped
				call(ptr[rip + callLabel]);
				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(targetAddr + 0x5);

				L(callLabel);
				dq(std::uintptr_t(SetWeapAdjAnimVar_Hook));
			}
		};

		LogPatchBegin(__FUNCTION__);
		{
			Assembly code(m_weapAdj_a);
			ISKSE::GetBranchTrampoline().Write5Branch(
				m_weapAdj_a,
				code.get());
		}
		LogPatchEnd(__FUNCTION__);
	}

	void EngineExtensions::Patch_CreateWeaponNodes()
	{
		struct Assembly : JITASM::JITASM
		{
			Assembly(std::uintptr_t a_targetAddr) :
				JITASM(ISKSE::GetLocalTrampoline())
			{
				Xbyak::Label retnLabel;
				db(reinterpret_cast<Xbyak::uint8*>(a_targetAddr), 0x7);
				jmp(ptr[rip + retnLabel]);
				L(retnLabel);
				dq(a_targetAddr + 0x7);
			}
		};

		LogPatchBegin(__FUNCTION__);
		{
			Assembly code(m_createWeaponNodes_a);
			m_createWeaponNodes_o = code.get<decltype(m_createWeaponNodes_o)>();
			ISKSE::GetBranchTrampoline().Write6Branch(
				m_createWeaponNodes_a,
				std::uintptr_t(CreateWeaponNodes_Hook));
		}
		LogPatchEnd(__FUNCTION__);
	}

	void EngineExtensions::RemoveAllBipedParts_Hook(Biped* a_biped)
	{
		NiPointer<TESObjectREFR> ref;

		if (!a_biped->handle.Lookup(ref))
		{
			return;
		}

		if (!ref->formID)
		{
			return;
		}

		if (auto actor = ref->As<Actor>())
		{
			auto controller = m_Instance->m_controller.get();

			IScopedLock lock(controller->GetLock());

			controller->RemoveActorImpl(actor, a_biped->handle, ControllerUpdateFlags::kNone);
			controller->QueueEvaluate(actor, ControllerUpdateFlags::kNone);
		}
	}

	void EngineExtensions::Character_Resurrect_Hook(
		Character* a_actor,
		bool a_resetInventory,
		bool a_attach3D)
	{
		if (a_attach3D)
		{
			m_Instance->m_controller->QueueReset(a_actor, ControllerUpdateFlags::kNone);

			//_DMESSAGE("resurrect %X", a_actor->formID.get());
		}

		m_Instance->m_characterResurrect_o(a_actor, a_resetInventory, a_attach3D);
	}

	void EngineExtensions::Actor_Release3D_Hook(
		Actor* a_actor)
	{
		if (a_actor->formID)
		{
			m_Instance->m_controller->RemoveActor(a_actor, ControllerUpdateFlags::kNone);
		}

		m_Instance->m_actorRelease3D_o(a_actor);

		if (a_actor->formID)
		{
			m_Instance->m_controller->QueueEvaluate(a_actor, ControllerUpdateFlags::kNone);
		}
	}

	void EngineExtensions::Character_Release3D_Hook(
		Character* a_actor)
	{
		if (a_actor->formID)
		{
			m_Instance->m_controller->RemoveActor(a_actor, ControllerUpdateFlags::kNone);
		}

		m_Instance->m_characterRelease3D_o(a_actor);

		if (a_actor->formID)
		{
			m_Instance->m_controller->QueueEvaluate(a_actor, ControllerUpdateFlags::kNone);
		}
	}

	void EngineExtensions::ReanimateActorStateUpdate_Hook(
		Actor* a_actor,
		bool a_unk1)
	{
		m_Instance->m_ReanimActorStateUpd_o(a_actor, a_unk1);

		if (a_actor->actorState.actorState1.lifeState ==
		    ActorState::ACTOR_LIFE_STATE::kReanimate)
		{
			m_Instance->m_controller->QueueReset(a_actor, ControllerUpdateFlags::kNone);

			//_DMESSAGE("reanimate %X", a_actor->formID.get());
		}
	}

	void EngineExtensions::ArmorUpdate_Hook(
		Game::InventoryChanges* a_ic,
		Game::InitWornVisitor& a_visitor)
	{
		auto formid = a_visitor.actor ?
                          a_visitor.actor->formID :
                          0;

		m_Instance->m_ArmorChange_o(a_ic, a_visitor);

		if (formid)
		{
			m_Instance->m_controller->QueueRequestEvaluateTransforms(formid, false);
		}
	}

	bool EngineExtensions::GarbageCollectorReference_Hook(TESObjectREFR* a_refr)
	{
		if (auto actor = a_refr->As<Actor>())
		{
			m_Instance->m_controller->RemoveActor(actor, ControllerUpdateFlags::kNone);
		}

		return m_Instance->m_garbageCollectorReference_o(a_refr);
	}

	bool EngineExtensions::SetWeapAdjAnimVar_Hook(
		TESObjectREFR* a_refr,
		const BSFixedString& a_animVarName,
		float a_val,
		Biped* a_biped)
	{
		if (m_Instance->m_conf.weaponAdjustDisable)
		{
			if (a_refr != *g_thePlayer || m_Instance->m_conf.nodeOverridePlayerEnabled)
			{
				return a_refr->animGraphHolder.SetVariableOnGraphsFloat(a_animVarName, 0.0f);
			}
		}

		if (m_Instance->m_conf.weaponAdjustFix)
		{
			auto biped = a_refr->GetBiped(false);
			if (!biped || biped->ptr != a_biped)
			{
				return false;
			}
		}

		return a_refr->animGraphHolder.SetVariableOnGraphsFloat(a_animVarName, a_val);
	}

	void EngineExtensions::CreateWeaponNodes_Hook(
		TESObjectREFR* a_actor,
		TESForm* a_object,
		bool a_left)
	{
		m_Instance->m_createWeaponNodes_o(a_actor, a_object, a_left);

		if (a_actor)
		{
			m_Instance->m_controller->QueueRequestEvaluateTransforms(a_actor->formID, true);
		}
	}

	auto EngineExtensions::AttachObject(
		Actor* a_actor,
		NiNode* a_root,
		NiNode* a_targetNode,
		NiNode* a_object,
		ModelType a_modelType,
		bool a_shield,
		bool a_leftWeapon,
		bool a_dropOnDeath,
		bool a_removeScabbards)
		-> stl::flag<AttachResultFlags>
	{
		stl::flag<AttachResultFlags> result{
			AttachResultFlags::kNone
		};

		if (auto bsxFlags = m_Instance->GetBSXFlags(a_object))
		{
			auto flag = static_cast<BSXFlags::Flag>(bsxFlags->m_data);
			if ((flag & BSXFlags::Flag::kAddon) == BSXFlags::Flag::kAddon)
			{
				fUnk28BAD0(a_object);
			}
		}

		AttachAddonNodes(a_object);

		SetRootOnShaderProperties(a_object, a_root);

		a_targetNode->AttachChild(a_object, true);

		NiAVObject::ControllerUpdateContext ctx{
			static_cast<float>(*m_unkglob0),
			0
		};

		a_object->UpdateDownwardPass(ctx, nullptr);

		a_object->IncRef();

		fUnk12ba3e0(*m_shadowSceneNode, a_object);
		fUnk12b99f0(*m_shadowSceneNode, a_object);

		a_object->DecRef();

		auto sh = m_Instance->m_controller->GetBSStringHolder();

		a_object->m_name.Set_ref(sh->m_object);

		if (a_modelType == ModelType::kWeapon)
		{
			auto scbNode = GetObjectByName(a_object, sh->m_scb, true);
			auto scbLeftNode = GetObjectByName(a_object, sh->m_scbLeft, true);

			if (a_removeScabbards)
			{
				if (scbNode || scbLeftNode)
				{
					if (scbNode)
					{
						scbNode->m_parent->RemoveChild(scbNode);
					}

					if (scbLeftNode)
					{
						scbLeftNode->m_parent->RemoveChild(scbLeftNode);
					}

					ShrinkChildrenToSize(a_object);
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

						// does something with NiCollisionObject->bhkWorldObject (target node won't have it but ok)
						fUnkDC6140(a_targetNode, true);
					}

					if (scbRemove)
					{
						scbRemove->m_parent->RemoveChild(scbRemove);
					}

					ShrinkChildrenToSize(a_object);
				}
			}
		}
		else if (a_modelType == ModelType::kLight)
		{
			if (auto torchFireNode = GetObjectByName(a_object, sh->m_torchFire, true))
			{
				torchFireNode->m_parent->RemoveChild(torchFireNode);
				ShrinkChildrenToSize(a_object);

				result.set(AttachResultFlags::kTorchFlameRemoved);
			}
		}

		// collision related, 2nd param = flags
		fUnk1CD130(a_object, 0x0);  //(a_shield || a_leftWeapon) ? 0x12 : (a_weapon ? 0x14 : 0x0));

		fUnk5C3C40(BSTaskPool::GetSingleton(), a_object, a_dropOnDeath ? 4 : 0, true);

		if (auto cell = a_actor->parentCell)
		{
			if (auto world = cell->GetHavokWorld())
			{
				NiPointer<Actor> mountedActor;

				auto isMounted = a_actor->GetMountedActor(mountedActor);

				unks_01 tmp;

				if (auto r = fUnk5EBD90(isMounted ? mountedActor : a_actor, &tmp))
				{
					fUnk5C39F0(BSTaskPool::GetSingleton(), a_object, world, r->p2);
				}
			}
		}

		SetRootOnShaderProperties(a_targetNode, a_root);

		a_actor->UpdateAlpha();

		return result;
	}

	void EngineExtensions::UpdateRoot(NiNode* a_root)
	{
		a_root->UpdateWorldBound();

		NiAVObject::ControllerUpdateContext ctx{ 0, 0x2000 };
		a_root->Update(ctx);

		fUnk12BAFB0(*m_shadowSceneNode, a_root, false);
	}

	void EngineExtensions::SetDropOnDeath(
		Actor* a_actor,
		NiAVObject* a_object,
		bool a_switch)
	{
	}

	void EngineExtensions::CleanupObject(
		Game::ObjectRefHandle a_handle,
		NiAVObject* a_object,
		NiNode* a_root)
	{
		if (!fUnk63F810())
		{
			CleanupNodeImpl(a_handle, a_object);
		}
		else
		{
			//BSTaskPool::GetSingleton()->QueueCleanupNode(a_handle, a_object);

			class NodeCleanupTask :
				public TaskDelegate
			{
			public:
				NodeCleanupTask(
					Game::ObjectRefHandle a_handle,
					NiAVObject* a_object,
					NiNode* a_root) :
					m_handle(a_handle),
					m_object(a_object),
					m_root(a_root)
				{
				}

				virtual void Run() override
				{
					CleanupNodeImpl(m_handle, m_object);

					m_object.reset();
					m_root.reset();
				}

				virtual void Dispose() override
				{
					delete this;
				}

			private:
				Game::ObjectRefHandle m_handle;
				NiPointer<NiAVObject> m_object;
				NiPointer<NiNode> m_root;
			};

			ITaskPool::AddTask<NodeCleanupTask>(
				a_handle,
				a_object,
				a_root);
		}
	}

	BSXFlags* EngineExtensions::GetBSXFlags(NiObjectNET* a_object)
	{
		auto sh = m_controller->GetBSStringHolder();

		if (auto r = FindNiExtraData(a_object, sh->m_bsx))
		{
			return RTTI<BSXFlags>()(r);
		}

		return nullptr;
	}

}  // namespace IED