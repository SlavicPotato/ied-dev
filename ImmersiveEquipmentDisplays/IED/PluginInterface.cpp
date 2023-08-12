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