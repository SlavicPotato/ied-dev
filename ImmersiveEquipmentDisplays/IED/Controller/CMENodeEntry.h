#pragma once

#include "IED/ConfigNodeOverride.h"

namespace IED
{
	class BSStringHolder;

	struct CMENodeEntry
	{
		CMENodeEntry(
			NiNode*            a_node,
			const NiTransform& a_originalTransform) :
			node(a_node),
			orig(a_originalTransform)
#if defined(IED_ENABLE_1D10T_SAFEGUARDS)
			,
			current(a_originalTransform)
#endif
		{
		}

		NiPointer<NiNode> node;
		NiTransform       orig;  // cached or zero, never read from loaded actor 3D

#if defined(IED_ENABLE_1D10T_SAFEGUARDS)
		mutable NiTransform current;
#endif

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