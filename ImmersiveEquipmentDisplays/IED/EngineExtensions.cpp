#include "pch.h"

#include "EngineExtensions.h"

#include "Controller/Controller.h"

#include "Util/Logging.h"

#include "AnimationUpdateController.h"
#include "ReferenceLightController.h"
#include "SkeletonCache.h"
#include "SkeletonExtensions.h"
#include "StringHolder.h"

#include <ext/IHook.h>
#include <ext/JITASM.h>
#include <ext/MemoryValidation.h>
#include <ext/Node.h>
#include <ext/VFT.h>

#define UNWRAP(...) __VA_ARGS__
#define VALIDATE_MEMORY(addr, bytes_se, bytes_ae) \
	stl::validate_memory_with_report_and_fail(addr, UNWRAP bytes_se, UNWRAP bytes_ae, PLUGIN_NAME_FULL)

namespace IED
{
	EngineExtensions EngineExtensions::m_Instance;

	void EngineExtensions::Install(
		Controller*                      a_controller,
		const stl::smart_ptr<ConfigINI>& a_config)
	{
		m_Instance.InstallImpl(a_controller, a_config);
	}

	void EngineExtensions::InstallImpl(
		Controller*                      a_controller,
		const stl::smart_ptr<ConfigINI>& a_config)
	{
		m_controller                   = a_controller;
		m_conf.applyTransformOverrides = a_config->m_applyTransformOverrides;
		m_conf.early3DLoadHooks        = a_config->m_enableEarlyLoadHooks;

		Install_RemoveAllBipedParts();
		Install_REFR_GarbageCollector();
		Install_Actor_Resurrect();
		Install_Actor_3DEvents();
		Install_Actor_ActorValueOwner();

		if (a_config->m_nodeOverrideEnabled)
		{
			m_conf.weaponAdjustDisable       = a_config->m_weaponAdjustDisable;
			m_conf.weaponAdjustDisableForce  = a_config->m_weaponAdjustForceDisable;
			m_conf.nodeOverridePlayerEnabled = a_config->m_nodeOverridePlayerEnabled;

			Install_Armor_Update();
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

		if (a_config->m_weaponAdjustDisable)
		{
			Install_WeaponAdjustDisable();
		}

		if (a_config->m_immediateFavUpdate)
		{
			Install_ToggleFav();
		}

		if (a_config->m_behaviorGraphAnims)
		{
			Install_UpdateReferenceBehaviorGraphs();
		}

		if (a_config->m_enableLights)
		{
			Install_Lighting();
		}

		if (a_config->m_effectShaderFlickerFix)
		{
			Install_EffectShaderPostResume();
		}

		if (a_config->m_clearRPCOnSceneMove)
		{
			Install_Player_OnMoveScene();
		}

		//Install_SetupEventSinks();
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

	void EngineExtensions::Install_REFR_GarbageCollector()
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

	void EngineExtensions::Install_Actor_Resurrect()
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

	void EngineExtensions::Install_Actor_3DEvents()
	{
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
		VALIDATE_MEMORY(
			m_refrLoad3DClone_a.get(),
			({ 0xFF, 0x90, 0x50, 0x02, 0x00, 0x00 }),
			({ 0xFF, 0x90, 0x50, 0x02, 0x00, 0x00 }));

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

	void EngineExtensions::Install_Armor_Update()
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

	void EngineExtensions::Install_ToggleFav()
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

	void EngineExtensions::Install_UpdateReferenceBehaviorGraphs()
	{
		const auto addrRefUpdate    = m_animUpdateRef_a.get() + 0xAB;
		const auto addrPlayerUpdate = m_animUpdatePlayer_a.get() + 0xD0;

		VALIDATE_MEMORY(
			addrRefUpdate,
			({ 0xFF, 0x90, 0xF8, 0x03, 0x00, 0x00 }),
			({ 0xFF, 0x90, 0xF8, 0x03, 0x00, 0x00 }));

		VALIDATE_MEMORY(
			addrPlayerUpdate,
			({ 0xFF, 0x90, 0xF0, 0x03, 0x00, 0x00 }),
			({ 0xFF, 0x90, 0xF0, 0x03, 0x00, 0x00 }));

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
				reinterpret_cast<std::uintptr_t>(UpdateRefAnim_Hook));

			const auto dst = code.get();

			ISKSE::GetBranchTrampoline().Write6Call(
				addrRefUpdate,
				dst);

			ISKSE::GetBranchTrampoline().Write6Call(
				addrPlayerUpdate,
				dst);
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

		addr = m_Actor_Update_Actor_GetExtraLight_a.get() + (IAL::IsAE() ? 0x60B : 0x66B);

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

		InstallVtableDetour(
			m_vtblPlayerCharacter_a,
			0x55,
			PlayerCharacter_UpdateRefLight_Hook,
			m_pcUpdateRefLightPlayerCharacter_o,
			true,
			"PlayerCharacter::UpdateRefLight");
	}

	void EngineExtensions::Install_EffectShaderPostResume()
	{
		const auto addr = m_ShaderReferenceEffect_Resume_a.get() + 0x84;

		VALIDATE_MEMORY(
			addr,
			({ 0xFF, 0x90, 0xE0, 0x01, 0x00, 0x00 }),
			({ 0xFF, 0x90, 0xE0, 0x01, 0x00, 0x00 }));

		LogPatchBegin();
		{
			struct Assembly : JITASM::JITASM
			{
				Assembly(std::uintptr_t a_callAddr) :
					JITASM(ISKSE::GetLocalTrampoline())
				{
					Xbyak::Label callLabel;

					mov(rdx, rbx);
					jmp(ptr[rip + callLabel]);

					L(callLabel);
					dq(a_callAddr);
				}
			};

			Assembly code(
				reinterpret_cast<std::uintptr_t>(ShaderReferenceEffect_Resume_GetAttachRoot));

			ISKSE::GetBranchTrampoline().Write6Call(
				addr,
				code.get());
		}
		LogPatchEnd();
	}

	/*void EngineExtensions::Install_SetupEventSinks()
	{
		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				m_setupEventSinks_a.get(),
				std::uintptr_t(SetupEventSinks_Hook),
				m_SetupEventSinks_o))
		{
			Debug(
				"[%s] Installed @0x%llX",
				__FUNCTION__,
				m_setupEventSinks_a.get());
		}
		else
		{
			HALT(__FUNCTION__ ": failed");
		}
	}*/

	template <class T>
	inline void EngineExtensions::AVThunk<T>::Install(
		const IAL::Address<std::uintptr_t>& a_vtblAddr)
	{
		GetSingleton().InstallVtableDetour(
			a_vtblAddr,
			0x4,
			SetBaseActorValue_Hook,
			_SetBaseActorValue_o,
			true,
			"ActorValueOwner::SetBaseActorValue");

		GetSingleton().InstallVtableDetour(
			a_vtblAddr,
			0x5,
			ModActorValue_Hook,
			_ModActorValue_o,
			true,
			"ActorValueOwner::ModActorValue");

		GetSingleton().InstallVtableDetour(
			a_vtblAddr,
			0x6,
			RestoreActorValue_Hook,
			_RestoreActorValue_o,
			true,
			"ActorValueOwner::RestoreActorValue");
	}

	template <class T>
	inline void EngineExtensions::AVThunk<T>::OnFuncCall(
		T*             a_actor,
		RE::ActorValue a_akValue) noexcept
	{
		if (IsValidAV(a_akValue))
		{
			GetSingleton().m_controller->QueueRequestEvaluateLF(a_actor->formID);
		}
	}

	template <class T>
	inline void EngineExtensions::AVThunk<T>::SetBaseActorValue_Hook(
		ActorValueOwner* a_this,
		RE::ActorValue   a_akValue,
		float            a_value) noexcept
	{
		_SetBaseActorValue_o(a_this, a_akValue, a_value);

		OnFuncCall(static_cast<T*>(a_this), a_akValue);
	}

	template <class T>
	inline void EngineExtensions::AVThunk<T>::ModActorValue_Hook(
		ActorValueOwner* a_this,
		RE::ActorValue   a_akValue,
		float            a_value) noexcept
	{
		_ModActorValue_o(a_this, a_akValue, a_value);

		OnFuncCall(static_cast<T*>(a_this), a_akValue);
	}

	template <class T>
	inline void EngineExtensions::AVThunk<T>::RestoreActorValue_Hook(
		ActorValueOwner*         a_this,
		RE::ACTOR_VALUE_MODIFIER a_modifier,
		RE::ActorValue           a_akValue,
		float                    a_value) noexcept
	{
		_RestoreActorValue_o(a_this, a_modifier, a_akValue, a_value);

		OnFuncCall(static_cast<T*>(a_this), a_akValue);
	}

	void EngineExtensions::Install_Actor_ActorValueOwner()
	{
		AVThunk<Actor>::Install(m_vtblActor_ActorValueOwner);
		AVThunk<Character>::Install(m_vtblCharacter_ActorValueOwner);
		AVThunk<PlayerCharacter>::Install(m_vtblPlayerCharacter_ActorValueOwner);
	}

	void EngineExtensions::Install_Player_OnMoveScene()
	{
		const auto addr = m_PlayerCharacter_unk_205_a.get() + 0x7A0;

		if (hook::call5(
				ISKSE::GetBranchTrampoline(),
				addr,
				std::uintptr_t(PlayerCharacter_unk_205_Post_Hook),
				m_PlayerCharacter_unk_205_Post_o))
		{
			Debug(
				"[%s] Installed @0x%llX",
				__FUNCTION__,
				addr);
		}
		else
		{
			Warning(__FUNCTION__ ": failed to install hook");
		}
	}

	void EngineExtensions::RemoveAllBipedParts_Hook(Biped* a_biped)
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
		bool       a_attach3D)
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
	void EngineExtensions::RunRelease3DHook(T* a_actor, void (*&a_origCall)(T*))
	{
		bool eval = false;

		if (a_actor->formID)
		{
			//m_Instance.Debug("%s: release 3D: %X", __FUNCTION__, a_actor->formID);

#if defined(IED_PERF_BUILD)
			ASSERT(!ITaskPool::IsRunningOnCurrentThread());  // REMOVE ME!!
#endif

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
		PlayerCharacter* a_actor)
	{
		RunRelease3DHook(a_actor, m_Instance.m_pcRelease3D_o);
	}

	void EngineExtensions::Actor_Release3D_Hook(
		Actor* a_actor)
	{
		RunRelease3DHook(a_actor, m_Instance.m_actorRelease3D_o);
	}

	void EngineExtensions::Character_Release3D_Hook(
		Character* a_actor)
	{
		RunRelease3DHook(a_actor, m_Instance.m_characterRelease3D_o);
	}

	NiAVObject* EngineExtensions::REFR_Load3D_Clone_Hook(
		TESBoundObject* a_obj,
		TESObjectREFR*  a_refr)
	{
		auto result = a_obj->Clone3D2(a_refr);

		if (result)
		{
			if (auto actor = a_refr->As<Actor>())
			{
				SkeletonExtensions::PostLoad3D(result, GetTransformOverridesEnabled());
				SkeletonCache::GetSingleton().OnLoad3D(actor, result, false);
			}
		}

		return result;
	}

	std::uint32_t EngineExtensions::PlayerCharacter_Load3D_LoadSkeleton_Hook(
		const char*            a_path,
		NiPointer<NiAVObject>& a_3D,
		std::uint32_t&         a_unk3)
	{
		auto result = m_Instance.m_playerLoad3DSkel_o(a_path, a_3D, a_unk3);

		if (a_3D)
		{
			SkeletonExtensions::PostLoad3D(a_3D, GetTransformOverridesEnabled());
			SkeletonCache::GetSingleton().OnLoad3D(*g_thePlayer, a_3D, true);
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
		bool   a_unk1)
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
		Game::InitWornVisitor&  a_visitor)
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

	bool EngineExtensions::GarbageCollectorReference_Hook(TESObjectREFR* a_refr)
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
		Biped*               a_biped)
	{
		auto& biped3p = a_refr->GetBiped1(false);
		if (biped3p && biped3p.get() != a_biped)
		{
			return false;
		}

		return a_refr->SetVariableOnGraphsFloat(a_animVarName, a_val);
	}

	BaseExtraList* EngineExtensions::ToggleFavGetExtraList_Hook(TESObjectREFR* a_actor)
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
		    ::_stricmp(a_hkaSkeleton.name, StringHolder::HK_NPC_ROOT) != 0)
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

		if (::_strnicmp(parent1->m_name.data(), "MOV ", 4) != 0)
		{
			return false;
		}

		auto parent2 = parent1->m_parent;
		if (!parent2)
		{
			return false;
		}

		if (::_strnicmp(parent2->m_name.data(), "CME ", 4) != 0)
		{
			return false;
		}

		return true;
	}

	bool EngineExtensions::hkaLookupSkeletonNode_Hook(
		NiNode*                   a_root,
		const BSFixedString&      a_name,
		hkaGetSkeletonNodeResult& a_result,
		const RE::hkaSkeleton&    a_hkaSkeleton)
	{
		if (hkaShouldBlockNode(a_root, a_name, a_hkaSkeleton))
		{
			a_result.root  = nullptr;
			a_result.index = std::numeric_limits<std::uint32_t>::max();

			return false;
		}

		return m_Instance.m_hkaLookupSkeletonNode_o(a_root, a_name, a_result);
	}

	const RE::BSTSmartPointer<Biped>& IED::EngineExtensions::UpdateRefAnim_Hook(
		TESObjectREFR*               a_refr,
		const BSAnimationUpdateData& a_data)
	{
		auto& bip = a_refr->GetBiped1(false);

		if (bip)  // skip if no biped data
		{
			if (auto actor = a_refr->As<Actor>())
			{
				AnimationUpdateController::GetSingleton().OnUpdate(
					actor,
					a_data);
			}
		}

		return bip;
	}

	void EngineExtensions::PlayerCharacter_UpdateRefLight_Hook(
		PlayerCharacter* a_player)
	{
		m_Instance.m_pcUpdateRefLightPlayerCharacter_o(a_player);
		ReferenceLightController::GetSingleton().OnUpdatePlayerLight(a_player);
	}

	REFR_LIGHT* EngineExtensions::TESObjectCELL_unk_178_Actor_GetExtraLight_Hook(
		Actor* a_actor)
	{
		ReferenceLightController::GetSingleton().OnActorCrossCellBoundary(a_actor);
		return m_Instance.m_TESObjectCELL_unk_178_Actor_GetExtraLight_o(a_actor);
	}

	void EngineExtensions::PlayerCharacter_unk_205_RefreshMagicCasterLights_Hook(
		PlayerCharacter*     a_actor,
		RE::ShadowSceneNode* a_ssn)
	{
		m_Instance.m_PlayerCharacter_unk_205_RefreshMagicCasterLights_o(a_actor, a_ssn);
		ReferenceLightController::GetSingleton().OnRefreshLightOnSceneMove(a_actor);
	}

	void EngineExtensions::PlayerCharacter_RefreshLights_RefreshMagicCasterLights_Hook(
		PlayerCharacter*     a_actor,
		RE::ShadowSceneNode* a_ssn)
	{
		m_Instance.m_PlayerCharacter_RefreshLights_RefreshMagicCasterLights_o(a_actor, a_ssn);
		ReferenceLightController::GetSingleton().OnRefreshLightOnSceneMove(a_actor);
	}

	REFR_LIGHT* EngineExtensions::Actor_Update_Actor_GetExtraLight_Hook(
		Actor* a_actor)
	{
		const auto result = m_Instance.m_Actor_Update_Actor_GetExtraLight_o(a_actor);

		ReferenceLightController::GetSingleton().OnActorUpdate(a_actor, result);

		return result;
	}

	NiAVObject* EngineExtensions::ShaderReferenceEffect_Resume_GetAttachRoot(
		RE::ShaderReferenceEffect* a_this,
		TESObjectREFR*             a_refr)
	{
		auto result = a_this->GetAttachRoot();

		const auto controller = m_Instance.m_controller;

		if (result && controller->ShaderProcessingEnabled())
		{
			if (auto actor = a_refr->As<Actor>())
			{
				controller->QueueSetEffectShaders(actor);
			}
		}

		return result;
	}

	void EngineExtensions::PlayerCharacter_unk_205_Post_Hook(RE::TES* a_tes)
	{
		m_Instance.m_PlayerCharacter_unk_205_Post_o(a_tes);

		const auto controller = m_Instance.m_controller;

		controller->ClearPlayerRPC();
	}

	/*void EngineExtensions::SetupEventSinks_Hook() noexcept
	{
		m_Instance.m_SetupEventSinks_o();
		m_Instance.m_controller->SinkEventsT3();
	}*/

	/*void EngineExtensions::Character_UpdateRefLight_Hook(Character* a_character) noexcept
	{
		_DMESSAGE(".8X", a_character->formID);

		m_Instance.m_updateRefLightPlayerCharacter_o(a_character);
	}*/

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

}