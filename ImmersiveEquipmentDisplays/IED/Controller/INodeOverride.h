#pragma once

#include "CMENodeEntry.h"
#include "IED/ConfigCommon.h"
#include "IED/FormCommon.h"
#include "IED/FormHolder.h"
#include "IED/Inventory.h"
#include "IED/NodeOverrideData.h"
#include "IED/NodeOverrideParams.h"

namespace IED
{
	namespace Data
	{
		struct configNodeOverrideTransform_t;
	}

	class ActorObjectHolder;
	class Controller;
	struct WeaponNodeEntry;

	class INodeOverride
	{
	public:
		static void ResetNodeOverrideImpl(
			const CMENodeEntry::Node& a_node) noexcept;

		static void ResetNodeOverride(
			const CMENodeEntry& a_entry,
			bool                a_defer) noexcept;

		static void ResetNodePlacement(
			const WeaponNodeEntry& a_entry,
			nodeOverrideParams_t*  a_params,
			bool                   a_defer) noexcept;

		static void ApplyNodeOverride(
			const stl::fixed_string&                   a_name,
			const CMENodeEntry&                        a_entry,
			const Data::configNodeOverrideTransform_t& a_data,
			nodeOverrideParams_t&                      a_params) noexcept;

		static void ApplyNodeVisibility(
			const CMENodeEntry&                        a_entry,
			const Data::configNodeOverrideTransform_t& a_data,
			nodeOverrideParams_t&                      a_params) noexcept;

		static void attach_node_to(
			const WeaponNodeEntry&   a_entry,
			const NiPointer<NiNode>& a_target,
			nodeOverrideParams_t*    a_params,
			WeaponPlacementID        a_placementID,
			bool                     a_defer) noexcept;

		static void ApplyNodePlacement(
			const Data::configNodeOverridePlacement_t& a_data,
			const WeaponNodeEntry&                     a_entry,
			nodeOverrideParams_t&                      a_params) noexcept;

		static const Data::configNodePhysicsValues_t& GetPhysicsConfig(
			const Data::configNodeOverridePhysics_t& a_data,
			nodeOverrideParams_t&                    a_params) noexcept;

	private:
		static constexpr const stl::fixed_string& get_target_node(
			const Data::configNodeOverridePlacement_t& a_data,
			nodeOverrideParams_t&                      a_params) noexcept;

		static bool process_offsets(
			const Data::configNodeOverrideOffsetList_t& a_data,
			NiTransform&                                a_out,
			NiPoint3&                                   a_posAccum,
			nodeOverrideParams_t&                       a_params) noexcept;
	};

}