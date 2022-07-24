#include "pch.h"

#include "Controller/Controller.h"
#include "EngineExtensions.h"
#include "Util/Logging.h"

#include <ext/GarbageCollector.h>
#include <ext/IHook.h>
#include <ext/JITASM.h>
#include <ext/Node.h>
#include <ext/VFT.h>

constexpr static auto mv_failstr = "Memory validation failed";

#define UNWRAP(...) __VA_ARGS__
#define VALIDATE_MEMORY(addr, bytes_se, bytes_ae)                                  \
	{                                                                              \
		if (IAL::IsAE())                                                           \
		{                                                                          \
			ASSERT_STR(Patching::validate_mem(addr, UNWRAP bytes_ae), mv_failstr); \
		}                                                                          \
		else                                                                       \
		{                                                                          \
			ASSERT_STR(Patching::validate_mem(addr, UNWRAP bytes_se), mv_failstr); \
		}                                                                          \
	}

namespace IED
{
	EngineExtensions EngineExtensions::m_Instance;

	void EngineExtensions::Install(
		Controller*                       a_controller,
		const std::shared_ptr<ConfigINI>& a_config)
	{
		m_Instance.InstallImpl(a_controller, a_config);
	}

	void EngineExtensions::InstallImpl(
		Controller*                       a_controller,
		const std::shared_ptr<ConfigINI>& a_config)
	{
		m_controller                    = a_controller;
		m_conf.parallelAnimationUpdates = a_config->m_parallelAnimationUpdates;

		Install_RemoveAllBipedParts();
		Hook_REFR_GarbageCollector();
		Hook_Actor_Resurrect();
		Hook_Actor_3DEvents();

		if (a_config->m_nodeOverrideEnabled)
		{
			m_conf.weaponAdjustDisable       = a_config->m_weaponAdjustDisable;
			m_conf.nodeOverridePlayerEnabled = a_config->m_nodeOverridePlayerEnabled;
			m_conf.disableNPCProcessing      = a_config->m_disableNPCProcessing;

			Hook_Armor_Update();
			Install_CreateWeaponNodes();
		}

		if (a_config->m_weaponAdjustFix)
		{
			m_conf.weaponAdjustFix = a_config->m_weaponAdjustFix;

			Install_SetWeapAdjAnimVar();
		}

		if (m_conf.weaponAdjustDisable)
		{
			Install_WeaponAdjustDisable();
		}

		if (a_config->m_immediateFavUpdate)
		{
			Hook_ToggleFav();
		}

		if (a_config->m_effectShaders)
		{
			Hook_ProcessEffectShaders();
		}

		if (m_conf.parallelAnimationUpdates)
		{
			Install_ParallelAnimationUpdate();
		}
	}

	void EngineExtensions::Install_RemoveAllBipedParts()
	{
		VALIDATE_MEMORY(
			m_removeAllBipedParts_a.get(),
			({ 0x40, 0x57, 0x48, 0x83, 0xEC, 0x30 }),
			({ 0x40, 0x56, 0x57, 0x41, 0x56, 0x48, 0x83, 0xEC, 0x30 }));

		struct Assembly : JITASM::JITASM
		{
			Assembly(std::uintptr_t a_targetAddr) :
				JITASM(ISKSE::GetLocalTrampoline())
			{
				Xbyak::Label retnLabel;

				std::size_t size = IAL::IsAE() ? 0x9 : 0x6;

				db(reinterpret_cast<Xbyak::uint8*>(a_targetAddr), size);
				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(a_targetAddr + size);
			}
		};

		LogPatchBegin();
		{
			Assembly code(m_removeAllBipedParts_a.get());
			m_removeAllBipedParts_o = code.get<decltype(m_removeAllBipedParts_o)>();

			ISKSE::GetBranchTrampoline().Write6Branch(
				m_removeAllBipedParts_a.get(),
				std::uintptr_t(RemoveAllBipedParts_Hook));
		}
		LogPatchEnd();
	}

	void EngineExtensions::Hook_REFR_GarbageCollector()
	{
		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_garbageCollectorREFR_a.get(),
				std::uintptr_t(GarbageCollectorReference_Hook),
				m_garbageCollectorReference_o))
		{
			Debug("[%s] Installed", __FUNCTION__);
		}
		else
		{
			HALT("Failed to install garbage collector hook");
		}
	}

	void EngineExtensions::Hook_Actor_Resurrect()
	{
		if (VTable::Detour2(
				m_vtblCharacter_a.get(),
				0xAB,
				Character_Resurrect_Hook,
				&m_characterResurrect_o))
		{
			Debug("[%s] Detoured Character::Resurrect @0xAB", __FUNCTION__);
		}
		else
		{
			HALT("Failed to install Character::Resurrect vtbl hook");
		}

		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_reanimActorStateUpdate_a.get(),
				std::uintptr_t(ReanimateActorStateUpdate_Hook),
				m_ReanimActorStateUpd_o))
		{
			Debug("[%s] Installed reanimate hook", __FUNCTION__);
		}
		else
		{
			HALT("Failed to install state update hook");
		}
	}

	void EngineExtensions::Hook_Actor_3DEvents()
	{
		if (VTable::Detour2(
				m_vtblCharacter_a.get(),
				0x6B,
				Character_Release3D_Hook,
				&m_characterRelease3D_o))
		{
			Debug("[%s] Detoured Character::Release3D @0x6B", __FUNCTION__);
		}
		else
		{
			HALT("Failed to install Character::Release3D vtbl hook");
		}

		if (VTable::Detour2(
				m_vtblActor_a.get(),
				0x6B,
				Actor_Release3D_Hook,
				&m_actorRelease3D_o))
		{
			Debug("[%s] Detoured Actor::Release3D @0x6B", __FUNCTION__);
		}
		else
		{
			HALT("Failed to install Actor::Release3D vtbl hook");
		}
	}

	void EngineExtensions::Hook_Armor_Update()
	{
		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_armorUpdate_a.get(),
				std::uintptr_t(ArmorUpdate_Hook),
				m_ArmorChange_o))
		{
			Debug("[%s] Installed", __FUNCTION__);
		}
		else
		{
			HALT("Failed to install armor update hook");
		}
	}

	void EngineExtensions::Install_SetWeapAdjAnimVar()
	{
		VALIDATE_MEMORY(
			m_weapAdj_a.get(),
			({ 0xE8 }),
			({ 0xE8 }));

		struct Assembly : JITASM::JITASM
		{
			Assembly() :
				JITASM(ISKSE::GetLocalTrampoline())
			{
				Xbyak::Label callLabel;

				mov(rcx, rsi);
				mov(r9, r13);  // Biped
				jmp(ptr[rip + callLabel]);

				L(callLabel);
				dq(std::uintptr_t(SetWeapAdjAnimVar_Hook));
			}
		};

		LogPatchBegin();
		{
			Assembly code;
			ISKSE::GetBranchTrampoline().Write5Call(
				m_weapAdj_a.get(),
				code.get());
		}
		LogPatchEnd();
	}

	void EngineExtensions::Install_CreateWeaponNodes()
	{
		VALIDATE_MEMORY(
			m_createWeaponNodes_a.get(),
			({ 0x40, 0x56, 0x57, 0x41, 0x54, 0x41, 0x56 }),
			({ 0x40, 0x56, 0x57, 0x41, 0x54, 0x41, 0x56 }));

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

		LogPatchBegin();
		{
			Assembly code(m_createWeaponNodes_a.get());
			m_createWeaponNodes_o = code.get<decltype(m_createWeaponNodes_o)>();
			ISKSE::GetBranchTrampoline().Write6Branch(
				m_createWeaponNodes_a.get(),
				std::uintptr_t(CreateWeaponNodes_Hook));
		}
		LogPatchEnd();
	}

	// actually blocks the node from havok entirely
	void EngineExtensions::Install_WeaponAdjustDisable()
	{
		if (!hook::get_dst5<0xE8>(
				m_hkaLookupSkeletonBones_a.get(),
				m_hkaLookupSkeletonNode_o))
		{
			HALT("Couldn't get hkaLookupSkeletonBones call address");
		}

		struct Assembly : JITASM::JITASM
		{
			Assembly() :
				JITASM(ISKSE::GetLocalTrampoline())
			{
				Xbyak::Label callLabel;

				mov(r9, IAL::IsAE() ? r13 : r15);
				jmp(ptr[rip + callLabel]);

				L(callLabel);
				dq(std::uintptr_t(hkaLookupSkeletonNode_Hook));
			}
		};

		LogPatchBegin();
		{
			Assembly code;

			ISKSE::GetBranchTrampoline().Write5Call(
				m_hkaLookupSkeletonBones_a.get(),
				code.get());
		}
		LogPatchEnd();
	}

	void EngineExtensions::Hook_ToggleFav()
	{
		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_toggleFav1_a.get(),
				std::uintptr_t(ToggleFavGetExtraList_Hook),
				m_toggleFavGetExtraList_o))
		{
			Debug("[%s] Installed", __FUNCTION__);
		}
		else
		{
			Error("[%s] Failed", __FUNCTION__);
		}
	}

	void EngineExtensions::Hook_ProcessEffectShaders()
	{
		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_processEffectShaders_a.get(),
				std::uintptr_t(ProcessEffectShaders_Hook),
				m_processEffectShaders_o))
		{
			Debug("[%s] Installed", __FUNCTION__);
		}
		else
		{
			Error("[%s] Failed", __FUNCTION__);
		}
	}

	void EngineExtensions::Install_ParallelAnimationUpdate()
	{
		auto addrRefUpdate    = m_animUpdateRef_a.get() + 0x74;
		auto addrPlayerUpdate = m_animUpdatePlayer_a.get() + 0xD0;

		VALIDATE_MEMORY(
			addrRefUpdate,
			({ 0xE8 }),
			({ 0xE8 }));

		VALIDATE_MEMORY(
			addrPlayerUpdate,
			({ 0xFF, 0x90, 0xF0, 0x03, 0x00, 0x00 }),
			({ 0xFF, 0x90, 0xF0, 0x03, 0x00, 0x00 }));

		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_animUpdateDispatcher_a.get() + (IAL::IsAE() ? 0xB9 : 0xC0),
				std::uintptr_t(PrepareAnimUpdateLists_Hook),
				m_prepareAnimUpdateLists_o))
		{
			Debug("[%s] Installed anim list prep hook", __FUNCTION__);
		}
		else
		{
			HALT("Failed to install anim list prep hook");
		}

		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_animUpdateDispatcher_a.get() + (IAL::IsAE() ? 0xEB : 0xF2),
				std::uintptr_t(ClearAnimUpdateLists_Hook),
				m_clearAnimUpdateLists_o))
		{
			Debug("[%s] Installed post anim update hook", __FUNCTION__);
		}
		else
		{
			HALT("Failed to install post anim update hook");
		}

		std::uintptr_t dummy;
		if (hook::call(
				ISKSE::GetBranchTrampoline(),
				addrRefUpdate,
				std::uintptr_t(UpdateReferenceAnimations),
				dummy))
		{
			Debug("[%s] Replaced reference anim update call", __FUNCTION__);
		}
		else
		{
			HALT("Failed to replace reference anim update call");
		}

		LogPatchBegin();
		{
			struct Assembly : JITASM::JITASM
			{
				Assembly(std::uintptr_t a_callAddr) :
					JITASM(ISKSE::GetLocalTrampoline())
				{
					Xbyak::Label callLabel;

					lea(rdx, ptr[rsp + 0x8 + 0x28]);  // +0x8 for return address
					jmp(ptr[rip + callLabel]);

					L(callLabel);
					dq(a_callAddr);
				}
			};

			Assembly code(
				reinterpret_cast<std::uintptr_t>(UpdatePlayerAnim_Hook));

			ISKSE::GetBranchTrampoline().Write6Call(
				addrPlayerUpdate,
				code.get());
		}
		LogPatchEnd();
	}

	void EngineExtensions::RemoveAllBipedParts_Hook(Biped* a_biped)
	{
		{
			NiPointer<TESObjectREFR> ref;

			if (a_biped->handle.Lookup(ref) && ref->formID)
			{
				if (auto actor = ref->As<Actor>())
				{
					if (ITaskPool::IsRunningOnCurrentThread())
					{
						m_Instance.FailsafeCleanupAndEval(actor);
					}
					else
					{
						m_Instance.m_controller->RemoveActor(
							actor,
							a_biped->handle,
							ControllerUpdateFlags::kNone);

						m_Instance.m_controller->QueueEvaluate(
							actor,
							ControllerUpdateFlags::kImmediateTransformUpdate);
					}
				}
			}
		}

		m_Instance.m_removeAllBipedParts_o(a_biped);
	}

	void EngineExtensions::Character_Resurrect_Hook(
		Character* a_actor,
		bool       a_resetInventory,
		bool       a_attach3D)
	{
		if (a_attach3D)
		{
			m_Instance.m_controller->QueueReset(
				a_actor,
				ControllerUpdateFlags::kImmediateTransformUpdate);

			//_DMESSAGE("resurrect %X", a_actor->formID.get());
		}

		m_Instance.m_characterResurrect_o(a_actor, a_resetInventory, a_attach3D);
	}

	void EngineExtensions::Actor_Release3D_Hook(
		Actor* a_actor)
	{
		bool eval = false;

		if (a_actor->formID)
		{
			if (ITaskPool::IsRunningOnCurrentThread())
			{
				m_Instance.FailsafeCleanupAndEval(a_actor);
			}
			else
			{
				m_Instance.m_controller->RemoveActor(
					a_actor,
					ControllerUpdateFlags::kNone);

				eval = true;
			}
		}

		m_Instance.m_actorRelease3D_o(a_actor);

		if (eval)
		{
			m_Instance.m_controller->QueueEvaluate(
				a_actor,
				ControllerUpdateFlags::kImmediateTransformUpdate);
		}
	}

	void EngineExtensions::Character_Release3D_Hook(
		Character* a_actor)
	{
		bool eval = false;

		if (a_actor->formID)
		{
			if (ITaskPool::IsRunningOnCurrentThread())
			{
				m_Instance.FailsafeCleanupAndEval(a_actor);
			}
			else
			{
				m_Instance.m_controller->RemoveActor(
					a_actor,
					ControllerUpdateFlags::kNone);

				eval = true;
			}
		}

		m_Instance.m_characterRelease3D_o(a_actor);

		if (eval)
		{
			m_Instance.m_controller->QueueEvaluate(
				a_actor,
				ControllerUpdateFlags::kImmediateTransformUpdate);
		}
	}

	void EngineExtensions::FailsafeCleanupAndEval(
		Actor*                     a_actor,
		const std::source_location a_loc)
	{
		ITaskPool::AddTask([this,
		                    fid    = a_actor->formID,
		                    handle = a_actor->GetHandle()]() {
			m_controller->RemoveActor(
				fid,
				ControllerUpdateFlags::kNone);

			m_controller->QueueEvaluate(
				handle,
				ControllerUpdateFlags::kImmediateTransformUpdate);
		});

		Debug(
			"[%.8X] [%s]: called from ITaskPool",
			a_actor->formID.get(),
			a_loc.function_name());
	}

	void EngineExtensions::ReanimateActorStateUpdate_Hook(
		Actor* a_actor,
		bool   a_unk1)
	{
		m_Instance.m_ReanimActorStateUpd_o(a_actor, a_unk1);

		if (a_actor->actorState1.lifeState ==
		    ActorState::ACTOR_LIFE_STATE::kReanimate)
		{
			m_Instance.m_controller->QueueReset(
				a_actor,
				ControllerUpdateFlags::kImmediateTransformUpdate);

			//_DMESSAGE("reanimate %X", a_actor->formID.get());
		}
	}

	void EngineExtensions::ArmorUpdate_Hook(
		Game::InventoryChanges* a_ic,
		Game::InitWornVisitor&  a_visitor)
	{
		auto formid = a_visitor.actor ?
		                  a_visitor.actor->formID :
                          0;

		m_Instance.m_ArmorChange_o(a_ic, a_visitor);

		if (formid)
		{
			m_Instance.m_controller->QueueRequestEvaluate(formid, false, true, true);
		}
	}

	bool EngineExtensions::GarbageCollectorReference_Hook(TESObjectREFR* a_refr)
	{
		if (auto actor = a_refr->As<Actor>())
		{
			m_Instance.m_controller->RemoveActor(actor, ControllerUpdateFlags::kNone);
		}

		return m_Instance.m_garbageCollectorReference_o(a_refr);
	}

	bool EngineExtensions::SetWeapAdjAnimVar_Hook(
		TESObjectREFR*       a_refr,
		const BSFixedString& a_animVarName,
		float                a_val,
		Biped*               a_biped)
	{
		if (m_Instance.m_conf.weaponAdjustFix)
		{
			auto& biped3p = a_refr->GetBiped1(false);
			if (!biped3p || biped3p.get() != a_biped)
			{
				return false;
			}
		}

		return a_refr->SetVariableOnGraphsFloat(a_animVarName, a_val);
	}

	BaseExtraList* EngineExtensions::ToggleFavGetExtraList_Hook(TESObjectREFR* a_actor)
	{
		m_Instance.m_controller->QueueRequestEvaluate(a_actor->formID, true, false);

		return m_Instance.m_toggleFavGetExtraList_o(a_actor);
	}

	void EngineExtensions::ProcessEffectShaders_Hook(
		Game::ProcessLists* a_pl,
		float               a_frameTimerSlow)
	{
		m_Instance.m_processEffectShaders_o(a_pl, a_frameTimerSlow);

		m_Instance.m_controller->ProcessEffectShaders();
	}

	bool EngineExtensions::hkaLookupSkeletonNode_Hook(
		NiNode*                   a_root,
		const BSFixedString&      a_name,
		hkaGetSkeletonNodeResult& a_result,
		const RE::hkaSkeleton&    a_hkaSkeleton)
	{
		if (a_hkaSkeleton.name &&
		    _stricmp(a_hkaSkeleton.name, StringHolder::HK_NPC_ROOT) == 0)
		{
			if (auto sh = BSStringHolder::GetSingleton())
			{
				if (a_name == sh->m_weaponAxe ||
				    a_name == sh->m_weaponMace ||
				    a_name == sh->m_weaponSword ||
				    a_name == sh->m_weaponDagger ||
				    a_name == sh->m_weaponBack ||
				    a_name == sh->m_weaponBow ||
				    a_name == sh->m_quiver)
				{
					a_result.root  = nullptr;
					a_result.unk08 = std::numeric_limits<std::uint32_t>::max();

					return false;
				}
			}
		}

		return m_Instance.m_hkaLookupSkeletonNode_o(a_root, a_name, a_result);
	}

	void EngineExtensions::PrepareAnimUpdateLists_Hook(
		Game::ProcessLists* a_pl,
		void*               a_unk)
	{
		m_Instance.BeginAnimationUpdate(m_Instance.m_controller);
		m_Instance.m_prepareAnimUpdateLists_o(a_pl, a_unk);
	}

	void EngineExtensions::ClearAnimUpdateLists_Hook(std::uint32_t a_unk)
	{
		m_Instance.EndAnimationUpdate(m_Instance.m_controller);
		m_Instance.m_clearAnimUpdateLists_o(a_unk);
	}

	const RE::BSTSmartPointer<Biped>& IED::EngineExtensions::UpdatePlayerAnim_Hook(
		TESObjectREFR*               a_player,
		const BSAnimationUpdateData& a_data)
	{
		auto& bip = a_player->GetBiped1(false);

		if (bip)  // skip if no biped data
		{
			if (auto actor = a_player->As<Actor>())
			{
				UpdateActorAnimationList(
					actor,
					a_data,
					m_Instance.m_controller);
			}
		}

		return bip;
	}

	void EngineExtensions::CreateWeaponNodes_Hook(
		TESObjectREFR* a_actor,
		TESForm*       a_object,
		bool           a_left)
	{
		m_Instance.m_createWeaponNodes_o(a_actor, a_object, a_left);

		if (a_actor)
		{
			m_Instance.m_controller->QueueRequestEvaluate(
				a_actor->formID,
				false,
				true,
				true);
		}
	}

	bool EngineExtensions::RemoveAllChildren(
		NiNode*              a_object,
		const BSFixedString& a_name)
	{
		bool result = false;

		// some massive paranoia
		std::uint32_t maxiter = 1000;

		while (auto object = GetObjectByName(a_object, a_name, true))
		{
			object->m_parent->DetachChild2(object);
			result = true;

			if (!--maxiter)
			{
				break;
			}
		}

		return result;
	}

	auto EngineExtensions::AttachObject(
		Actor*    a_actor,
		NiNode*   a_root,
		NiNode*   a_targetNode,
		NiNode*   a_object,
		ModelType a_modelType,
		bool      a_leftWeapon,
		bool      a_shield,
		bool      a_dropOnDeath,
		bool      a_removeScabbards,
		bool      a_keepTorchFlame,
		bool      a_disableHavok)
		-> stl::flag<AttachResultFlags>
	{
		stl::flag<AttachResultFlags> result{
			AttachResultFlags::kNone
		};

		if (a_disableHavok)
		{
			::Util::Node::Traverse(
				a_object,
				[&](NiAVObject* a_object) {
					a_object->collisionObject.reset();
					return ::Util::Node::VisitorControl::kContinue;
				});

			a_dropOnDeath = false;
		}

		if (auto bsxFlags = GetBSXFlags(a_object))
		{
			stl::flag<BSXFlags::Flag> flags(bsxFlags->m_data);

			if (a_disableHavok &&
			    flags.test(BSXFlags::kHavok))
			{
				// recreate since this isn't cloned

				NiPointer newbsx = BSXFlags::Create(
					flags.value & ~BSXFlags::Flag::kHavok);

				if (auto index = a_object->GetIndexOf(newbsx->m_pcName); index >= 0)
				{
					if (auto& entry = a_object->m_extraData[index]; entry == bsxFlags)
					{
						newbsx->IncRef();
						entry->DecRef();
						entry = newbsx;

						flags = newbsx->m_data;
					}
				}
			}

			if (flags.test(BSXFlags::Flag::kAddon))
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

		a_object->UpdateDownwardPass(ctx, 0);

		a_object->IncRef();

		fUnk12ba3e0(*m_shadowSceneNode, a_object);
		fUnk12b99f0(*m_shadowSceneNode, a_object);

		a_object->DecRef();

		auto sh = BSStringHolder::GetSingleton();

		a_object->m_name = sh->m_object;

		std::uint32_t collisionFilterInfo = 0;

		if (a_modelType == ModelType::kWeapon)
		{
			auto scbNode     = GetObjectByName(a_object, sh->m_scb, true);
			auto scbLeftNode = GetObjectByName(a_object, sh->m_scbLeft, true);

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

			collisionFilterInfo = a_leftWeapon ? 0x12 : 0x14;
		}
		else if (a_modelType == ModelType::kLight)
		{
			if (!a_keepTorchFlame)
			{
				bool shrink = false;

				if (auto node = GetObjectByName(a_object, sh->m_torchFire, true))
				{
					node->m_parent->DetachChild2(node);
					shrink = true;

					result.set(AttachResultFlags::kTorchFlameRemoved);
				}

				bool custRemoved = false;

				custRemoved |= RemoveAllChildren(a_object, sh->m_mxTorchSmoke);
				custRemoved |= RemoveAllChildren(a_object, sh->m_mxTorchSparks);
				custRemoved |= RemoveAllChildren(a_object, sh->m_mxAttachSmoke);
				custRemoved |= RemoveAllChildren(a_object, sh->m_mxAttachSparks);
				custRemoved |= RemoveAllChildren(a_object, sh->m_attachENBLight);
				custRemoved |= RemoveAllChildren(a_object, sh->m_enbFireLightEmitter);
				custRemoved |= RemoveAllChildren(a_object, sh->m_enbTorchLightEmitter);

				if (custRemoved)
				{
					result.set(AttachResultFlags::kTorchCustomRemoved);
				}

				shrink |= custRemoved;

				if (shrink)
				{
					ShrinkToSize(a_object);
				}
			}

			collisionFilterInfo = 0x12;
		}
		else if (a_modelType == ModelType::kArmor)
		{
			if (a_shield)
			{
				collisionFilterInfo = 0x12;
			}
		}

		fUnk1CD130(a_object, collisionFilterInfo);

		QueueAttachHavok(
			BSTaskPool::GetSingleton(),
			a_object,
			a_dropOnDeath ? 4 : 0,
			true);

		if (auto cell = a_actor->GetParentCell())
		{
			if (auto world = cell->GetHavokWorld())
			{
				NiPointer<Actor> mountedActor;

				bool isMounted = a_actor->GetMountedActor(mountedActor);

				unks_01 tmp;

				auto& r = fUnk5EBD90(isMounted ? mountedActor.get() : a_actor, tmp);
				fUnk5C39F0(BSTaskPool::GetSingleton(), a_object, world, r.p2);
			}
		}

		SetRootOnShaderProperties(a_targetNode, a_root);

		a_actor->UpdateAlpha();

		return result;
	}

	void EngineExtensions::CleanupWeaponBehaviorGraph(
		RE::WeaponAnimationGraphManagerHolderPtr& a_graph)
	{
		RE::BSAnimationGraphManagerPtr manager;
		if (a_graph->GetAnimationGraphManagerImpl(manager))
		{
			auto gc = RE::GarbageCollector::GetSingleton();
			assert(gc);
			gc->QueueBehaviorGraph(manager);
		}
	}

	void EngineExtensions::UpdateRoot(NiNode* a_root)
	{
		a_root->UpdateWorldBound();

		NiAVObject::ControllerUpdateContext ctx{ 0, 0x2000 };
		a_root->Update(ctx);

		fUnk12BAFB0(*m_shadowSceneNode, a_root, false);
	}

	BSXFlags* EngineExtensions::GetBSXFlags(NiObjectNET* a_object)
	{
		return a_object->GetExtraData<BSXFlags>(BSStringHolder::GetSingleton()->m_bsx);
	}

	static inline bool should_update(TESObjectREFR* a_refr)
	{
		if (a_refr->GetMustUpdate())
		{
			return true;
		}

		if (auto door = a_refr->baseForm ? a_refr->baseForm->As<TESObjectDOOR>() : nullptr)
		{
			return door->doorFlags.test(TESObjectDOOR::Flag::kAutomatic);
		}

		return false;
	}

	void EngineExtensions::UpdateReferenceAnimations(
		TESObjectREFR* a_refr,
		float          a_step)
	{
		struct TLSData
		{
			std::uint8_t  unk000[0x768];  // 000
			std::uint32_t unk768;         // 768
		};

		auto tlsData = reinterpret_cast<TLSData**>(__readgsqword(0x58));

		auto& tlsUnk768 = tlsData[*tlsIndex]->unk768;

		auto oldUnk768 = tlsUnk768;
		tlsUnk768      = 0x3A;

		BSAnimationUpdateData data{ a_step };
		data.reference    = a_refr;
		data.shouldUpdate = should_update(a_refr);

		a_refr->ModifyAnimationUpdateData(data);
		UpdateAnimationGraph(a_refr, data);

		if (auto& bip = a_refr->GetBiped2())
		{
			for (auto& e : bip->objects)
			{
				if (e.weaponAnimationGraphManagerHolder)
				{
					UpdateAnimationGraph(e.weaponAnimationGraphManagerHolder.get(), data);
				}
			}

			if (auto actor = a_refr->As<Actor>())
			{
				m_Instance.ProcessAnimationUpdateList(actor, data, m_Instance.m_controller);
			}
		}

		tlsUnk768 = oldUnk768;
	}

}