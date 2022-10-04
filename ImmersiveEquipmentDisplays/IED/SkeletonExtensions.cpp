#include "pch.h"

#include "SkeletonExtensions.h"

#include "ConvertNodes.h"
#include "ExtraNodes.h"

#include "SkeletonID.h"
#include "StringHolder.h"

#include <ext/Node.h>

namespace IED
{
	namespace SkeletonExtensions
	{
		void PostLoad3D(NiAVObject* a_root)
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

			auto npcroot = FindNode(
				root,
				BSStringHolder::GetSingleton()->m_npcroot);

			if (!npcroot)
			{
				return;
			}

			SkeletonID id(root);

			if (NodeOverrideData::GetConvertNodes().test(id))
			{
				ConvertVanillaSheathsToXP32(npcroot);
			}

			CreateExtraMovNodes(npcroot, id);
		}
	}
}