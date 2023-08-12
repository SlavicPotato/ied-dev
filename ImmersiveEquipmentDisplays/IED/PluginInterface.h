#pragma once

#include "IED/GearNodeID.h"
#include "IED/WeaponPlacementID.h"

namespace IED
{
	class Controller;

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

	private:
		Controller& m_controller;
	};
}