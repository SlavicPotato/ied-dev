#pragma once

#include "NodeOverrideData.h"

namespace IED
{
	class SkeletonID;

	namespace SkeletonExtensions
	{
		struct attachExtraNodesResult_t
		{
			NiNode* mov{ nullptr };
			NiNode* cme{ nullptr };
		};

		void CreateExtraMovNodes(
			NiNode*           a_root,
			const SkeletonID& a_id) noexcept;

		attachExtraNodesResult_t AttachExtraNodes(
			NiNode*                                       a_target,
			const NodeOverrideData::extraNodeEntry_t&     a_entry,
			const NodeOverrideData::extraNodeEntrySkel_t& a_skelEntry) noexcept;

	}
}