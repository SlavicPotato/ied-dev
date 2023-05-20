#pragma once

#include "ConfigSkeletonMatch.h"
#include "ConfigTransform.h"

#include "WeaponPlacementID.h"

namespace IED
{
	namespace Data
	{
		enum class ExtraNodeEntrySkelTransformSyncNodeFlags : std::uint32_t
		{
			kNone = 0,

			kInvert = 1u << 0
		};

		DEFINE_ENUM_CLASS_BITWISE(ExtraNodeEntrySkelTransformSyncNodeFlags);

		struct configExtraNodeEntrySkelTransformSyncNode_t
		{
			stl::fixed_string                                   name;
			stl::flag<ExtraNodeEntrySkelTransformSyncNodeFlags> flags{ ExtraNodeEntrySkelTransformSyncNodeFlags::kNone };
		};

		struct configExtraNodeEntrySkelTransform_t
		{
			configTransform_t                                        xfrm;
			std::vector<configExtraNodeEntrySkelTransformSyncNode_t> syncNodes;
			stl::fixed_string                                        read_from;
		};

		struct configExtraNodeEntrySkel_t
		{
			configSkeletonMatch_t                              match;
			std::vector<std::pair<stl::fixed_string, bool>>    objMatch;
			std::array<configExtraNodeEntrySkelTransform_t, 2> sxfrms;
		};

		struct configExtraNodeEntry_t
		{
			stl::fixed_string                     name;
			stl::fixed_string                     ovr_cme_name;
			stl::fixed_string                     ovr_mov_name;
			stl::fixed_string                     parent;
			stl::fixed_string                     desc;
			stl::list<configExtraNodeEntrySkel_t> skel;
			WeaponPlacementID                     placementID;
		};

		using configExtraNodeList_t = stl::list<configExtraNodeEntry_t>;
	}
}