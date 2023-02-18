#pragma once

#include "IED/Physics/SimComponent.h"
#include "IED/WeaponPlacementID.h"

#include "IED/SkeletonCache.h"

namespace IED
{
	struct MOVNodeEntry
	{
		struct Node
		{
			Node() = default;

			Node(
				NiNode*            a_node,
				const NiTransform& a_xfrm) noexcept;

			Node(
				NiNode*                          a_node,
				const SkeletonCache::ActorEntry& a_sce,
				const stl::fixed_string&         a_name) noexcept;

			Node(
				NiNode*                          a_root,
				const SkeletonCache::ActorEntry& a_sce,
				const stl::fixed_string&         a_name,
				const BSFixedString&             a_bsname) noexcept;

			[[nodiscard]] constexpr explicit operator bool() const noexcept
			{
				return static_cast<bool>(node.get());
			}

			bool has_visible_geometry() const noexcept;
			bool parent_has_visible_geometry() const noexcept;

			NiPointer<NiNode>                       node;
			NiTransform                             orig{ NiTransform::noinit_arg_t{} };  // cached or read from loaded actor 3D
			mutable stl::smart_ptr<PHYSimComponent> simComponent;
		};

		inline MOVNodeEntry(
			NiNode*                          a_node3p,
			NiNode*                          a_root1p,
			const SkeletonCache::ActorEntry& a_sce3p,
			const SkeletonCache::ActorEntry& a_sce1p,
			const stl::fixed_string&         a_name,
			const BSFixedString&             a_bsname,
			WeaponPlacementID                a_placementID) noexcept :
			thirdPerson(a_node3p, a_sce3p, a_name),
			firstPerson(a_root1p, a_sce1p, a_name, a_bsname),
			placementID(a_placementID)
		{
		}

		const Node              thirdPerson;
		const Node              firstPerson;
		const WeaponPlacementID placementID;
	};
}