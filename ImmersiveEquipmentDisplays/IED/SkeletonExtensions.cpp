#include "pch.h"

#include "SkeletonExtensions.h"

#include "ConvertNodes.h"
#include "ConvertNodes2.h"
#include "ExtraNodes.h"
#include "TransformOverrides.h"

#include "NodeOverrideData.h"
#include "SkeletonID.h"
#include "StringHolder.h"
#include "ConvertNodes2.h"

namespace IED
{
	namespace SkeletonExtensions
	{
		using namespace ::Util::Node;

		void PostLoad3D(NiAVObject* a_root, bool a_applyTransformOverrides) noexcept
		{
			using namespace ::Util::Node;

			if (!a_root)
			{
				return;
			}

			auto root = a_root->AsNode();
			if (!root)
			{
				return;
			}

			auto npcroot = GetNodeByName(
				root,
				BSStringHolder::GetSingleton()->m_npcroot);

			if (!npcroot)
			{
				return;
			}

			SkeletonID id(root);

			RunConvertNodes2(root, id);

			if (NodeOverrideData::GetConvertNodes().test(id))
			{
				ConvertVanillaSheathsToXP32(npcroot);
			}

			CreateExtraMovNodes(npcroot, id);

			if (a_applyTransformOverrides)
			{
				ApplyXP32NodeTransformOverrides(npcroot, id);
			}
		}
	}
}