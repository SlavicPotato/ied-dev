#include "pch.h"

#include "ExtraNodes.h"

#include "Controller/INode.h"

#include "SkeletonID.h"
#include "StringHolder.h"

namespace IED
{
	namespace ExtraNodes
	{
		attachExtraNodesResult_t AttachExtraNodes(
			NiNode*                                       a_target,
			std::int32_t                                  a_skeletonID,
			const NodeOverrideData::extraNodeEntry_t&     a_entry,
			const NodeOverrideData::extraNodeEntrySkel_t& a_skelEntry)
		{
			auto cme = INode::CreateAttachmentNode(a_entry.bsname_cme);
			a_target->AttachChild(cme, true);

			auto mov = INode::CreateAttachmentNode(a_entry.bsname_mov);

			mov->m_localTransform = a_skelEntry.transform_mov;

			cme->AttachChild(mov, true);

			auto node = INode::CreateAttachmentNode(a_entry.bsname_node);

			node->m_localTransform = a_skelEntry.transform_node;

			mov->AttachChild(node, true);

			INode::UpdateDownwardPass(cme);

			return { mov, cme };
		}

		void CreateExtraMovNodes(
			NiAVObject* a_root)
		{
			if (!a_root)
			{
				return;
			}

			auto& movNodes = NodeOverrideData::GetExtraMovNodes();

			if (movNodes.empty())
			{
				return;
			}

			auto sh = BSStringHolder::GetSingleton();
			if (!sh)
			{
				return;
			}

			auto root = a_root->AsNode();
			if (!root)
			{
				return;
			}

			auto npcroot = ::Util::Node::FindNode(root, sh->m_npcroot);
			if (!npcroot)
			{
				return;
			}

			SkeletonID id(root);

			if (!id.get_id())
			{
				return;
			}

			//_DMESSAGE("%X: %u", a_actor->formID, id.get_id());

			for (auto& v : movNodes)
			{
				if (npcroot->GetObjectByName(v.bsname_cme) ||
				    npcroot->GetObjectByName(v.bsname_mov) ||
				    npcroot->GetObjectByName(v.bsname_node))
				{
					continue;
				}

				auto target = ::Util::Node::FindNode(npcroot, v.name_parent);
				if (!target)
				{
					continue;
				}

				auto it = std::find_if(
					v.skel.begin(),
					v.skel.end(),
					[&](auto& a_v) {
						return a_v.ids.contains(*id.get_id());
					});

				if (it != v.skel.end())
				{
					AttachExtraNodes(target, *id.get_id(), v, *it);
				}
			}
		}
	}
}