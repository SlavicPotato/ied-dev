#include "pch.h"

#include "MOVNodeEntry.h"

namespace IED
{
	MOVNodeEntry::Node::Node(
		NiNode*            a_node,
		const NiTransform& a_xfrm) noexcept :
		node(a_node),
		orig(a_xfrm)
	{
	}

	MOVNodeEntry::Node::Node(
		NiNode*                          a_node,
		const SkeletonCache::ActorEntry& a_sce,
		const stl::fixed_string&         a_name) noexcept :
		node(a_node),
		orig(a_sce.GetCachedOrCurrentTransform(a_name, a_node))
	{
	}

	MOVNodeEntry::Node::Node(
		NiNode*                          a_root,
		const SkeletonCache::ActorEntry& a_sce,
		const stl::fixed_string&         a_name,
		const BSFixedString&             a_bsname) noexcept :
		node(a_root ? ::Util::Node::GetNodeByName(a_root, a_bsname) : nullptr)
	{
		if (node)
		{
			orig = a_sce.GetCachedOrCurrentTransform(a_name, node);
		}
	}

	bool MOVNodeEntry::Node::has_visible_geometry() const noexcept
	{
		return ::Util::Node::HasVisibleGeometry(node);
	}

	bool MOVNodeEntry::Node::parent_has_visible_geometry() const noexcept
	{
		return ::Util::Node::HasVisibleGeometry(node->m_parent);
	}
}