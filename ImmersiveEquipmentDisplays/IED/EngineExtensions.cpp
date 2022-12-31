#include "pch.h"

#include "EngineExtensions.h"

#include "Controller/Controller.h"

#include "Util/Logging.h"

#include "ReferenceLightController.h"
#include "SkeletonExtensions.h"

#include <ext/GarbageCollector.h>
#include <ext/IHook.h>
#include <ext/JITASM.h>
#include <ext/MemoryValidation.h>
#include <ext/Node.h>
#include <ext/VFT.h>

#define UNWRAP(...) __VA_ARGS__
#define VALIDATE_MEMORY(addr, bytes_se, bytes_ae) \
	stl::validate_memory_with_report_and_fail(addr, UNWRAP bytes_se, UNWRAP bytes_ae, PLUGIN_NAME)

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
		m_conf.applyTransformOverrides  = a_config->m_applyTransformOverrides;
		//m_conf.enableLights             = a_config->m_enableLights;

		Install_RemoveAllBipedParts();
		Hook_REFR_GarbageCollector();
		Hook_Actor_Resurrect();
		Hook_Actor_3DEvents();

		if (a_config->m_nodeOverrideEnabled)
		{
			m_conf.weaponAdjustDisable       = a_config->m_weaponAdjustDisable;
			m_conf.weaponAdjustDisableForce  = a_config->m_weaponAdjustForceDisable;
			m_conf.nodeOverridePlayerEnabled = a_config->m_nodeOverridePlayerEnabled;
			m_conf.disableNPCProcessing      = a_config->m_disableNPCProcessing;

			Hook_Armor_Update();
			Install_CreateWeaponNodes();
		}

		if (a_config->m_enableEarlyLoadHooks)
		{
			Install_PostLoad3DHooks();
		}

		if (a_config->m_weaponAdjustFix)
		{
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

		if (a_config->m_parallelAnimationUpdates)
		{
			Install_ParallelAnimationUpdate();
		}

		if (a_config->m_enableLights)
		{
			Install_Lighting();
		}
	}

	template <class T>
	void EngineExtensions::InstallVtableDetour(
		const IAL::Address<std::uintptr_t>& a_vtblAddr,
		std::ptrdiff_t                      a_offset,
		T*                                  a_hookFunc,
		T*&                                 a_originalFunc,
		bool                                a_critical,
		const char*                         a_desc,
		std::source_location                a_src)
	{
		if (VTable::Detour2(
				a_vtblAddr.get(),
				a_offset,
				a_hookFunc,
				std::addressof(a_originalFunc)))
		{
			Debug(
				"[%s] Detoured %s @0x%llX:0x%llX",
				a_src.function_name(),
				a_desc,
				a_vtblAddr.get(),
				a_offset);
		}
		else
		{
			char buf[128];
			stl::snprintf(
				buf,
				"[%s] Failed to install %s vtbl hook @0x%llX:0x%llX",
				a_src.function_name(),
				a_desc,
				a_vtblAddr.get(),
				a_offset);

			if (a_critical)
			{
				HALT(buf);
			}
			else
			{
				Error(buf);
			}
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
			Debug(
				"[%s] Installed @0x%llX",
				__FUNCTION__,
				m_garbageCollectorREFR_a.get());
		}
		else
		{
			HALT(__FUNCTION__ ": failed to install garbage collector hook");
		}
	}

	void EngineExtensions::Hook_Actor_Resurrect()
	{
		InstallVtableDetour(
			m_vtblCharacter_a,
			0xAB,
			Character_Resurrect_Hook,
			m_characterResurrect_o,
			true,
			"Character::Resurrect");

		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_reanimActorStateUpdate_a.get(),
				std::uintptr_t(ReanimateActorStateUpdate_Hook),
				m_ReanimActorStateUpd_o))
		{
			Debug(
				"[%s] Installed reanimate hook @0x%llX",
				__FUNCTION__,
				m_reanimActorStateUpdate_a.get());
		}
		else
		{
			HALT(__FUNCTION__ ": failed to install state update hook");
		}
	}

	void EngineExtensions::Hook_Actor_3DEvents()
	{
		VALIDATE_MEMORY(
			m_refrLoad3DClone_a.get(),
			({ 0xFF, 0x90, 0x50, 0x02, 0x00, 0x00 }),
			({ 0xFF, 0x90, 0x50, 0x02, 0x00, 0x00 }));

		InstallVtableDetour(
			m_vtblActor_a,
			0x6B,
			Actor_Release3D_Hook,
			m_actorRelease3D_o,
			true,
			"Actor::Release3D");

		InstallVtableDetour(
			m_vtblCharacter_a,
			0x6B,
			Character_Release3D_Hook,
			m_characterRelease3D_o,
			true,
			"Character::Release3D");

		InstallVtableDetour(
			m_vtblPlayerCharacter_a,
			0x6B,
			PlayerCharacter_Release3D_Hook,
			m_pcRelease3D_o,
			true,
			"PlayerCharacter::Release3D");
	}

	void EngineExtensions::Install_PostLoad3DHooks()
	{
		ISKSE::GetBranchTrampoline().Write6Call(
			m_refrLoad3DClone_a.get(),
			std::uintptr_t(REFR_Load3D_Clone_Hook));

		Debug(
			"[%s] Installed reference clone 3D hook @0x%llX",
			__FUNCTION__,
			m_refrLoad3DClone_a.get());

		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_playerLoad3DSkel_a.get(),
				std::uintptr_t(PlayerCharacter_Load3D_LoadSkeleton_Hook),
				m_playerLoad3DSkel_o))
		{
			Debug(
				"[%s] Installed player load 3D hook @0x%llX",
				__FUNCTION__,
				m_playerLoad3DSkel_a.get());
		}
		else
		{
			HALT(__FUNCTION__ ": failed to install player load 3D hook");
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
			Debug(
				"[%s] Installed @0x%llX",
				__FUNCTION__,
				m_armorUpdate_a.get());
		}
		else
		{
			HALT(__FUNCTION__ ": failed to install armor update hook");
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
			HALT(__FUNCTION__ ": couldn't get hkaLookupSkeletonBones call address");
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
		bool result = hook::call5(
			ISKSE::GetBranchTrampoline(),
			m_toggleFav1_a.get(),
			std::uintptr_t(ToggleFavGetExtraList_Hook),
			m_toggleFavGetExtraList_o);

		Debug(
			"[%s] %s @0x%llX",
			__FUNCTION__,
			result ? "Installed" :
					 "Failed",
			m_toggleFav1_a.get());
	}

	void EngineExtensions::Install_ParallelAnimationUpdate()
	{
		auto addrRefUpdate      = m_animUpdateRef_a.get() + 0x74;
		auto addrPlayerUpdate   = m_animUpdatePlayer_a.get() + 0xD0;
		auto addrAnimUpdatePre  = m_animUpdateDispatcher_a.get() + (IAL::IsAE() ? 0xB9 : 0xC0);
		auto addrAnimUpdatePost = m_animUpdateDispatcher_a.get() + (IAL::IsAE() ? 0xEB : 0xF2);

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
				addrAnimUpdatePre,
				std::uintptr_t(PrepareAnimUpdateLists_Hook),
				m_prepareAnimUpdateLists_o))
		{
			Debug(
				"[%s] Installed pre anim update hook @0x%llX",
				__FUNCTION__,
				addrAnimUpdatePre);
		}
		else
		{
			HALT(__FUNCTION__ ": failed to install pre anim update hook");
		}

		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_animUpdateDispatcher_a.get() + (IAL::IsAE() ? 0xEB : 0xF2),
				std::uintptr_t(ClearAnimUpdateLists_Hook),
				m_clearAnimUpdateLists_o))
		{
			Debug(
				"[%s] Installed post anim update hook @0x%llX",
				__FUNCTION__,
				addrAnimUpdatePost);
		}
		else
		{
			HALT(__FUNCTION__ ": failed to install post anim update hook");
		}

		std::uintptr_t dummy;
		if (hook::call(
				ISKSE::GetBranchTrampoline(),
				addrRefUpdate,
				std::uintptr_t(UpdateReferenceAnimations),
				dummy))
		{
			Debug(
				"[%s] Replaced reference anim update call @0x%llX",
				__FUNCTION__,
				addrRefUpdate);
		}
		else
		{
			HALT(__FUNCTION__ ": failed to replace reference anim update call");
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

	void EngineExtensions::Install_Lighting()
	{
		auto addr = m_TESObjectCELL_unk_178_a.get() + 0x149;

		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				addr,
				std::uintptr_t(TESObjectCELL_unk_178_Actor_GetExtraLight_Hook),
				m_TESObjectCELL_unk_178_Actor_GetExtraLight_o))
		{
			Debug(
				"[%s] Installed @0x%llX [TESObjectCELL_unk_178]",
				__FUNCTION__,
				addr);
		}
		else
		{
			HALT(__FUNCTION__ ": failed to install TESObjectCELL_unk_178 hook");
		}

		addr = m_PlayerCharacter_unk_205_a.get() + 0x6F5;

		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				addr,
				std::uintptr_t(PlayerCharacter_unk_205_RefreshMagicCasterLights_Hook),
				m_PlayerCharacter_unk_205_RefreshMagicCasterLights_o))
		{
			Debug(
				"[%s] Installed @0x%llX [PlayerCharacter_unk_205]",
				__FUNCTION__,
				addr);
		}
		else
		{
			HALT(__FUNCTION__ ": failed to install PlayerCharacter_unk_205 hook");
		}

		addr = m_PlayerCharacter_RefreshLights_a.get() + 0x4C;

		if (hook::jmp5(
				ISKSE::GetBranchTrampoline(),
				addr,
				std::uintptr_t(PlayerCharacter_RefreshLights_RefreshMagicCasterLights_Hook),
				m_PlayerCharacter_RefreshLights_RefreshMagicCasterLights_o))
		{
			Debug(
				"[%s] Installed @0x%llX [PlayerCharacter_RefreshLights]",
				__FUNCTION__,
				addr);
		}
		else
		{
			HALT(__FUNCTION__ ": failed to install PlayerCharacter_RefreshLights hook");
		}

		addr = m_Actor_Update_Actor_GetExtraLight_a.get() + 0x66B;

		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				addr,
				std::uintptr_t(Actor_Update_Actor_GetExtraLight_Hook),
				m_Actor_Update_Actor_GetExtraLight_o))
		{
			Debug(
				"[%s] Installed @0x%llX [Actor_Update]",
				__FUNCTION__,
				addr);
		}
		else
		{
			HALT(__FUNCTION__ ": failed to install Actor_Update hook");
		}

		/*InstallVtableDetour(
			m_vtblTESObjectREFR_a,
			0x55,
			TESObjectREFR_UpdateRefLight_Hook,
			m_pcUpdateRefLightREFR_o,
			true,
			"TESObjectREFR::UpdateRefLight");

		InstallVtableDetour(
			m_vtblActor_a,
			0x55,
			Actor_UpdateRefLight_Hook,
			m_pcUpdateRefLightActor_o,
			true,
			"Actor::UpdateRefLight");

		InstallVtableDetour(
			m_vtblCharacter_a,
			0x55,
			Character_UpdateRefLight_Hook,
			m_pcUpdateRefLightCharacter_o,
			true,
			"Character::UpdateRefLight");*/

		InstallVtableDetour(
			m_vtblPlayerCharacter_a,
			0x55,
			PlayerCharacter_UpdateRefLight_Hook,
			m_pcUpdateRefLightPlayerCharacter_o,
			true,
			"PlayerCharacter::UpdateRefLight");
	}

	void EngineExtensions::RemoveAllBipedParts_Hook(Biped* a_biped) noexcept
	{
		{
			NiPointer<TESObjectREFR> ref;

			if (a_biped->handle.Lookup(ref) && ref->formID)
			{
				if (auto actor = ref->As<Actor>())
				{
					m_Instance.m_controller->QueueReset(
						actor->formID,
						ControllerUpdateFlags::kImmediateTransformUpdate);
				}
			}
		}

		m_Instance.m_removeAllBipedParts_o(a_biped);
	}

	void EngineExtensions::Character_Resurrect_Hook(
		Character* a_actor,
		bool       a_resetInventory,
		bool       a_attach3D) noexcept
	{
		if (a_attach3D)
		{
			m_Instance.m_controller->QueueResetGear(
				a_actor->formID,
				ControllerUpdateFlags::kImmediateTransformUpdate);

			//_DMESSAGE("resurrect %X", a_actor->formID.get());
		}

		m_Instance.m_characterResurrect_o(a_actor, a_resetInventory, a_attach3D);
	}

	template <class T>
	void EngineExtensions::RunRelease3DHook(T* a_actor, void (*&a_origCall)(T*) noexcept) noexcept
	{
		bool eval = false;

		if (a_actor->formID)
		{
			//m_Instance.Debug("%s: release 3D: %X", __FUNCTION__, a_actor->formID);

			if (ITaskPool::IsRunningOnCurrentThread())
			{
				m_Instance.FailsafeCleanupAndEval(a_actor);
			}
			else
			{
				m_Instance.m_controller->RemoveActor(
					a_actor->formID,
					ControllerUpdateFlags::kDestroyed);

				eval = true;
			}
		}

		a_origCall(a_actor);

		if (eval)
		{
			m_Instance.m_controller->QueueEvaluate(
				a_actor,
				ControllerUpdateFlags::kImmediateTransformUpdate);
		}
	}

	void EngineExtensions::PlayerCharacter_Release3D_Hook(
		PlayerCharacter* a_actor) noexcept
	{
		RunRelease3DHook(a_actor, m_Instance.m_pcRelease3D_o);
	}

	void EngineExtensions::Actor_Release3D_Hook(
		Actor* a_actor) noexcept
	{
		RunRelease3DHook(a_actor, m_Instance.m_actorRelease3D_o);
	}

	void EngineExtensions::Character_Release3D_Hook(
		Character* a_actor) noexcept
	{
		RunRelease3DHook(a_actor, m_Instance.m_characterRelease3D_o);
	}

	NiAVObject* EngineExtensions::REFR_Load3D_Clone_Hook(
		TESBoundObject* a_obj,
		TESObjectREFR*  a_refr) noexcept
	{
		auto result = a_obj->Clone3D2(a_refr);

		if (result && a_refr->IsActor())
		{
			SkeletonExtensions::PostLoad3D(result, GetTransformOverridesEnabled());
		}

		return result;
	}

	std::uint32_t EngineExtensions::PlayerCharacter_Load3D_LoadSkeleton_Hook(
		const char*            a_path,
		NiPointer<NiAVObject>& a_3D,
		std::uint32_t&         a_unk3) noexcept
	{
		auto result = m_Instance.m_playerLoad3DSkel_o(a_path, a_3D, a_unk3);

		if (a_3D)
		{
			SkeletonExtensions::PostLoad3D(a_3D, GetTransformOverridesEnabled());
		}

		return result;
	}

	void EngineExtensions::FailsafeCleanupAndEval(
		Actor*                     a_actor,
		const std::source_location a_loc) noexcept
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
		bool   a_unk1) noexcept
	{
		m_Instance.m_ReanimActorStateUpd_o(a_actor, a_unk1);

		if (a_actor->actorState1.lifeState ==
		    ActorState::ACTOR_LIFE_STATE::kReanimate)
		{
			m_Instance.m_controller->QueueResetGear(
				a_actor->formID,
				ControllerUpdateFlags::kImmediateTransformUpdate);

			//_DMESSAGE("reanimate %X", a_actor->formID.get());
		}
	}

	void EngineExtensions::ArmorUpdate_Hook(
		Game::InventoryChanges* a_ic,
		Game::InitWornVisitor&  a_visitor) noexcept
	{
		const auto formid = a_visitor.actor ?
		                        a_visitor.actor->formID :
		                        Game::FormID{};

		m_Instance.m_ArmorChange_o(a_ic, a_visitor);

		if (formid)
		{
			m_Instance.m_controller->QueueRequestEvaluate(formid, false, true, true);
		}
	}

	bool EngineExtensions::GarbageCollectorReference_Hook(TESObjectREFR* a_refr) noexcept
	{
		if (auto actor = a_refr->As<Actor>())
		{
			//m_Instance.Debug("%s: GC: %X", __FUNCTION__, a_refr->formID);

			m_Instance.m_controller->RemoveActor(
				actor->formID,
				ControllerUpdateFlags::kDestroyed);
		}

		return m_Instance.m_garbageCollectorReference_o(a_refr);
	}

	bool EngineExtensions::SetWeapAdjAnimVar_Hook(
		TESObjectREFR*       a_refr,
		const BSFixedString& a_animVarName,
		float                a_val,
		Biped*               a_biped) noexcept
	{
		auto& biped3p = a_refr->GetBiped1(false);
		if (biped3p && biped3p.get() != a_biped)
		{
			return false;
		}

		return a_refr->SetVariableOnGraphsFloat(a_animVarName, a_val);
	}

	BaseExtraList* EngineExtensions::ToggleFavGetExtraList_Hook(TESObjectREFR* a_actor) noexcept
	{
		m_Instance.m_controller->QueueRequestEvaluate(a_actor->formID, true, false);

		return m_Instance.m_toggleFavGetExtraList_o(a_actor);
	}

	bool EngineExtensions::hkaShouldBlockNode(
		NiAVObject*            a_root,
		const BSFixedString&   a_name,
		const RE::hkaSkeleton& a_hkaSkeleton) noexcept
	{
		if (!a_root)
		{
			return false;
		}

		auto root = a_root->AsNode();
		if (!root)
		{
			return false;
		}

		auto sh = BSStringHolder::GetSingleton();
		if (!sh)
		{
			return false;
		}

		if (!a_hkaSkeleton.name ||
		    _stricmp(a_hkaSkeleton.name, StringHolder::HK_NPC_ROOT) != 0)
		{
			return false;
		}

		if (!sh->IsVanillaSheathNode(a_name))
		{
			return false;
		}

		if (m_Instance.m_conf.weaponAdjustDisableForce)
		{
			return true;
		}

		auto object = root->GetObjectByName(a_name);
		if (!object)
		{
			return false;
		}

		auto parent1 = object->m_parent;
		if (!parent1)
		{
			return false;
		}

		if (_strnicmp(parent1->m_name.data(), "MOV ", 4) != 0)
		{
			return false;
		}

		auto parent2 = parent1->m_parent;
		if (!parent2)
		{
			return false;
		}

		if (_strnicmp(parent2->m_name.data(), "CME ", 4) != 0)
		{
			return false;
		}

		return true;
	}

	bool EngineExtensions::hkaLookupSkeletonNode_Hook(
		NiNode*                   a_root,
		const BSFixedString&      a_name,
		hkaGetSkeletonNodeResult& a_result,
		const RE::hkaSkeleton&    a_hkaSkeleton) noexcept
	{
		if (hkaShouldBlockNode(a_root, a_name, a_hkaSkeleton))
		{
			a_result.root  = nullptr;
			a_result.unk08 = std::numeric_limits<std::uint32_t>::max();

			return false;
		}

		return m_Instance.m_hkaLookupSkeletonNode_o(a_root, a_name, a_result);
	}

	void EngineExtensions::PrepareAnimUpdateLists_Hook(
		Game::ProcessLists* a_pl,
		void*               a_unk) noexcept
	{
		m_Instance.m_prepareAnimUpdateLists_o(a_pl, a_unk);
		m_Instance.BeginAnimationUpdate(m_Instance.m_controller);
	}

	void EngineExtensions::ClearAnimUpdateLists_Hook(std::uint32_t a_unk) noexcept
	{
		m_Instance.EndAnimationUpdate(m_Instance.m_controller);
		m_Instance.m_clearAnimUpdateLists_o(a_unk);
	}

	const RE::BSTSmartPointer<Biped>& IED::EngineExtensions::UpdatePlayerAnim_Hook(
		TESObjectREFR*               a_player,
		const BSAnimationUpdateData& a_data) noexcept
	{
		auto& bip = a_player->GetBiped1(false);

		if (bip)  // skip if no biped data
		{
			if (auto actor = a_player->As<Actor>())
			{
				UpdateActorAnimationList(
					actor,
					a_data,
					*m_Instance.m_controller);
			}
		}

		return bip;
	}

	/*void EngineExtensions::TESObjectREFR_UpdateRefLight_Hook(TESObjectREFR* a_actor) noexcept
	{
		m_Instance.m_pcUpdateRefLightREFR_o(a_actor);
		if (auto actor = a_actor->As<Actor>())
			ReferenceLightController::GetSingleton().OnUpdate(actor);
	}

	void EngineExtensions::Actor_UpdateRefLight_Hook(Actor* a_actor) noexcept
	{
		m_Instance.m_pcUpdateRefLightActor_o(a_actor);
		ReferenceLightController::GetSingleton().OnUpdate(a_actor);
	}

	void EngineExtensions::Character_UpdateRefLight_Hook(Character* a_character) noexcept
	{
		m_Instance.m_pcUpdateRefLightCharacter_o(a_character);
		ReferenceLightController::GetSingleton().OnUpdate(a_character);
	}*/

	void EngineExtensions::PlayerCharacter_UpdateRefLight_Hook(PlayerCharacter* a_player) noexcept
	{
		m_Instance.m_pcUpdateRefLightPlayerCharacter_o(a_player);
		ReferenceLightController::GetSingleton().OnUpdate(a_player);
	}

	REFR_LIGHT* EngineExtensions::TESObjectCELL_unk_178_Actor_GetExtraLight_Hook(Actor* a_actor) noexcept
	{
		ReferenceLightController::GetSingleton().OnActorCrossCellBoundary(a_actor);
		return m_Instance.m_TESObjectCELL_unk_178_Actor_GetExtraLight_o(a_actor);
	}

	void EngineExtensions::PlayerCharacter_unk_205_RefreshMagicCasterLights_Hook(PlayerCharacter* a_actor, RE::ShadowSceneNode* a_ssn) noexcept
	{
		m_Instance.m_PlayerCharacter_unk_205_RefreshMagicCasterLights_o(a_actor, a_ssn);
		ReferenceLightController::GetSingleton().OnRefreshLightOnSceneMove(a_actor);
	}

	void EngineExtensions::PlayerCharacter_RefreshLights_RefreshMagicCasterLights_Hook(PlayerCharacter* a_actor, RE::ShadowSceneNode* a_ssn) noexcept
	{
		m_Instance.m_PlayerCharacter_RefreshLights_RefreshMagicCasterLights_o(a_actor, a_ssn);
		ReferenceLightController::GetSingleton().OnRefreshLightOnSceneMove(a_actor);
	}

	REFR_LIGHT* EngineExtensions::Actor_Update_Actor_GetExtraLight_Hook(Actor* a_actor) noexcept
	{
		ReferenceLightController::GetSingleton().OnUnkQueueBSLight(a_actor);
		return m_Instance.m_Actor_Update_Actor_GetExtraLight_o(a_actor);
	}

	void EngineExtensions::CreateWeaponNodes_Hook(
		TESObjectREFR* a_actor,
		TESForm*       a_object,
		bool           a_left) noexcept
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
		const BSFixedString& a_name) noexcept
	{
		bool result = false;

		std::uint32_t maxiter = 1000;

		while (NiPointer object = GetObjectByName(a_object, a_name, true))
		{
			object->m_parent->DetachChild2(object);
			result = true;

			if (!--maxiter)
			{
				break;
			}

			ShrinkToSize(a_object);
		}

		return result;
	}

	auto EngineExtensions::AttachObject(
		Actor*       a_actor,
		TESForm*     a_modelForm,
		NiNode*      a_root,
		NiNode*      a_targetNode,
		NiNode*      a_object,
		ModelType    a_modelType,
		bool         a_leftWeapon,
		bool         a_dropOnDeath,
		bool         a_removeScabbards,
		bool         a_keepTorchFlame,
		bool         a_disableHavok,
		bool         a_removeTracers,
		bool         a_attachLight,
		ObjectLight& a_attachedLight) noexcept
		-> stl::flag<AttachResultFlags>
	{
		stl::flag<AttachResultFlags> result{
			AttachResultFlags::kNone
		};

		if (const auto* const bsxFlags = GetBSXFlags(a_object))
		{
			if (ReferenceLightController::GetSingleton().GetEnabled() &&
			    a_attachLight &&
			    a_modelType == ModelType::kLight)
			{
				if (const auto* const light = a_modelForm->As<TESObjectLIGH>())
				{
					a_attachedLight = ReferenceLightController::AttachLight(light, a_actor, a_object);
				}
			}

			const stl::flag<BSXFlags::Flag> flags(bsxFlags->m_data);

			if (flags.test(BSXFlags::Flag::kAddon))
			{
				AttachAddonNodes(a_object);
			}
		}

		AttachAddonParticles(a_object);

		if (auto fadeNode = a_object->AsFadeNode())
		{
			fadeNode->unk153 = (fadeNode->unk153 & 0xF0) | 0x7;
		}

		SetRootOnShaderProperties(a_object, a_root);

		a_targetNode->AttachChild(a_object, true);

		NiAVObject::ControllerUpdateContext ctx{
			static_cast<float>(*m_gameRuntimeMS),
			0
		};

		a_object->UpdateDownwardPass(ctx, 0);

		a_object->IncRef();

		fUnk12ba3e0(*m_shadowSceneNode, a_object);
		fUnk12b99f0(*m_shadowSceneNode, a_object);

		a_object->DecRef();

		const auto sh = BSStringHolder::GetSingleton();

		a_object->m_name = sh->m_object;

		std::uint32_t collisionFilterInfo = 0;

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

				collisionFilterInfo = 0x14;
			}

			break;

		case ModelType::kLight:

			if (!a_keepTorchFlame)
			{
				if (NiPointer torchFireObj = GetObjectByName(a_object, sh->m_torchFire, true))
				{
					if (auto parent = torchFireObj->m_parent)
					{
						parent->DetachChild2(torchFireObj);

						result.set(AttachResultFlags::kTorchFlameRemoved);

						ShrinkToSize(a_object);
					}
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

			collisionFilterInfo = 0x14;

			break;

		case ModelType::kShield:

			collisionFilterInfo = 0x14;

			break;

		case ModelType::kProjectile:

			if (a_removeTracers)
			{
				if (NiPointer object = GetObjectByName(a_object, sh->m_tracerRoot, true))
				{
					if (auto parent = object->m_parent)
					{
						parent->DetachChild2(object);

						ShrinkToSize(a_object);
					}
				}
			}

			break;
		}

		if (a_disableHavok)  // maybe just force this for ammo
		{
			StripCollision(a_object, true, true);
		}

		fUnk1CD130(a_object, collisionFilterInfo);

		QueueAttachHavok(
			BSTaskPool::GetSingleton(),
			a_object,
			a_dropOnDeath ? 4 : 0,
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

		SetRootOnShaderProperties(a_targetNode, a_root);

		a_actor->UpdateAlpha();

		return result;
	}

	bool EngineExtensions::CreateWeaponBehaviorGraph(
		NiAVObject*                               a_object,
		RE::WeaponAnimationGraphManagerHolderPtr& a_out,
		std::function<bool(const char*)>          a_filter)
	{
		const auto sh = BSStringHolder::GetSingleton();

		auto bged = a_object->GetExtraData<BSBehaviorGraphExtraData>(sh->m_bged);
		if (!bged)
		{
			return false;
		}

		if (bged->controlsBaseSkeleton)
		{
			return false;
		}

		if (bged->behaviorGraphFile.empty())
		{
			return false;
		}

		if (!a_filter(bged->behaviorGraphFile.c_str()))
		{
			return false;
		}

		auto result = RE::WeaponAnimationGraphManagerHolder::Create();

		if (!LoadWeaponAnimationBehahaviorGraph(
				*result,
				bged->behaviorGraphFile.c_str()))
		{
			return false;
		}

		if (!BindAnimationObject(*result, a_object))
		{
			CleanupWeaponBehaviorGraph(result);

			gLog.Warning(
				"%s: binding animation object failed [0x%p | %s]",
				__FUNCTION__,
				a_object,
				a_object->m_name.c_str());

			return false;
		}

		a_out = std::move(result);

		return true;
	}

	void EngineExtensions::CleanupWeaponBehaviorGraph(
		RE::WeaponAnimationGraphManagerHolderPtr& a_graph) noexcept
	{
		RE::BSAnimationGraphManagerPtr manager;
		if (a_graph->GetAnimationGraphManagerImpl(manager))
		{
			auto gc = RE::GarbageCollector::GetSingleton();
			assert(gc);
			gc->QueueBehaviorGraph(manager);
		}
	}

	void EngineExtensions::UpdateRoot(NiNode* a_root) noexcept
	{
		a_root->UpdateWorldBound();

		NiAVObject::ControllerUpdateContext ctx{ 0, 0x2000 };
		a_root->Update(ctx);

		fUnk12BAFB0(*m_shadowSceneNode, a_root, false);
	}

	BSXFlags* EngineExtensions::GetBSXFlags(NiObjectNET* a_object) noexcept
	{
		return a_object->GetExtraData<BSXFlags>(BSStringHolder::GetSingleton()->m_bsx);
	}

	static inline constexpr bool should_update(TESObjectREFR* a_refr) noexcept
	{
		if (a_refr->GetMustUpdate())
		{
			return true;
		}

		if (const auto* const door = a_refr->baseForm ? a_refr->baseForm->As<TESObjectDOOR>() : nullptr)
		{
			return door->doorFlags.test(TESObjectDOOR::Flag::kAutomatic);
		}

		return false;
	}

	void EngineExtensions::UpdateReferenceAnimations(
		TESObjectREFR* a_refr,
		float          a_step) noexcept
	{
		struct TLSData
		{
			std::uint8_t  unk000[0x768];  // 000
			std::uint32_t unk768;         // 768
		};

		auto tlsData = reinterpret_cast<TLSData**>(__readgsqword(0x58));

		auto& tlsUnk768 = tlsData[*g_TlsIndexPtr]->unk768;

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
				if (auto& h = e.weaponAnimationGraphManagerHolder)
				{
					UpdateAnimationGraph(h.get(), data);
				}
			}

			if (auto actor = a_refr->As<Actor>())
			{
				m_Instance.ProcessAnimationUpdateList(actor, data, *m_Instance.m_controller);
			}
		}

		tlsUnk768 = oldUnk768;
	}

}