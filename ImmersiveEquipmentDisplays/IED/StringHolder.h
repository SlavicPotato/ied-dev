#pragma once

#include "ConfigData.h"

namespace IED
{
	class BSStringHolder
	{
	public:
		static constexpr auto NINODE_WEAPON_BACK      = "WeaponBack";
		static constexpr auto NINODE_SWORD            = "WeaponSword";
		static constexpr auto NINODE_SWORD_LEFT       = "WeaponSwordLeft";
		static constexpr auto NINODE_AXE              = "WeaponAxe";
		static constexpr auto NINODE_AXE_LEFT         = "WeaponAxeLeft";
		static constexpr auto NINODE_AXE_LEFT_REVERSE = "WeaponAxeLeftReverse";
		static constexpr auto NINODE_MACE             = "WeaponMace";
		static constexpr auto NINODE_MACE_LEFT        = "WeaponMaceLeft";
		static constexpr auto NINODE_AXE_MACE_BACK    = "WeaponBackAxeMace";
		static constexpr auto NINODE_DAGGER           = "WeaponDagger";
		static constexpr auto NINODE_DAGGER_LEFT      = "WeaponDaggerLeft";
		static constexpr auto NINODE_STAFF            = "WeaponStaff";
		static constexpr auto NINODE_STAFF_LEFT       = "WeaponStaffLeft";
		static constexpr auto NINODE_BOW              = "WeaponBow";
		static constexpr auto NINODE_CROSSBOW         = "WeaponCrossBow";
		static constexpr auto NINODE_SHIELD_BACK      = "ShieldBack";
		static constexpr auto NINODE_QUIVER           = "QUIVER";
		static constexpr auto NINODE_SHIELD           = "SHIELD";
		static constexpr auto NINODE_WEAPON           = "WEAPON";
		static constexpr auto NINODE_NPCROOT          = "NPC Root [Root]";
		static constexpr auto NINODE_NPCSPINE2        = "NPC Spine2 [Spn2]";
		static constexpr auto NINODE_NPCRHAND         = "NPC R Hand [RHnd]";

		static constexpr auto NINODE_ARROW_QUIVER = "ArrowQuiver";

		static constexpr auto NINODE_SCB           = "Scb";
		static constexpr auto NINODE_SCB_LEFT      = "ScbLeft";
		static constexpr auto BSX                  = "BSX";
		static constexpr auto BGED                 = "BGED";
		static constexpr auto NINODE_TORCH_FIRE    = "TorchFire";
		static constexpr auto NINODE_ATTACH_LIGHT  = "AttachLight";
		static constexpr auto NINODE_GLOW_ADD_MESH = "GlowAddMesh";
		static constexpr auto NINODE_TRACER_ROOT   = "TracerRoot";
		static constexpr auto NINODE_EDITOR_MARKER = "EditorMarker";

		static constexpr auto NINODE_MOV_DEFAULT_SWORD  = "MOV WeaponSwordDefault";
		static constexpr auto NINODE_MOV_DEFAULT_AXE    = "MOV WeaponAxeDefault";
		static constexpr auto NINODE_MOV_DEFAULT_MACE   = "MOV WeaponMaceDefault";
		static constexpr auto NINODE_MOV_DEFAULT_DAGGER = "MOV WeaponDaggerDefault";
		static constexpr auto NINODE_MOV_DEFAULT_BOW    = "MOV WeaponBowDefault";
		static constexpr auto NINODE_MOV_DEFAULT_BACK   = "MOV WeaponBackDefault";
		static constexpr auto NINODE_MOV_DEFAULT_QUIVER = "MOV QUIVERDefault";

		static constexpr auto NINODE_CME_DEFAULT_SWORD  = "CME WeaponSwordDefault";
		static constexpr auto NINODE_CME_DEFAULT_AXE    = "CME WeaponAxeDefault";
		static constexpr auto NINODE_CME_DEFAULT_MACE   = "CME WeaponMaceDefault";
		static constexpr auto NINODE_CME_DEFAULT_DAGGER = "CME WeaponDaggerDefault";
		static constexpr auto NINODE_CME_DEFAULT_BOW    = "CME WeaponBowDefault";
		static constexpr auto NINODE_CME_DEFAULT_BACK   = "CME WeaponBackDefault";
		static constexpr auto NINODE_CME_DEFAULT_QUIVER = "CME QUIVERDefault";

		// IED-added

		static constexpr auto NINODE_IED_OBJECT  = "OBJECT ROOT";
		static constexpr auto NINODE_IED_PHYSICS = "OBJECT PHY";

		// non-vanilla

		static constexpr auto NINODE_MX_TORCH_SMOKE          = "MXTorchSmoke01";
		static constexpr auto NINODE_MX_TORCH_SPARKS         = "MXTorchSparks02";
		static constexpr auto NINODE_MX_ATTACH_SMOKE         = "MXAttachSmoke";
		static constexpr auto NINODE_MX_ATTACH_SPARKS        = "MXAttachSparks";
		static constexpr auto BSVALUENODE_ATTACH_ENB_LIGHT   = "AttachENBLight";
		static constexpr auto NINODE_ENB_FIRE_LIGHT_EMITTER  = "pFireENBLight01-Emitter";
		static constexpr auto NINODE_ENB_TORCH_LIGHT_EMITTER = "pTorchENBLight01-Emitter";

		// animation

		static constexpr auto WEAPON_SHEATHE = "WeaponSheathe";
		static constexpr auto GRAPH_DELETING = "GraphDeleting";

		static constexpr std::size_t NUM_DYN_ARROWS = 19;

		BSStringHolder();

		BSStringHolder(const BSStringHolder&)            = delete;
		BSStringHolder(BSStringHolder&&)                 = delete;
		BSStringHolder& operator=(const BSStringHolder&) = delete;
		BSStringHolder& operator=(BSStringHolder&&)      = delete;

		static void Create();

		static constexpr const auto* GetSingleton() noexcept
		{
			return m_Instance.get();
		}

		constexpr auto& GetSheathNodes() const noexcept
		{
			return m_sheathNodes;
		}

		bool IsVanillaSheathNode(const BSFixedString& a_name) const noexcept;

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
		//BSFixedString m_glowAddMesh{ NINODE_GLOW_ADD_MESH };
		BSFixedString m_tracerRoot{ NINODE_TRACER_ROOT };
		BSFixedString m_editorMarker{ NINODE_EDITOR_MARKER };

		BSFixedString m_weaponAxe{ NINODE_AXE };
		BSFixedString m_weaponMace{ NINODE_MACE };
		BSFixedString m_weaponSword{ NINODE_SWORD };
		BSFixedString m_weaponDagger{ NINODE_DAGGER };
		BSFixedString m_weaponBack{ NINODE_WEAPON_BACK };
		BSFixedString m_weaponBow{ NINODE_BOW };
		BSFixedString m_quiver{ NINODE_QUIVER };

		BSFixedString m_arrowQuiver{ NINODE_ARROW_QUIVER };

		std::array<BSFixedString, 7> m_enbLightAttachNodes{
			NINODE_MX_TORCH_SMOKE,
			NINODE_MX_TORCH_SPARKS,
			NINODE_MX_ATTACH_SMOKE,
			NINODE_MX_ATTACH_SPARKS,
			BSVALUENODE_ATTACH_ENB_LIGHT,
			NINODE_ENB_FIRE_LIGHT_EMITTER,
			NINODE_ENB_TORCH_LIGHT_EMITTER,
		};

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

		std::array<SheathNodeEntry, 7>            m_sheathNodes;
		std::array<BSFixedString, NUM_DYN_ARROWS> m_arrows;

		static std::unique_ptr<BSStringHolder> m_Instance;
	};

	class StringHolder
	{
	public:
		static constexpr auto FMT_NINODE_IED_GROUP = "OBJECT GROUP [%.8X]";
		static constexpr auto HK_NPC_ROOT          = "NPC Root [Root]";

		using slot_names_array_t = std::array<
			stl::fixed_string,
			stl::underlying(Data::ObjectSlot::kMax)>;

		[[nodiscard]] static constexpr const auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		StringHolder(const StringHolder&)            = delete;
		StringHolder(StringHolder&&)                 = delete;
		StringHolder& operator=(const StringHolder&) = delete;
		StringHolder& operator=(StringHolder&&)      = delete;

		[[nodiscard]] constexpr const auto& GetSlotName(Data::ObjectSlot a_slot) const noexcept
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

		std::array<stl::fixed_string, 5> papyrusRestrictedPlugins;

	private:
		StringHolder();

		static StringHolder m_Instance;
	};

}