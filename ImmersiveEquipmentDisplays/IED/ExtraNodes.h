#pragma once

#include "NodeOverrideData.h"

namespace IED
{
	class SkeletonID;

	namespace SkeletonExtensions
	{
		void CreateExtraMovNodes(
			NiNode*           a_root,
			const SkeletonID& a_id) noexcept;

	}
}