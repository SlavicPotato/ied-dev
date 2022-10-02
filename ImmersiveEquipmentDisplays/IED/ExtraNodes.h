#pragma once

#include "NodeOverrideData.h"

namespace IED
{
	namespace ExtraNodes
	{
		struct attachExtraNodesResult_t
		{
			NiNode* mov{ nullptr };
			NiNode* cme{ nullptr };
		};

		void CreateExtraMovNodes(
			NiAVObject* a_root);

		attachExtraNodesResult_t AttachExtraNodes(
			NiNode*                                       a_target,
			std::int32_t                                  a_skeletonID,
			const NodeOverrideData::extraNodeEntry_t&     a_entry,
			const NodeOverrideData::extraNodeEntrySkel_t& a_skelEntry);

	}
}