#pragma once

#include "IED/ConfigNodeOverride.h"

namespace IED
{
	class BSStringHolder;

	struct CMENodeEntry
	{
		struct Node
		{
			[[nodiscard]] constexpr explicit operator bool() const noexcept
			{
				return static_cast<bool>(node.get());
			}

			NiPointer<NiNode> node;
			NiTransform       orig;  // cached or zero, never read from loaded actor 3D
		};

		inline CMENodeEntry(
			NiNode*            a_node3p,
			const NiTransform& a_originalTransform3p) noexcept :
			thirdPerson{ a_node3p, a_originalTransform3p }
		{
		}

		inline CMENodeEntry(
			NiNode*            a_node3p,
			const NiTransform& a_originalTransform3p,
			NiNode*            a_node1p,
			const NiTransform& a_originalTransform1p) noexcept :
			thirdPerson{ a_node3p, a_originalTransform3p },
			firstPerson{ a_node1p, a_originalTransform1p }
		{
		}

		Node thirdPerson;
		Node firstPerson;

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