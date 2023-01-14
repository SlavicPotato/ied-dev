#include "pch.h"

#include "TransformOverrides.h"

#include "NodeOverrideData.h"
#include "SkeletonID.h"

namespace IED
{
	namespace SkeletonExtensions
	{
		using namespace ::Util::Node;

		void ApplyXP32NodeTransformOverrides(NiNode* a_root, const SkeletonID& a_skeletonId) noexcept
		{
			auto& sig = a_skeletonId.signature();

			// apply only to XPMSSE skeleton

			if (!NodeOverrideData::GetHumanoidSkeletonSignaturesXp32().contains(sig))
			{
				return;
			}

			for (auto& e : NodeOverrideData::GetTransformOverrideData())
			{
				if (auto node = GetNodeByName(a_root, e.name))
				{
					node->m_localTransform.rot = e.rot;
				}
			}
		}

	}
}