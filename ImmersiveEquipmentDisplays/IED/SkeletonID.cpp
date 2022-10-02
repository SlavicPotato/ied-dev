#include "pch.h"

#include "SkeletonID.h"

#include "IED/NodeOverrideData.h"

#include <ext/Node.h>

namespace IED
{
	SkeletonID::SkeletonID(NiNode* a_root)
	{
		if (auto extra = a_root->GetExtraData<NiIntegerExtraData>(
				NodeOverrideData::GetSkelIDExtraDataName()))
		{
			m_id = extra->m_data;
		}

		if (auto npcNode = ::Util::Node::FindChildNode(
				a_root,
				NodeOverrideData::GetNPCNodeName()))
		{
			if (auto extra = npcNode->GetExtraData<NiFloatExtraData>(
					NodeOverrideData::GetXPMSEExtraDataName()))
			{
				m_xpmse_version = extra->m_data;
			}
		}
	}
}