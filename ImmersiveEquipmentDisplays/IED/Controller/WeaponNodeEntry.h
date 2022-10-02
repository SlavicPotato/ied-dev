#pragma once

#include "IED/AnimationWeaponSlot.h"

namespace IED
{

	struct WeaponNodeEntry
	{
		friend class INodeOverride;

	public:
		WeaponNodeEntry(
			const stl::fixed_string&          a_nodeName,
			NiNode*                           a_node,
			NiNode*                           a_defaultNode,
			AnimationWeaponSlot               a_animID,
			const std::optional<NiTransform>& a_xfrm) :
			nodeName(a_nodeName),
			node(a_node),
			defaultNode(a_defaultNode),
			animSlot(a_animID),
			originalTransform(a_xfrm)
		{
		}

		const stl::fixed_string    nodeName;
		NiPointer<NiNode>          node;
		NiPointer<NiNode>          defaultNode;
		AnimationWeaponSlot        animSlot;
		std::optional<NiTransform> originalTransform;

	private:
		mutable NiPointer<NiNode> target;
	};

}