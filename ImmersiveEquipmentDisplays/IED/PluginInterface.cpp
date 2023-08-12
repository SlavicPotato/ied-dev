#include "pch.h"

#include "PluginInterface.h"

#include "Controller/Controller.h"

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

	WeaponPlacementID PluginInterface::GetPlacementHintForGearNode(TESObjectREFR* a_refr, GearNodeID a_id) const
	{
		const stl::lock_guard lock(m_controller.GetLock());

		const auto wn = LookupWeaponNodeEntry(a_refr, a_id);
		return wn ? wn->currentPlacement : WeaponPlacementID::None;
	}

	RE::BSString PluginInterface::GetGearNodeParentName(TESObjectREFR* a_refr, GearNodeID a_id) const
	{
		const stl::lock_guard lock(m_controller.GetLock());

		if (const auto wn = LookupWeaponNodeEntry(a_refr, a_id))
		{
			if (const auto parent = wn->node3p.node->m_parent)
			{
				return parent->m_name.data();
			}
		}

		return {};
	}

	const WeaponNodeEntry* PluginInterface::LookupWeaponNodeEntry(
		TESObjectREFR* a_refr,
		GearNodeID     a_id) const
	{
		if (a_refr == nullptr || a_id == GearNodeID::kNone)
		{
			return nullptr;
		}

		const auto& actorMap = m_controller.GetActorMap();

		auto ita = actorMap.find(a_refr->formID);
		if (ita == actorMap.end())
		{
			return nullptr;
		}

		const auto& weapNodes = ita->second.GetWeapNodes();

		auto itw = std::upper_bound(
			weapNodes.begin(),
			weapNodes.end(),
			a_id,
			ActorObjectHolder::WeaponNodeSetUBPredicate{});

		return itw != weapNodes.end() && itw->gearNodeID == a_id ? std::addressof(*itw) : nullptr;
	}
}