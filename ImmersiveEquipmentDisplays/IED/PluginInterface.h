#pragma once

#include "IED/GearNodeData.h"
#include "IED/GearNodeID.h"
#include "IED/PluginOptionKey.h"
#include "IED/WeaponPlacementID.h"

#include <ext/BSString.h>

namespace IED
{
	class Controller;
	struct GearNodeEntry;

	class PluginInterface :
		PluginInterfaceBase
	{
	public:
		static constexpr auto          UNIQUE_ID         = stl::fnv1a_64::hash_string(PLUGIN_AUTHOR "_" PLUGIN_NAME);
		static constexpr std::uint32_t INTERFACE_VERSION = 1;

		PluginInterface(Controller& a_controller);

		virtual std::uint32_t GetPluginVersion() const override;
		virtual const char*   GetPluginName() const override;
		virtual std::uint32_t GetInterfaceVersion() const override;
		virtual const char*   GetInterfaceName() const override;
		virtual std::uint64_t GetUniqueID() const override;

		//

		virtual WeaponPlacementID GetPlacementHintForGearNode(TESObjectREFR* a_refr, GearNodeID a_id) const;
		virtual WeaponPlacementID GetPlacementHintForEquippedWeapon(TESObjectREFR* a_refr, bool a_leftHand) const;
		virtual RE::BSString      GetGearNodeParentName(TESObjectREFR* a_refr, GearNodeID a_id) const;
		virtual std::int32_t      GetPluginOption(PluginOptionKey a_key) const;

	private:
		std::optional<GearNodeData::Entry::Node> LookupNodeEntry(TESObjectREFR* a_refr, GearNodeID a_id) const;

		GearNodeID GetGearNodeIDForItem(
			const TESForm* a_form,
			bool           a_leftHand) const;

		Controller& m_controller;
	};
}