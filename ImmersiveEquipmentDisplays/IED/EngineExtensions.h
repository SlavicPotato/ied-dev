#pragma once

#include "AnimationUpdateManager.h"
#include "ConfigINI.h"
#include "Controller/ModelType.h"

#include <ext/GameCommon.h>
#include <ext/ITasks.h>

#include <ext/BSAnimationGraphManager.h>
#include <ext/WeaponAnimationGraphManagerHolder.h>
#include <ext/hkaSkeleton.h>

#include <skse64/NiExtraData.h>

namespace IED
{
	class Controller;

	enum class AttachResultFlags : std::uint8_t
	{
		kNone = 0,

		kScbLeft            = 1ui8 << 0,
		kTorchFlameRemoved  = 1ui8 << 1,
		kTorchCustomRemoved = 1ui8 << 2,
	};

	DEFINE_ENUM_CLASS_BITWISE(AttachResultFlags);

	class EngineExtensions :
		AnimationUpdateManager,
		ILog
	{
		struct unks_01
		{
			std::uint16_t p1;
			std::uint16_t p2;
		};

		struct hkaGetSkeletonNodeResult
		{
			NiNode*       root;   // 00
			std::uint32_t unk08;  // 08 - index?
			std::uint32_t pad1C;  // 1C
		};

		static_assert(sizeof(hkaGetSkeletonNodeResult) == 0x10);

		class ShadowSceneNode;

		typedef BSFadeNode* (*GetNearestFadeNodeParent_t)(NiAVObject* a_object);
		typedef NiNode* (*unk1291cc0_t)(NiAVObject* a_object, NiNode* a_node);
		typedef NiNode* (*unkSSN1_t)(ShadowSceneNode* a_node, NiAVObject* a_object);
		typedef void (*unkDC6140_t)(NiNode* a_node, bool a_unk2);
		typedef void (*unk1CDB30_t)(NiAVObject* a_node);
		typedef void (*unk1CD130_t)(NiAVObject* a_object, std::uint32_t a_collisionFilterInfo);
		typedef void (*unk5C3C40_t)(
			BSTaskPool*   a_taskpool,
			NiAVObject*   a_object,
			std::uint32_t a_unk3,
			bool          a_unk4);
		typedef unks_01& (*unk5EBD90_t)(TESObjectREFR* a_ref, unks_01& a_out);
		typedef void (*unk5C39F0_t)(
			BSTaskPool*   a_taskpool,
			NiAVObject*   a_object,
			RE::bhkWorld* a_world,
			std::uint32_t a_unk4);
		typedef void (*attachAddonNodes_t)(NiAVObject* a_object);
		typedef bool (*unk63F810_t)(void);
		typedef void (*cleanupNodeGeometry_t)(
			const Game::ObjectRefHandle& a_handle,
			NiAVObject*                  a_object);

		typedef NiAVObject* (*fGetObjectByName_t)(
			NiNode*              a_root,
			const BSFixedString& a_name,
			bool                 a_unk);
		typedef NiAVObject* (*fUnk1401CDB30_t)(NiNode*);
		typedef NiAVObject* (*fUnk140DC6140_t)(NiAVObject*, bool);
		typedef NiAVObject* (*fUnk1412BAFB0_t)(
			ShadowSceneNode* a_shadowSceneNode,
			NiAVObject*      a_object,
			bool);
		typedef NiAVObject* (*fUnk1412BAFB0_t)(
			ShadowSceneNode* a_shadowSceneNode,
			NiAVObject*      a_object,
			bool);
		typedef NiExtraData* (*fFindNiExtraData_t)(
			NiObjectNET*         a_object,
			const BSFixedString& a_name);
		typedef bool (*unk14028BAD0_t)(NiNode* a_node);
		typedef void (*fUnkC6B900_t)(NiAVObject* a_object, const char* a_str);
		typedef void (*fUnk362E90_t)(TESNPC* a_npc, Actor* a_actor, NiAVObject* a_object);

		typedef void (*applyTextureSwap_t)(TESModelTextureSwap* a_swap, NiAVObject* a_object);

		typedef bool (*hkaLookupSkeletonNode_t)(
			NiNode*                   a_root,
			const BSFixedString&      a_name,
			hkaGetSkeletonNodeResult& a_result);

		typedef bool (*loadAndRegisterWeaponGraph_t)(
			RE::WeaponAnimationGraphManagerHolder& a_weapHolder,
			const char*                            a_hkxPath,
			RE::IAnimationGraphManagerHolder&      a_characterHolder);

		typedef bool (*loadWeaponGraph_t)(
			RE::WeaponAnimationGraphManagerHolder& a_weapHolder,
			const char*                            a_hkxPath);

		typedef bool (*bindAnimationObject_t)(
			RE::IAnimationGraphManagerHolder& a_holder,
			NiAVObject*                       a_object);

		typedef bool (*updateAnimationGraph_t)(
			RE::IAnimationGraphManagerHolder* a_holder,
			const BSAnimationUpdateData&      a_data);

		// typedef void (*playSound_t)(const char* a_editorID);

	public:
		EngineExtensions(const EngineExtensions&) = delete;
		EngineExtensions(EngineExtensions&&)      = delete;
		EngineExtensions& operator=(const EngineExtensions&) = delete;
		EngineExtensions& operator=(EngineExtensions&&) = delete;

		static void Install(
			Controller*                       a_controller,
			const std::shared_ptr<ConfigINI>& a_config);

		static bool RemoveAllChildren(
			NiNode*              a_object,
			const BSFixedString& a_name);

		static stl::flag<AttachResultFlags> AttachObject(
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
			bool      a_disableHavok);

		template <class Tf>
		static bool CreateWeaponBehaviorGraph(
			NiAVObject*                               a_object,
			RE::WeaponAnimationGraphManagerHolderPtr& a_out,
			Tf                                        a_filter);

		static void CleanupWeaponBehaviorGraph(
			RE::WeaponAnimationGraphManagerHolderPtr& a_graph);

		static void UpdateRoot(NiNode* a_root);

		// inline static const auto playSound = IAL::Address<playSound_t>(52054);

		inline static const auto GetObjectByName =
			IAL::Address<fGetObjectByName_t>(74481, 76207);

		inline static const auto ApplyTextureSwap     = IAL::Address<applyTextureSwap_t>(14660, 14837);  // 19baa0
		inline static const auto m_unkglob0           = IAL::Address<std::int32_t*>(523662, 410201);
		inline static const auto SceneRendering       = IAL::Address<unk63F810_t>(38079, 39033);
		inline static const auto CleanupObjectImpl    = IAL::Address<cleanupNodeGeometry_t>(15495, 15660);
		inline static const auto UpdateAnimationGraph = IAL::Address<updateAnimationGraph_t>(32155, 32899);
		inline static const auto tlsIndex             = IAL::Address<std::uint32_t*>(528600, 415542);

		// BSDismemberSkinInstance
		//inline static const auto SetEditorVisible = IAL::Address<fUnkC6B900_t>(69401, 0);

		//inline static const auto fUnk362E90 = IAL::Address<fUnk362E90_t>(24220, 0);

		[[nodiscard]] inline static constexpr bool IsWeaponAdjustDisabled() noexcept
		{
			return m_Instance.m_conf.weaponAdjustDisable;
		}

		[[nodiscard]] inline static constexpr bool ParallelAnimationUpdatesEnabled() noexcept
		{
			return m_Instance.m_conf.parallelAnimationUpdates;
		}

		FN_NAMEPROC("EngineExtensions");

	private:
		EngineExtensions() = default;

		void InstallImpl(
			Controller*                       a_controller,
			const std::shared_ptr<ConfigINI>& a_config);

		inline static const auto m_shadowSceneNode = IAL::Address<ShadowSceneNode**>(513211, 390951);
		//inline static const auto removeHavokFuncPtr = IAL::Address<void**>(512244, 389072);

		inline static const auto GetNearestFadeNode        = IAL::Address<GetNearestFadeNodeParent_t>(98861, 105503);
		inline static const auto SetRootOnShaderProperties = IAL::Address<unk1291cc0_t>(98895, 105542);
		inline static const auto fUnk12ba3e0               = IAL::Address<unkSSN1_t>(99702, 106336);
		inline static const auto fUnk12b99f0               = IAL::Address<unkSSN1_t>(99696, 106330);
		inline static const auto fUnk1CD130                = IAL::Address<unk1CD130_t>(15567, 15745);
		inline static const auto QueueAttachHavok          = IAL::Address<unk5C3C40_t>(35950, 36925);
		inline static const auto fUnk5EBD90                = IAL::Address<unk5EBD90_t>(36559, 37560);
		inline static const auto fUnk5C39F0                = IAL::Address<unk5C39F0_t>(35947, 36922);
		inline static const auto AttachAddonNodes          = IAL::Address<attachAddonNodes_t>(19207, 19633);
		inline static const auto ShrinkToSize              = IAL::Address<fUnk1401CDB30_t>(15571, 15748);
		inline static const auto fUnkDC6140                = IAL::Address<fUnk140DC6140_t>(76545, 78389);
		inline static const auto fUnk12BAFB0               = IAL::Address<fUnk1412BAFB0_t>(99712, 106349);
		inline static const auto fUnk28BAD0                = IAL::Address<unk14028BAD0_t>(19206, 19632);

		//inline static const auto m_unkDC6140 = IAL::Address<unkDC6140_t>(76545);
		//inline static const auto m_unk1CDB30 = IAL::Address<unk1CDB30_t>(15571);
		//inline static const auto FindNiExtraData = IAL::Address<fFindNiExtraData_t>(69149, 70510);

		//inline static const auto fLoadAndRegisterWeaponGraph = IAL::Address<loadAndRegisterWeaponGraph_t>(32249, 32984);
		inline static const auto LoadWeaponAnimationBehahaviorGraph = IAL::Address<loadWeaponGraph_t>(32148, 32892);
		inline static const auto BindAnimationObject                = IAL::Address<bindAnimationObject_t>(32250, 32985);

		//inline static const auto hkaGetSkeletonNode = IAL::Address<hkaLookupSkeletonNode_t>(69352, 70732);

		static BSXFlags* GetBSXFlags(NiObjectNET* a_object);

		void Install_RemoveAllBipedParts();
		void Hook_REFR_GarbageCollector();
		void Hook_Actor_Resurrect();
		void Hook_Actor_3DEvents();
		void Hook_Armor_Update();
		void Install_SetWeapAdjAnimVar();
		void Install_CreateWeaponNodes();
		void Install_WeaponAdjustDisable();
		void Hook_ToggleFav();
		void Hook_ProcessEffectShaders();
		void Install_ParallelAnimationUpdate();

		void FailsafeCleanupAndEval(
			Actor*                     a_actor,
			const std::source_location a_loc = std::source_location::current());

		static void                              RemoveAllBipedParts_Hook(Biped* a_biped);
		static void                              Character_Resurrect_Hook(Character* a_actor, bool a_resetInventory, bool a_attach3D);
		static void                              Actor_Release3D_Hook(Actor* a_actor);
		static void                              Character_Release3D_Hook(Character* a_actor);
		static void                              ReanimateActorStateUpdate_Hook(Actor* a_actor, bool a_unk1);
		static void                              CreateWeaponNodes_Hook(TESObjectREFR* a_actor, TESForm* a_object, bool a_left);
		static void                              ArmorUpdate_Hook(Game::InventoryChanges* a_ic, Game::InitWornVisitor& a_visitor);
		static bool                              GarbageCollectorReference_Hook(TESObjectREFR* a_refr);
		static bool                              SetWeapAdjAnimVar_Hook(TESObjectREFR* a_refr, const BSFixedString& a_animVarName, float a_val, Biped* a_biped);
		static BaseExtraList*                    ToggleFavGetExtraList_Hook(TESObjectREFR* a_actor);  // always player
		static void                              ProcessEffectShaders_Hook(Game::ProcessLists* a_pl, float a_frameTimerSlow);
		static bool                              hkaLookupSkeletonNode_Hook(NiNode* a_root, const BSFixedString& a_name, hkaGetSkeletonNodeResult& a_result, const RE::hkaSkeleton& a_hkaSkeleton);
		static void                              PrepareAnimUpdateLists_Hook(Game::ProcessLists* a_pl, void* a_unk);
		static void                              ClearAnimUpdateLists_Hook(std::uint32_t a_unk);
		static const RE::BSTSmartPointer<Biped>& UpdatePlayerAnim_Hook(TESObjectREFR* a_player, const BSAnimationUpdateData& a_data);  // getbiped1

		static void UpdateReferenceAnimations(TESObjectREFR* a_refr, float a_step);

		inline static const auto m_vtblCharacter_a          = IAL::Address<std::uintptr_t>(261397, 207886);
		inline static const auto m_vtblActor_a              = IAL::Address<std::uintptr_t>(260538, 207511);
		inline static const auto m_createWeaponNodes_a      = IAL::Address<std::uintptr_t>(19342, 19769);
		inline static const auto m_removeAllBipedParts_a    = IAL::Address<std::uintptr_t>(15494, 15659);  //, 0x30, 0xA8);
		inline static const auto m_reanimActorStateUpdate_a = IAL::Address<std::uintptr_t>(37865, 38820, 0x3F, 0x3F);
		inline static const auto m_armorUpdate_a            = IAL::Address<std::uintptr_t>(24231, 24725, 0x81, 0x1EF);
		inline static const auto m_garbageCollectorREFR_a   = IAL::Address<std::uintptr_t>(35492, 36459, 0x75, 0x7A);
		inline static const auto m_weapAdj_a                = IAL::Address<std::uintptr_t>(15501, 15678, 0xEF9, 0x427);
		inline static const auto m_adjustSkip_a             = IAL::Address<std::uintptr_t>(62933, 63856);
		inline static const auto m_toggleFav1_a             = IAL::Address<std::uintptr_t>(50990, 51848, 0x4E, 0x71B);
		inline static const auto m_processEffectShaders_a   = IAL::Address<std::uintptr_t>(35565, 36564, 0x53C, 0x8E6);
		inline static const auto m_bipedAttachHavok_a       = IAL::Address<std::uintptr_t>(15569, 15746, 0x556, 0x56B);
		inline static const auto m_hkaLookupSkeletonBones_a = IAL::Address<std::uintptr_t>(62931, 63854, 0x89, 0x108);
		inline static const auto m_animUpdateDispatcher_a   = IAL::Address<std::uintptr_t>(38098, 39054);
		inline static const auto m_animUpdateRef_a          = IAL::Address<std::uintptr_t>(40436, 41453);
		inline static const auto m_animUpdatePlayer_a       = IAL::Address<std::uintptr_t>(39445, 40521);

		//inline static const auto m_updateRefAnim_func = IAL::Address<std::uintptr_t>(19729, 20123);

		decltype(&Character_Resurrect_Hook)       m_characterResurrect_o{ nullptr };
		decltype(&Character_Release3D_Hook)       m_characterRelease3D_o{ nullptr };
		decltype(&Actor_Release3D_Hook)           m_actorRelease3D_o{ nullptr };
		decltype(&ReanimateActorStateUpdate_Hook) m_ReanimActorStateUpd_o{ nullptr };
		decltype(&ArmorUpdate_Hook)               m_ArmorChange_o{ nullptr };
		decltype(&GarbageCollectorReference_Hook) m_garbageCollectorReference_o{ nullptr };
		decltype(&CreateWeaponNodes_Hook)         m_createWeaponNodes_o{ nullptr };
		decltype(&RemoveAllBipedParts_Hook)       m_removeAllBipedParts_o{ nullptr };
		decltype(&ToggleFavGetExtraList_Hook)     m_toggleFavGetExtraList_o{ nullptr };
		decltype(&ProcessEffectShaders_Hook)      m_processEffectShaders_o{ nullptr };
		decltype(&PrepareAnimUpdateLists_Hook)    m_prepareAnimUpdateLists_o{ nullptr };
		decltype(&ClearAnimUpdateLists_Hook)      m_clearAnimUpdateLists_o{ nullptr };
		hkaLookupSkeletonNode_t                   m_hkaLookupSkeletonNode_o{ nullptr };

		struct
		{
			bool weaponAdjustDisable{ false };
			bool weaponAdjustFix{ false };
			bool nodeOverridePlayerEnabled{ false };
			bool disableNPCProcessing{ false };
			bool parallelAnimationUpdates{ false };
		} m_conf;

		Controller* m_controller{ nullptr };

		static EngineExtensions m_Instance;
	};

	template <class Tf>
	bool EngineExtensions::CreateWeaponBehaviorGraph(
		NiAVObject*                               a_object,
		RE::WeaponAnimationGraphManagerHolderPtr& a_out,
		Tf                                        a_filter)
	{
		auto sh = BSStringHolder::GetSingleton();

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

		a_out = std::move(result);

		if (!BindAnimationObject(*a_out, a_object))
		{
			return false;
		}

		return true;
	}

}