#pragma once

#include "Config.h"
#include "Controller/ModelType.h"

#include <ext/GameCommon.h>
#include <ext/ITasks.h>

#include <skse64/NiExtraData.h>

namespace IED
{
	class Controller;

	enum class AttachResultFlags : std::uint8_t
	{
		kNone = 0,

		kScbLeft           = 1ui8 << 0,
		kTorchFlameRemoved = 1ui8 << 1,
	};

	DEFINE_ENUM_CLASS_BITWISE(AttachResultFlags);

	class EngineExtensions :
		ILog
	{
		struct unks_01
		{
			std::uint16_t p1;
			std::uint16_t p2;
		};

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
		typedef unks_01* (*unk5EBD90_t)(TESObjectREFR* a_ref, unks_01* a_in);
		typedef void (*unk5C39F0_t)(
			BSTaskPool*   a_taskpool,
			NiAVObject*   a_object,
			bhkWorld*     a_world,
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

		static void UpdateRoot(NiNode* a_root);
		static void SetDropOnDeath(Actor* a_actor, NiAVObject* a_object, bool a_switch);

		static void CleanupObject(Game::ObjectRefHandle a_handle, NiNode* a_object, NiNode* a_root);

		// inline static const auto playSound = IAL::Address<playSound_t>(52054);

		inline static const auto GetObjectByName =
			IAL::Address<fGetObjectByName_t>(74481, 76207);

		inline static const auto ApplyTextureSwap = IAL::Address<applyTextureSwap_t>(14660, 14837);  // 19baa0
		inline static const auto m_unkglob0       = IAL::Address<std::int32_t*>(523662, 410201);
		inline static const auto SceneRendering   = IAL::Address<unk63F810_t>(38079, 39033);
		inline static const auto CleanupNodeImpl  = IAL::Address<cleanupNodeGeometry_t>(15495, 15660);
		//inline static const auto StrDismemberedLimb = IAL::Address<const char*>(241891, 0);

		// BSDismemberSkinInstance
		//inline static const auto SetEditorVisible = IAL::Address<fUnkC6B900_t>(69401, 0);

		//inline static const auto fUnk362E90 = IAL::Address<fUnk362E90_t>(24220, 0);

		FN_NAMEPROC("EngineExtensions");

	private:
		EngineExtensions() = default;

		void InstallImpl(
			Controller*                       a_controller,
			const std::shared_ptr<ConfigINI>& a_config);

		inline static const auto m_shadowSceneNode = IAL::Address<ShadowSceneNode**>(513211, 390951);

		inline static const auto GetNearestFadeNode        = IAL::Address<GetNearestFadeNodeParent_t>(98861, 105503);
		inline static const auto SetRootOnShaderProperties = IAL::Address<unk1291cc0_t>(98895, 105542);
		inline static const auto fUnk12ba3e0               = IAL::Address<unkSSN1_t>(99702, 106336);
		inline static const auto fUnk12b99f0               = IAL::Address<unkSSN1_t>(99696, 106330);
		inline static const auto fUnk1CD130                = IAL::Address<unk1CD130_t>(15567, 15745);
		inline static const auto fUnk5C3C40                = IAL::Address<unk5C3C40_t>(35950, 36925);
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

		BSXFlags* GetBSXFlags(NiObjectNET* a_object);

		void Patch_RemoveAllBipedParts();
		void Hook_REFR_GarbageCollector();
		void Hook_Actor_Resurrect();
		void Hook_Actor_3DEvents();
		void Hook_Armor_Update();
		void Patch_SetWeapAdjAnimVar();
		void Patch_CreateWeaponNodes();
		void Patch_AdjustSkip_SE();
		void Patch_AdjustSkip_AE();
		void Hook_ToggleFav();
		void Hook_ProcessEffectShaders();

		void FailsafeCleanupAndEval(
			Actor*                     a_actor,
			const std::source_location a_loc = std::source_location::current());

		static void           RemoveAllBipedParts_Hook(Biped* a_biped);
		static void           Character_Resurrect_Hook(Character* a_actor, bool a_resetInventory, bool a_attach3D);
		static void           Actor_Release3D_Hook(Actor* a_actor);
		static void           Character_Release3D_Hook(Character* a_actor);
		static void           ReanimateActorStateUpdate_Hook(Actor* a_actor, bool a_unk1);
		static void           CreateWeaponNodes_Hook(TESObjectREFR* a_actor, TESForm* a_object, bool a_left);
		static void           ArmorUpdate_Hook(Game::InventoryChanges* a_ic, Game::InitWornVisitor& a_visitor);
		static bool           GarbageCollectorReference_Hook(TESObjectREFR* a_refr);
		static bool           SetWeapAdjAnimVar_Hook(TESObjectREFR* a_refr, const BSFixedString& a_animVarName, float a_val, Biped* a_biped);
		static BaseExtraList* ToggleFavGetExtraList_Hook(TESObjectREFR* a_actor);  // always player
		static void           ProcessEffectShaders_Hook(Game::ProcessLists* a_pl, float a_unk1);

		static bool AdjustSkip_Test(const BSFixedString& a_name);

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

		struct
		{
			bool weaponAdjustDisable{ false };
			bool weaponAdjustFix{ false };
			bool nodeOverridePlayerEnabled{ false };
			bool disableNPCProcessing{ false };
		} m_conf;

		Controller* m_controller{ nullptr };

		static EngineExtensions m_Instance;
	};
}  // namespace IED