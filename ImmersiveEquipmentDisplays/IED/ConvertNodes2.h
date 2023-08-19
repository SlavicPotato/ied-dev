#pragma once

namespace IED
{
	class SkeletonID;

	namespace SkeletonExtensions
	{
		void RunConvertNodes2(NiNode* a_root, const SkeletonID &a_id) noexcept;
	}
}