#pragma once

#include "ConfigData.h"

namespace IED
{
	class BSStringHolder
	{
	public:
		static inline constexpr auto NINODE_WEAPON_BACK      = "WeaponBack";
		static inline constexpr auto NINODE_SWORD            = "WeaponSword";
		static inline constexpr auto NINODE_SWORD_LEFT       = "WeaponSwordLeft";
		static inline constexpr auto NINODE_AXE              = "WeaponAxe";
		static inline constexpr auto NINODE_AXE_LEFT         = "WeaponAxeLeft";
		static inline constexpr auto NINODE_AXE_LEFT_REVERSE = "WeaponAxeLeftReverse";
		static inline constexpr auto NINODE_MACE             = "WeaponMace";
		static inline constexpr auto NINODE_MACE_LEFT        = "WeaponMaceLeft";
		static inline constexpr auto NINODE_AXE_MACE_BACK    = "WeaponBackAxeMace";
		static inline constexpr auto NINODE_DAGGER           = "WeaponDagger";
		static inline constexpr auto NINODE_DAGGER_LEFT      = "WeaponDaggerLeft";
		static inline constexpr auto NINODE_STAFF            = "WeaponStaff";
		static inline constexpr auto NINODE_STAFF_LEFT       = "WeaponStaffLeft";
		static inline constexpr auto NINODE_BOW              = "WeaponBow";
		static inline constexpr auto NINODE_CROSSBOW         = "WeaponCrossBow";
		static inline constexpr auto NINODE_SHIELD_BACK      = "ShieldBack";
		static inline constexpr auto NINODE_QUIVER           = "QUIVER";
		static inline constexpr auto NINODE_SHIELD           = "SHIELD";
		static inline constexpr auto NINODE_WEAPON           = "WEAPON";
		static inline constexpr auto NINODE_NPCROOT          = "NPC Root [Root]";
		static inline constexpr auto NINODE_NPCSPINE2        = "NPC Spine2 [Spn2]";
		static inline constexpr auto NINODE_NPCRHAND         = "NPC R Hand [RHnd]";

		static inline constexpr auto NINODE_ARROW_QUIVER = "ArrowQuiver";
		static inline constexpr auto NINODE_ARROW_1      = "Arrow1";
		static inline constexpr auto NINODE_ARROW_2      = "Arrow2";
		static inline constexpr auto NINODE_ARROW_3      = "Arrow3";
		static inline constexpr auto NINODE_ARROW_4      = "Arrow4";
		static inline constexpr auto NINODE_ARROW_5      = "Arrow5";

		static inline constexpr auto NINODE_SCB           = "Scb";
		static inline constexpr auto NINODE_SCB_LEFT      = "ScbLeft";
		static inline constexpr auto BSX                  = "BSX";
		static inline constexpr auto BGED                 = "BGED";
		static inline constexpr auto NINODE_TORCH_FIRE    = "TorchFire";
		static inline constexpr auto NINODE_ATTACH_LIGHT  = "AttachLight";
		static inline constexpr auto NINODE_GLOW_ADD_MESH = "GlowAddMesh";
		static inline constexpr auto NINODE_TRACER_ROOT   = "TracerRoot";

		static inline constexpr auto NINODE_MOV_DEFAULT_SWORD  = "MOV WeaponSwordDefault";
		static inline constexpr auto NINODE_MOV_DEFAULT_AXE    = "MOV WeaponAxeDefault";
		static inline constexpr auto NINODE_MOV_DEFAULT_MACE   = "MOV WeaponMaceDefault";
		static inline constexpr auto NINODE_MOV_DEFAULT_DAGGER = "MOV WeaponDaggerDefault";
		static inline constexpr auto NINODE_MOV_DEFAULT_BOW    = "MOV WeaponBowDefault";
		static inline constexpr auto NINODE_MOV_DEFAULT_BACK   = "MOV WeaponBackDefault";
		static inline constexpr auto NINODE_MOV_DEFAULT_QUIVER = "MOV QUIVERDefault";

		static inline constexpr auto NINODE_CME_DEFAULT_SWORD  = "CME WeaponSwordDefault";
		static inline constexpr auto NINODE_CME_DEFAULT_AXE    = "CME WeaponAxeDefault";
		static inline constexpr auto NINODE_CME_DEFAULT_MACE   = "CME WeaponMaceDefault";
		static inline constexpr auto NINODE_CME_DEFAULT_DAGGER = "CME WeaponDaggerDefault";
		static inline constexpr auto NINODE_CME_DEFAULT_BOW    = "CME WeaponBowDefault";
		static inline constexpr auto NINODE_CME_DEFAULT_BACK   = "CME WeaponBackDefault";
		static inline constexpr auto NINODE_CME_DEFAULT_QUIVER = "CME QUIVERDefault";

		// IED-added

		static inline constexpr auto NINODE_IED_OBJECT  = "OBJECT ROOT";
		static inline constexpr auto NINODE_IED_PHYSICS = "OBJECT PHY";

		// non-vanilla

		static inline constexpr auto NINODE_MX_TORCH_SMOKE          = "MXTorchSmoke01";
		static inline constexpr auto NINODE_MX_TORCH_SPARKS         = "MXTorchSparks02";
		static inline constexpr auto NINODE_MX_ATTACH_SMOKE         = "MXAttachSmoke";
		static inline constexpr auto NINODE_MX_ATTACH_SPARKS        = "MXAttachSparks";
		static inline constexpr auto BSVALUENODE_ATTACH_ENB_LIGHT   = "AttachENBLight";
		static inline constexpr auto NINODE_ENB_FIRE_LIGHT_EMITTER  = "pFireENBLight01-Emitter";
		static inline constexpr auto NINODE_ENB_TORCH_LIGHT_EMITTER = "pTorchENBLight01-Emitter";

		// animation

		static inline constexpr auto WEAPON_SHEATHE = "WeaponSheathe";
		static inline constexpr auto GRAPH_DELETING = "GraphDeleting";

		static constinit inline const char* ANIM_EVENTS[] = {
			//"tailUnequip",
			//"tailEquip",
			"BeginWeaponDraw",
			"BeginWeaponSheathe",
			"WeaponSheathe",
			"WeaponDraw",
			"weaponSwing",
			"weaponLeftSwing",
			"BowDraw",
			"BowDrawn",
			"BowDrawStart",
			"BowDrawEnd",
			"BowReset",
			"BowReleaseFast",
			"BowRelease",
			"BowZoomStart",
			"BowZoomStop",
			"BowEnd",
			"ArrowAttach",
			"ArrowDetach",
			"ArrowRelease",
			"AttackStart",
			"AttackStop",
			"Unequip_Out",
			"Unequip_OutMoving",
			"WeapEquip_OutMoving",
			"WeapEquip_Out",
			"AttackWinStart",
			"PowerAttack_Start_end",
			"AttackWinStartLeft",
			"AttackWinEndLeft",
			"AttackWinStart",
			"AttackWinEnd",
			"PowerAttackStop",
		};

		BSStringHolder();

		BSStringHolder(const BSStringHolder&)            = delete;
		BSStringHolder(BSStringHolder&&)                 = delete;
		BSStringHolder& operator=(const BSStringHolder&) = delete;
		BSStringHolder& operator=(BSStringHolder&&)      = delete;

		static void Create();

		inline static constexpr const auto* GetSingleton() noexcept
		{
			return m_Instance.get();
		}

		inline constexpr auto& GetSheathNodes() const noexcept
		{
			return m_sheathNodes;
		}

		bool IsVanillaSheathNode(const BSFixedString& a_name) const;

		BSFixedString m_npcroot{ NINODE_NPCROOT };
		//BSFixedString m_npcSpine2{ NINODE_NPCSPINE2 };
		//BSFixedString m_npcRhand{ NINODE_NPCRHAND };
		BSFixedString m_scb{ NINODE_SCB };
		BSFixedString m_scbLeft{ NINODE_SCB_LEFT };
		BSFixedString m_bsx{ BSX };
		BSFixedString m_bged{ BGED };
		BSFixedString m_torchFire{ NINODE_TORCH_FIRE };
		BSFixedString m_object{ NINODE_IED_OBJECT };
		BSFixedString m_objectPhy{ NINODE_IED_PHYSICS };
		BSFixedString m_attachLight{ NINODE_ATTACH_LIGHT };
		BSFixedString m_glowAddMesh{ NINODE_GLOW_ADD_MESH };
		BSFixedString m_tracerRoot{ NINODE_TRACER_ROOT };

		BSFixedString m_weaponAxe{ NINODE_AXE };
		BSFixedString m_weaponMace{ NINODE_MACE };
		BSFixedString m_weaponSword{ NINODE_SWORD };
		BSFixedString m_weaponDagger{ NINODE_DAGGER };
		BSFixedString m_weaponBack{ NINODE_WEAPON_BACK };
		BSFixedString m_weaponBow{ NINODE_BOW };
		BSFixedString m_quiver{ NINODE_QUIVER };

		BSFixedString m_arrowQuiver{ NINODE_ARROW_QUIVER };

		BSFixedString m_mxTorchSmoke{ NINODE_MX_TORCH_SMOKE };
		BSFixedString m_mxTorchSparks{ NINODE_MX_TORCH_SPARKS };
		BSFixedString m_mxAttachSmoke{ NINODE_MX_ATTACH_SMOKE };
		BSFixedString m_mxAttachSparks{ NINODE_MX_ATTACH_SPARKS };
		BSFixedString m_attachENBLight{ BSVALUENODE_ATTACH_ENB_LIGHT };
		BSFixedString m_enbFireLightEmitter{ NINODE_ENB_FIRE_LIGHT_EMITTER };
		BSFixedString m_enbTorchLightEmitter{ NINODE_ENB_TORCH_LIGHT_EMITTER };

		BSFixedString m_weaponSheathe{ WEAPON_SHEATHE };

		//stl::unordered_set<BSFixedString> m_animEventFilter;

		BSFixedString m_npcNodeName{ "NPC" };
		BSFixedString m_XPMSE{ "XPMSE" };
		BSFixedString m_skeletonID{ "SkeletonID" };
		BSFixedString m_rigPerspective{ "rigPerspective" };
		BSFixedString m_rigVersion{ "rigVersion" };
		BSFixedString m_species{ "species" };
		BSFixedString m_BSBoneLOD{ "BSBoneLOD" };
		BSFixedString m_BBX{ "BBX" };

		struct SheathNodeEntry
		{
			BSFixedString name;
			BSFixedString mov;
			BSFixedString cme;
		};

		std::array<SheathNodeEntry, 7> m_sheathNodes;
		std::array<BSFixedString, 5>   m_arrows;

		static std::unique_ptr<BSStringHolder> m_Instance;
	};

	class StringHolder
	{
	public:
		static inline constexpr auto FMT_NINODE_IED_GROUP = "OBJECT GROUP [%.8X]";
		static inline constexpr auto HK_NPC_ROOT          = "NPC Root [Root]";

		using slot_names_array_t = std::array<
			stl::fixed_string,
			stl::underlying(Data::ObjectSlot::kMax)>;

		[[nodiscard]] inline static constexpr const auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		StringHolder(const StringHolder&)            = delete;
		StringHolder(StringHolder&&)                 = delete;
		StringHolder& operator=(const StringHolder&) = delete;
		StringHolder& operator=(StringHolder&&)      = delete;

		[[nodiscard]] inline constexpr const auto& GetSlotName(Data::ObjectSlot a_slot) const noexcept
		{
			assert(a_slot < Data::ObjectSlot::kMax);
			return slotNames[stl::underlying(a_slot)];
		}

		stl::fixed_string save{ stl::fixed_string::make_tuple("Save") };
		stl::fixed_string snew{ stl::fixed_string::make_tuple("New") };
		stl::fixed_string apply{ stl::fixed_string::make_tuple("Apply") };
		stl::fixed_string merge{ stl::fixed_string::make_tuple("Merge") };
		stl::fixed_string IED{ stl::fixed_string::make_tuple("IED") };
		stl::fixed_string weaponSheathe{ stl::fixed_string::make_tuple("WeaponSheathe") };

		slot_names_array_t slotNames;

		stl::container_init_wrapper<std::unordered_set<stl::fixed_string>> papyrusRestrictedPlugins;

	private:
		StringHolder();

		static StringHolder m_Instance;
	};

}