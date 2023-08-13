#include "pch.h"

#include "PluginInterface.h"

#include "Controller/Controller.h"
#include "GearNodeData.h"

namespace IED
{
	PluginInterface::PluginInterface(
		Controller& a_controller) :
		m_controller(a_controller)
	{
	}

	std::uint32_t PluginInterface::GetPluginVersion() const
	{
		return MAKE_PLUGIN_VERSION(
			PLUGIN_VERSION_MAJOR,
			PLUGIN_VERSION_MINOR,
			PLUGIN_VERSION_REVISION);
	}

	std::uint32_t PluginInterface::GetInterfaceVersion() const
	{
		return INTERFACE_VERSION;
	}

	const char* PluginInterface::GetPluginName() const
	{
		return PLUGIN_NAME_FULL;
	}

	const char* PluginInterface::GetInterfaceName() const
	{
		return "Main";
	}

	std::uint64_t PluginInterface::GetUniqueID() const
	{
		return UNIQUE_ID;
	}

	WeaponPlacementID PluginInterface::GetPlacementHintForGearNode(
		TESObjectREFR* a_refr,
		GearNodeID     a_id) const
	{
		const auto entry = LookupNodeEntry(a_refr, a_id);
		return entry ? entry->currentPlacement : WeaponPlacementID::None;
	}

	GearNodeID PluginInterface::GetGearNodeIDForItem(
		const TESForm* a_form,
		bool           a_leftHand) const
	{
		switch (a_form->formType)
		{
		case TESObjectWEAP::kTypeID:

			switch (static_cast<const TESObjectWEAP*>(a_form)->type())
			{
			case WEAPON_TYPE::kOneHandSword:
				return a_leftHand ? GearNodeID::k1HSwordLeft : GearNodeID::k1HSword;
			case WEAPON_TYPE::kOneHandDagger:
				return a_leftHand ? GearNodeID::kDaggerLeft : GearNodeID::kDagger;
			case WEAPON_TYPE::kOneHandAxe:
				return a_leftHand ? GearNodeID::k1HAxeLeft : GearNodeID::k1HAxe;
			case WEAPON_TYPE::kOneHandMace:
				return a_leftHand ? GearNodeID::kMaceLeft : GearNodeID::kMace;
			case WEAPON_TYPE::kTwoHandSword:
				return a_leftHand ? GearNodeID::kTwoHandedLeft : GearNodeID::kTwoHanded;
			case WEAPON_TYPE::kTwoHandAxe:
				return m_controller.IsWeaponNodeSharingDisabled() ?
				           (a_leftHand ? GearNodeID::kTwoHandedAxeMaceLeft : GearNodeID::kTwoHandedAxeMace) :
				           (a_leftHand ? GearNodeID::kTwoHandedLeft : GearNodeID::kTwoHanded);
			case WEAPON_TYPE::kBow:
				return GearNodeID::kBow;
			case WEAPON_TYPE::kStaff:
				return a_leftHand ? GearNodeID::kStaff : GearNodeID::kStaffLeft;
			case WEAPON_TYPE::kCrossbow:
				return m_controller.IsWeaponNodeSharingDisabled() ? GearNodeID::kCrossBow : GearNodeID::kBow;
			default:
				return GearNodeID::kNone;
			}

			break;
		case TESObjectARMO::kTypeID:
			return static_cast<const TESObjectARMO*>(a_form)->IsShield() ? GearNodeID::kShield : GearNodeID::kNone;
		case TESAmmo::kTypeID:
			return GearNodeID::kQuiver;
		default:
			return GearNodeID::kNone;
		}
	}

	WeaponPlacementID PluginInterface::GetPlacementHintForEquippedWeapon(
		TESObjectREFR* a_refr,
		bool           a_leftHand) const
	{
		if (!a_refr)
		{
			return WeaponPlacementID::None;
		}

		const auto actor = a_refr->As<Actor>();
		if (!actor)
		{
			return WeaponPlacementID::None;
		}

		const auto equippedObject = actor->GetEquippedObject(a_leftHand);
		if (!equippedObject)
		{
			return WeaponPlacementID::None;
		}

		const auto id    = GetGearNodeIDForItem(equippedObject, a_leftHand);
		const auto entry = LookupNodeEntry(a_refr, id);

		return entry ? entry->currentPlacement : WeaponPlacementID::None;
	}

	RE::BSString PluginInterface::GetGearNodeParentName(
		TESObjectREFR* a_refr,
		GearNodeID     a_id) const
	{
		if (const auto entry = LookupNodeEntry(a_refr, a_id))
		{
			if (const auto parent = entry->node->m_parent)
			{
				return parent->m_name.data();
			}
		}

		return {};
	}

	std::optional<GearNodeData::Entry::Node> PluginInterface::LookupNodeEntry(
		TESObjectREFR* a_refr,
		GearNodeID     a_id) const
	{
		if (a_refr != nullptr && a_id != GearNodeID::kNone)
		{
			return GearNodeData::GetSingleton().GetNodeEntry(a_refr->formID, a_id);
		}
		else
		{
			return {};
		}
	}

}