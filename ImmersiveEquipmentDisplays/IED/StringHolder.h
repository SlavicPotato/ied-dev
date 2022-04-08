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

		static inline constexpr auto NINODE_SCB           = "Scb";
		static inline constexpr auto NINODE_SCB_LEFT      = "ScbLeft";
		static inline constexpr auto BSX                  = "BSX";
		static inline constexpr auto NINODE_TORCH_FIRE    = "TorchFire";
		static inline constexpr auto NINODE_ATTACH_LIGHT  = "AttachLight";
		static inline constexpr auto NINODE_GLOW_ADD_MESH = "GlowAddMesh";

		// IED-added

		static inline constexpr auto NINODE_IED_1HSWORD      = "IED WeaponOneHandedSword";
		static inline constexpr auto NINODE_IED_1HSWORD_LEFT = "IED WeaponOneHandedSwordLeft";
		static inline constexpr auto NINODE_IED_1HAXE        = "IED WeaponOneHandedAxe";
		static inline constexpr auto NINODE_IED_1HAXE_LEFT   = "IED WeaponOneHandedAxeLeft";
		static inline constexpr auto NINODE_IED_2HSWORD      = "IED WeaponTwoHandedSword";
		static inline constexpr auto NINODE_IED_2HSWORD_LEFT = "IED WeaponTwoHandedSwordLeft";
		static inline constexpr auto NINODE_IED_2HAXE        = "IED WeaponTwoHandedAxe";
		static inline constexpr auto NINODE_IED_2HAXE_LEFT   = "IED WeaponTwoHandedAxeLeft";
		static inline constexpr auto NINODE_IED_DAGGER       = "IED WeaponDagger";
		static inline constexpr auto NINODE_IED_DAGGER_LEFT  = "IED WeaponDaggerLeft";
		static inline constexpr auto NINODE_IED_MACE         = "IED WeaponMace";
		static inline constexpr auto NINODE_IED_MACE_LEFT    = "IED WeaponMaceLeft";
		static inline constexpr auto NINODE_IED_STAFF        = "IED WeaponStaff";
		static inline constexpr auto NINODE_IED_STAFF_LEFT   = "IED WeaponStaffLeft";
		static inline constexpr auto NINODE_IED_BOW          = "IED WeaponBow";
		static inline constexpr auto NINODE_IED_CROSSBOW     = "IED WeaponCrossbow";
		static inline constexpr auto NINODE_IED_SHIELD       = "IED ArmorShield";
		static inline constexpr auto NINODE_IED_TORCH        = "IED Torch";

		static inline constexpr auto NINODE_IED_OBJECT = "IED OBJECT";

		// non-vanilla

		static inline constexpr auto NINODE_MX_TORCH_SMOKE          = "MXTorchSmoke01";
		static inline constexpr auto NINODE_MX_TORCH_SPARKS         = "MXTorchSparks02";
		static inline constexpr auto NINODE_MX_ATTACH_SMOKE         = "MXAttachSmoke";
		static inline constexpr auto NINODE_MX_ATTACH_SPARKS        = "MXAttachSparks";
		static inline constexpr auto BSVALUENODE_ATTACH_ENB_LIGHT   = "AttachENBLight";
		static inline constexpr auto NINODE_ENB_FIRE_LIGHT_EMITTER  = "pFireENBLight01-Emitter";
		static inline constexpr auto NINODE_ENB_TORCH_LIGHT_EMITTER = "pTorchENBLight01-Emitter";

		BSStringHolder() = default;

		BSStringHolder(const BSStringHolder&) = delete;
		BSStringHolder(BSStringHolder&&)      = delete;
		BSStringHolder& operator=(const BSStringHolder&) = delete;
		BSStringHolder& operator=(BSStringHolder&&) = delete;

		/*inline constexpr auto& GetSheathNodes() const noexcept
		{
			return m_sheathNodes;
		}*/

		BSFixedString m_npcroot{ NINODE_NPCROOT };
		BSFixedString m_scb{ NINODE_SCB };
		BSFixedString m_scbLeft{ NINODE_SCB_LEFT };
		BSFixedString m_bsx{ BSX };
		BSFixedString m_torchFire{ NINODE_TORCH_FIRE };
		BSFixedString m_object{ NINODE_IED_OBJECT };
		BSFixedString m_attachLight{ NINODE_ATTACH_LIGHT };
		BSFixedString m_glowAddMesh{ NINODE_GLOW_ADD_MESH };

		BSFixedString m_weaponAxe{ NINODE_AXE };
		BSFixedString m_weaponMace{ NINODE_MACE };
		BSFixedString m_weaponSword{ NINODE_SWORD };
		BSFixedString m_weaponDagger{ NINODE_DAGGER };
		BSFixedString m_weaponBack{ NINODE_WEAPON_BACK };
		BSFixedString m_weaponBow{ NINODE_BOW };
		BSFixedString m_quiver{ NINODE_QUIVER };

		BSFixedString m_mxTorchSmoke{ NINODE_MX_TORCH_SMOKE };
		BSFixedString m_mxTorchSparks{ NINODE_MX_TORCH_SPARKS };
		BSFixedString m_mxAttachSmoke{ NINODE_MX_ATTACH_SMOKE };
		BSFixedString m_mxAttachSparks{ NINODE_MX_ATTACH_SPARKS };
		BSFixedString m_attachENBLight{ BSVALUENODE_ATTACH_ENB_LIGHT };
		BSFixedString m_enbFireLightEmitter{ NINODE_ENB_FIRE_LIGHT_EMITTER };
		BSFixedString m_enbTorchLightEmitter{ NINODE_ENB_TORCH_LIGHT_EMITTER };

		//stl::set_sa<BSFixedString> m_sheathNodes;
	};

	class StringHolder
	{
	public:
		static inline constexpr auto FMT_NINODE_IED_GROUP = "IED GROUP [%.8X]";
		static inline constexpr auto HK_NPC_ROOT          = "NPC Root [Root]";

		[[nodiscard]] inline static constexpr const auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		StringHolder(const StringHolder&) = delete;
		StringHolder(StringHolder&&)      = delete;
		StringHolder& operator=(const StringHolder&) = delete;
		StringHolder& operator=(StringHolder&&) = delete;

		[[nodiscard]] inline constexpr const auto& GetSlotName(Data::ObjectSlot a_slot) const noexcept
		{
			return slotNames[stl::underlying(a_slot)];
		}

		stl::fixed_string save{ "Save" };
		stl::fixed_string snew{ "New" };
		stl::fixed_string apply{ "Apply" };
		stl::fixed_string merge{ "Merge" };
		stl::fixed_string IED{ "IED" };

		stl::fixed_string slotNames[stl::underlying(Data::ObjectSlot::kMax)];

	private:
		StringHolder();

		static StringHolder m_Instance;
	};

}