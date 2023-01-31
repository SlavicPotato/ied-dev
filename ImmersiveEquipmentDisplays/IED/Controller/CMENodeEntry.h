#pragma once

#include "IED/ConfigNodeOverride.h"
#include "IED/SkeletonCache.h"

namespace IED
{
	class BSStringHolder;

	struct CMENodeEntry
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

			NiPointer<NiNode> node;
			NiTransform       orig{ NiTransform::noinit_arg_t{} };  // cached or zero, never read from loaded actor 3D
		};

		inline CMENodeEntry(
			NiNode*            a_node3p,
			const NiTransform& a_originalTransform3p) noexcept :
			thirdPerson(a_node3p, a_originalTransform3p)
		{
		}

		inline CMENodeEntry(
			NiNode*                          a_node3p,
			NiNode*                          a_root1p,
			const SkeletonCache::ActorEntry& a_sce3p,
			const SkeletonCache::ActorEntry& a_sce1p,
			const stl::fixed_string&         a_name,
			const BSFixedString&             a_bsname) noexcept :
			thirdPerson(a_node3p, a_sce3p, a_name),
			firstPerson(a_root1p, a_sce1p, a_name, a_bsname)
		{
		}

		const Node thirdPerson;
		const Node firstPerson;

		static bool find_visible_geometry(
			NiAVObject*           a_object,
			const BSStringHolder* a_sh) noexcept;

		bool has_visible_geometry(
			const BSStringHolder* a_sh) const noexcept;

		bool has_visible_object(
			NiAVObject* a_findObject) const noexcept;

		mutable const Data::configNodeOverrideEntryTransform_t* cachedConfCME{ nullptr };
	};

}