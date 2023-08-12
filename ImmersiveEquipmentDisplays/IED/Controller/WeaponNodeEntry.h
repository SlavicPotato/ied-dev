#pragma once

#include "IED/AnimationWeaponSlot.h"
#include "IED/GearNodeID.h"
#include "IED/WeaponPlacementID.h"

namespace IED
{
	struct WeaponNodeEntry
	{
		friend class INodeOverride;

	public:
		struct Node
		{
			NiPointer<NiNode> node;
			NiPointer<NiNode> defaultParentNode;

			[[nodiscard]] constexpr explicit operator bool() const noexcept
			{
				return static_cast<bool>(node.get());
			}
		};

		WeaponNodeEntry(
			const stl::fixed_string& a_nodeName,
			NiNode*                  a_node,
			NiNode*                  a_defaultNode,
			NiNode*                  a_node1p,
			NiNode*                  a_defaultNode1p,
			AnimationWeaponSlot      a_animID,
			GearNodeID               a_gearNodeID) noexcept :
			nodeName(a_nodeName),
			node3p{ a_node, a_defaultNode },
			node1p{ a_node1p, a_defaultNode1p },
			animSlot(a_animID),
			gearNodeID(a_gearNodeID)
		{
		}

		bool has_visible_geometry() const noexcept;

		stl::fixed_string         nodeName;
		Node                      node1p;
		Node                      node3p;
		AnimationWeaponSlot       animSlot;
		GearNodeID                gearNodeID;
		mutable WeaponPlacementID currentPlacement{ WeaponPlacementID::None };

		[[nodiscard]] friend constexpr bool operator<=(
			const WeaponNodeEntry& a_lhs,
			const WeaponNodeEntry& a_rhs) noexcept
		{
			return a_lhs.gearNodeID <= a_rhs.gearNodeID;
		}

		[[nodiscard]] friend constexpr bool operator==(
			const WeaponNodeEntry& a_lhs,
			const WeaponNodeEntry& a_rhs) noexcept
		{
			return a_lhs.gearNodeID == a_rhs.gearNodeID;
		}

		/*private:
		mutable NiPointer<NiNode> target;
		mutable NiPointer<NiNode> target1p;*/
	};

}