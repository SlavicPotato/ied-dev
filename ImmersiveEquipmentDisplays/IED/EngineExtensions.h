#pragma once

#include "ConfigINI.h"
#include "Controller/ModelType.h"
#include "Controller/ObjectLight.h"

#include "AnimationUpdateController.h"

namespace IED
{
	class Controller;

	class EngineExtensions :
		ILog
	{
		struct hkaGetSkeletonNodeResult
		{
			NiNode*       root;   // 00
			std::uint32_t index;  // 08
			std::uint32_t pad1C;  // 1C
		};
		static_assert(sizeof(hkaGetSkeletonNodeResult) == 0x10);

		typedef BSFadeNode* (*GetNearestFadeNodeParent_t)(NiAVObject* a_object) noexcept;

		typedef bool (*unk63F810_t)() noexcept;
		typedef NiExtraData* (*fFindNiExtraData_t)(
			NiObjectNET*         a_object,
			const BSFixedString& a_name) noexcept;
		typedef void (*fUnkC6B900_t)(NiAVObject* a_object, const char* a_str) noexcept;
		typedef void (*fUnk362E90_t)(TESNPC* a_npc, Actor* a_actor, NiAVObject* a_object) noexcept;

		typedef bool (*hkaLookupSkeletonNode_t)(
			NiNode*                   a_root,
			const BSFixedString&      a_name,
			hkaGetSkeletonNodeResult& a_result) noexcept;

		/*typedef bool (*loadAndRegisterWeaponGraph_t)(
			RE::WeaponAnimationGraphManagerHolder& a_weapHolder,
			const char*                            a_hkxPath,
			RE::IAnimationGraphManagerHolder&      a_characterHolder) noexcept;*/

		// typedef void (*playSound_t)(const char* a_editorID);

	public:
		EngineExtensions(const EngineExtensions&)            = delete;
		EngineExtensions& operator=(const EngineExtensions&) = delete;

		[[nodiscard]] static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		static void Install(
			Controller*                            a_controller,
			const stl::smart_ptr<const ConfigINI>& a_config);

		// inline static const auto playSound = IAL::Address<playSound_t>(52054);

		//inline static const auto tlsIndex             = IAL::Address<std::uint32_t*>(528600, 415542);
		inline static const auto ShouldDefer3DTaskImpl = IAL::Address<unk63F810_t>(38079, 39033);

		//inline static const auto m_skillIncreaseEventSource = IAL::Address<BSTEventSource<SkillIncrease::Event>*>(517608, 404136);

		//inline static const auto GetSkillIncreaseEventSource = IAL::Address<BSTEventSource<SkillIncrease::Event>& (*)()>(39248, 40320);

		// BSDismemberSkinInstance
		//inline static const auto SetEditorVisible = IAL::Address<fUnkC6B900_t>(69401, 0);

		//inline static const auto fUnk362E90 = IAL::Address<fUnk362E90_t>(24220, 0);

		[[nodiscard]] static constexpr bool IsWeaponAdjustDisabled() noexcept
		{
			return m_Instance.m_conf.weaponAdjustDisable;
		}

		[[nodiscard]] static constexpr bool GetTransformOverridesEnabled() noexcept
		{
			return m_Instance.m_conf.applyTransformOverrides;
		}

		[[nodiscard]] static constexpr bool HasEarly3DLoadHooks() noexcept
		{
			return m_Instance.m_conf.early3DLoadHooks;
		}

		[[nodiscard]] inline static bool ShouldDefer3DTask() noexcept
		{
			return
#if !defined(IED_ALLOW_3D_TASKS_OFF_MAIN)
				!ITaskPool::IsRunningOnCurrentThread() ||
#endif
				ShouldDefer3DTaskImpl();
		}

		FN_NAMEPROC("EngineExtensions");

	private:
		EngineExtensions() = default;

		void InstallImpl(
			Controller*                            a_controller,
			const stl::smart_ptr<const ConfigINI>& a_config);

		//inline static const auto removeHavokFuncPtr = IAL::Address<void**>(512244, 389072);

		//inline static const auto GetNearestFadeNode        = IAL::Address<GetNearestFadeNodeParent_t>(98861, 105503);

		//inline static const auto m_unkDC6140 = IAL::Address<unkDC6140_t>(76545);
		//inline static const auto m_unk1CDB30 = IAL::Address<unk1CDB30_t>(15571);
		//inline static const auto FindNiExtraData = IAL::Address<fFindNiExtraData_t>(69149, 70510);

		//inline static const auto fLoadAndRegisterWeaponGraph = IAL::Address<loadAndRegisterWeaponGraph_t>(32249, 32984);

		//inline static const auto hkaGetSkeletonNode = IAL::Address<hkaLookupSkeletonNode_t>(69352, 70732);

		void Install_RemoveAllBipedParts();
		void Install_REFR_GarbageCollector();
		void Install_Actor_Resurrect();
		void Install_Actor_3DEvents();
		void Install_PostLoad3DHooks();
		void Install_Armor_Update();
		void Install_SetWeapAdjAnimVar() const;
		void Install_CreateWeaponNodes();
		void Install_WeaponAdjustDisable();
		void Install_ToggleFav();
		void Install_UpdateReferenceBehaviorGraphs() const;
		void Install_Lighting();
		void Install_EffectShaderPostResume() const;
		void Install_Actor_ActorValueOwner();
		void Install_Player_OnMoveScene();

		template <class T>
		class AVThunk
		{
		public:
			static void Install(const IAL::Address<std::uintptr_t>& a_vtblAddr);

		private:
			static constexpr bool IsValidAV(RE::ActorValue a_akValue) noexcept
			{
				return (a_akValue >= RE::ActorValue::kAggresion &&
				        a_akValue <= RE::ActorValue::kEnchanting) ||
				       a_akValue == RE::ActorValue::kWaitingForPlayer;
			}

			static void OnFuncCall(T* a_actor, RE::ActorValue a_akValue) noexcept;

			static void SetBaseActorValue_Hook(ActorValueOwner* a_this, RE::ActorValue a_akValue, float a_value) noexcept;
			static void ModActorValue_Hook(ActorValueOwner* a_this, RE::ActorValue a_akValue, float a_value) noexcept;
			static void RestoreActorValue_Hook(ActorValueOwner* a_this, RE::ACTOR_VALUE_MODIFIER a_modifier, RE::ActorValue a_akValue, float a_value) noexcept;

			static inline decltype(&SetBaseActorValue_Hook) _SetBaseActorValue_o{ nullptr };
			static inline decltype(&ModActorValue_Hook)     _ModActorValue_o{ nullptr };
			static inline decltype(&RestoreActorValue_Hook) _RestoreActorValue_o{ nullptr };
		};

		template <class T>
		void InstallVtableDetour(
			const IAL::Address<std::uintptr_t>& a_vtblAddr,
			std::ptrdiff_t                      a_offset,
			T*                                  a_hookFunc,
			T*&                                 a_originalFunc,
			bool                                a_critical,
			const char*                         a_desc,
			std::source_location                a_src = std::source_location::current());

		void FailsafeCleanupAndEval(
			Actor*                     a_actor,
			const std::source_location a_loc = std::source_location::current()) noexcept;

		static void                              RemoveAllBipedParts_Hook(Biped* a_biped);
		static void                              Character_Resurrect_Hook(Character* a_actor, bool a_resetInventory, bool a_attach3D);
		static void                              PlayerCharacter_Release3D_Hook(PlayerCharacter* a_actor);
		static void                              Actor_Release3D_Hook(Actor* a_actor);
		static void                              Character_Release3D_Hook(Character* a_actor);
		static NiAVObject*                       REFR_Load3D_Clone_Hook(TESBoundObject* a_obj, TESObjectREFR* a_refr);
		static std::uint32_t                     PlayerCharacter_Load3D_LoadSkeleton_Hook(const char* a_path, NiPointer<NiAVObject>& a_out3D, std::uint32_t& a_unk3);
		static void                              ReanimateActorStateUpdate_Hook(Actor* a_actor, bool a_unk1);
		static void                              CreateWeaponNodes_Hook(TESObjectREFR* a_actor, TESForm* a_object, bool a_left);
		static void                              ArmorUpdate_Hook(Game::InventoryChanges* a_ic, Game::InitWornVisitor& a_visitor);
		static bool                              GarbageCollectorReference_Hook(TESObjectREFR* a_refr);
		static bool                              SetWeapAdjAnimVar_Hook(TESObjectREFR* a_refr, const BSFixedString& a_animVarName, float a_val, Biped* a_biped);
		static BaseExtraList*                    ToggleFavGetExtraList_Hook(TESObjectREFR* a_actor);  // always player
		static bool                              hkaLookupSkeletonNode_Hook(NiNode* a_root, const BSFixedString& a_name, hkaGetSkeletonNodeResult& a_result, const RE::hkaSkeleton& a_hkaSkeleton);
		static const RE::BSTSmartPointer<Biped>& UpdateRefAnim_Hook(TESObjectREFR* a_refr, const BSAnimationUpdateData& a_data);  // getbiped1
		static void                              PlayerCharacter_UpdateRefLight_Hook(PlayerCharacter* a_player);
		static REFR_LIGHT*                       TESObjectCELL_unk_178_Actor_GetExtraLight_Hook(Actor* a_actor);
		static void                              PlayerCharacter_unk_205_RefreshMagicCasterLights_Hook(PlayerCharacter* a_actor, RE::ShadowSceneNode* a_ssn);
		static void                              PlayerCharacter_RefreshLights_RefreshMagicCasterLights_Hook(PlayerCharacter* a_actor, RE::ShadowSceneNode* a_ssn);
		static REFR_LIGHT*                       Actor_Update_Actor_GetExtraLight_Hook(Actor* a_actor);
		static NiAVObject*                       ShaderReferenceEffect_Resume_GetAttachRoot(RE::ShaderReferenceEffect* a_this, TESObjectREFR* a_refr);
		static void                              PlayerCharacter_unk_205_Post_Hook(RE::TES* a_tes);
		//static void                              SetupEventSinks_Hook() noexcept;

		/*static void                                    Character_UpdateRefLight_Hook(Character* a_character) noexcept;
		decltype(&Character_UpdateRefLight_Hook) m_updateRefLightPlayerCharacter_o{ nullptr };*/

		static bool hkaShouldBlockNode(NiAVObject* a_root, const BSFixedString& a_name, const RE::hkaSkeleton& a_hkaSkeleton) noexcept;

		template <class T>
		static void RunRelease3DHook(T* a_actor, void (*&a_origCall)(T*));

		//inline static const auto m_vtblTESObjectREFR_a                = IAL::Address<std::uintptr_t>(235511, 190259);
		inline static const auto m_vtblCharacter_a                    = IAL::Address<std::uintptr_t>(261397, 207886);
		inline static const auto m_vtblActor_a                        = IAL::Address<std::uintptr_t>(260538, 207511);
		inline static const auto m_vtblPlayerCharacter_a              = IAL::Address<std::uintptr_t>(261916, 208040);
		inline static const auto m_refrLoad3DClone_a                  = IAL::Address<std::uintptr_t>(19300, 19727, 0x1D2, 0x1D1);
		inline static const auto m_playerLoad3DSkel_a                 = IAL::Address<std::uintptr_t>(39386, 40458, 0xEE, 0xDD);
		inline static const auto m_createWeaponNodes_a                = IAL::Address<std::uintptr_t>(19342, 19769);
		inline static const auto m_removeAllBipedParts_a              = IAL::Address<std::uintptr_t>(15494, 15659);  //, 0x30, 0xA8);
		inline static const auto m_reanimActorStateUpdate_a           = IAL::Address<std::uintptr_t>(37865, 38820, 0x3F, 0x3F);
		inline static const auto m_armorUpdate_a                      = IAL::Address<std::uintptr_t>(24231, 24725, 0x81, 0x1EF);
		inline static const auto m_garbageCollectorREFR_a             = IAL::Address<std::uintptr_t>(35492, 36459, 0x75, 0x7A);
		inline static const auto m_weapAdj_a                          = IAL::Address<std::uintptr_t>(15501, 15678, 0xEF9, IAL::ver() >= VER_1_6_629 ? 0x424 : 0x427);
		inline static const auto m_adjustSkip_a                       = IAL::Address<std::uintptr_t>(62933, 63856);
		inline static const auto m_toggleFav1_a                       = IAL::Address<std::uintptr_t>(50990, 51848, 0x4E, 0x71B);
		inline static const auto m_hkaLookupSkeletonBones_a           = IAL::Address<std::uintptr_t>(62931, 63854, 0x89, 0x108);
		inline static const auto m_animUpdateDispatcher_a             = IAL::Address<std::uintptr_t>(38098, 39054);
		inline static const auto m_animUpdateRef_a                    = IAL::Address<std::uintptr_t>(19729, 20123);
		inline static const auto m_animUpdatePlayer_a                 = IAL::Address<std::uintptr_t>(39445, 40521);
		inline static const auto m_TESObjectCELL_unk_178_a            = IAL::Address<std::uintptr_t>(19003, 19414);
		inline static const auto m_PlayerCharacter_unk_205_a          = IAL::Address<std::uintptr_t>(39657, 40744);
		inline static const auto m_PlayerCharacter_RefreshLights_a    = IAL::Address<std::uintptr_t>(39493, 40572);
		inline static const auto m_Actor_Update_Actor_GetExtraLight_a = IAL::Address<std::uintptr_t>(36357, 37348);
		inline static const auto m_ShaderReferenceEffect_Resume_a     = IAL::Address<std::uintptr_t>(34114, 34916);
		inline static const auto m_setupEventSinks_a                  = IAL::Address<std::uintptr_t>(35622, 36632, 0x2D7, 0x2D4);
		//inline static const auto m_taskPoolProcExit_a                 = IAL::Address<std::uintptr_t>(35916, 36891, 0x168, 0x2AE);

		inline static const auto m_vtblActor_ActorValueOwner           = IAL::Address<std::uintptr_t>(260543, 207521);
		inline static const auto m_vtblCharacter_ActorValueOwner       = IAL::Address<std::uintptr_t>(261402, 207896);
		inline static const auto m_vtblPlayerCharacter_ActorValueOwner = IAL::Address<std::uintptr_t>(261921, 208050);

		decltype(&Character_Resurrect_Hook)                 m_characterResurrect_o{ nullptr };
		decltype(&PlayerCharacter_Release3D_Hook)           m_pcRelease3D_o{ nullptr };
		decltype(&Character_Release3D_Hook)                 m_characterRelease3D_o{ nullptr };
		decltype(&Actor_Release3D_Hook)                     m_actorRelease3D_o{ nullptr };
		decltype(&PlayerCharacter_Load3D_LoadSkeleton_Hook) m_playerLoad3DSkel_o{ nullptr };
		decltype(&ReanimateActorStateUpdate_Hook)           m_ReanimActorStateUpd_o{ nullptr };
		decltype(&ArmorUpdate_Hook)                         m_ArmorChange_o{ nullptr };
		decltype(&GarbageCollectorReference_Hook)           m_garbageCollectorReference_o{ nullptr };
		decltype(&CreateWeaponNodes_Hook)                   m_createWeaponNodes_o{ nullptr };
		decltype(&RemoveAllBipedParts_Hook)                 m_removeAllBipedParts_o{ nullptr };
		decltype(&ToggleFavGetExtraList_Hook)               m_toggleFavGetExtraList_o{ nullptr };
		/*decltype(&PrepareAnimUpdateLists_Hook)                                 m_prepareAnimUpdateLists_o{ nullptr };
		decltype(&ClearAnimUpdateLists_Hook)                                   m_clearAnimUpdateLists_o{ nullptr };*/
		hkaLookupSkeletonNode_t                                                m_hkaLookupSkeletonNode_o{ nullptr };
		decltype(&PlayerCharacter_UpdateRefLight_Hook)                         m_pcUpdateRefLightPlayerCharacter_o{ nullptr };
		decltype(&TESObjectCELL_unk_178_Actor_GetExtraLight_Hook)              m_TESObjectCELL_unk_178_Actor_GetExtraLight_o{ nullptr };
		decltype(&PlayerCharacter_unk_205_RefreshMagicCasterLights_Hook)       m_PlayerCharacter_unk_205_RefreshMagicCasterLights_o{ nullptr };
		decltype(&PlayerCharacter_RefreshLights_RefreshMagicCasterLights_Hook) m_PlayerCharacter_RefreshLights_RefreshMagicCasterLights_o{ nullptr };
		decltype(&Actor_Update_Actor_GetExtraLight_Hook)                       m_Actor_Update_Actor_GetExtraLight_o{ nullptr };
		decltype(&PlayerCharacter_unk_205_Post_Hook)                           m_PlayerCharacter_unk_205_Post_o{ nullptr };
		//decltype(&SetupEventSinks_Hook)                                        m_SetupEventSinks_o{ nullptr };

		struct
		{
			bool weaponAdjustDisable{ false };
			bool weaponAdjustDisableForce{ false };
			bool nodeOverridePlayerEnabled{ false };
			bool applyTransformOverrides{ false };
			bool early3DLoadHooks{ false };
		} m_conf;

		Controller* m_controller{ nullptr };

		static EngineExtensions m_Instance;
	};

}