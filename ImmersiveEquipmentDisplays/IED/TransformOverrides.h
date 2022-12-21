#pragma once

namespace IED
{
	class SkeletonID;

	namespace SkeletonExtensions
	{
		void ApplyXP32NodeTransformOverrides(NiNode* a_root, const SkeletonID& a_skeletonId) noexcept;
	}
}