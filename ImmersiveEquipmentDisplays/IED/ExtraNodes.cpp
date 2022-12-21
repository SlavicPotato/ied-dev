#include "pch.h"

#include "ExtraNodes.h"

#include "SkeletonID.h"
#include "StringHolder.h"

namespace IED
{
	namespace SkeletonExtensions
	{
		using namespace ::Util::Node;

		attachExtraNodesResult_t AttachExtraNodes(
			NiNode*                                       a_target,
			const NodeOverrideData::extraNodeEntry_t&     a_entry,
			const NodeOverrideData::extraNodeEntrySkel_t& a_skelEntry) noexcept
		{
			auto cme = CreateAttachmentNode(a_entry.bsname_cme);

			a_target->AttachChild(cme, false);

			auto mov = CreateAttachmentNode(a_entry.bsname_mov);

			mov->m_localTransform = a_skelEntry.transform_mov;

			cme->AttachChild(mov, true);

			auto node = CreateAttachmentNode(a_entry.bsname_node);

			node->m_localTransform = a_skelEntry.transform_node;

			mov->AttachChild(node, true);

			UpdateDownwardPass(cme);

			return { mov, cme };
		}

		void CreateExtraMovNodes(
			NiNode*           a_root,
			const SkeletonID& a_id) noexcept
		{
			using namespace ::Util::Node;

			for (auto& v : NodeOverrideData::GetExtraMovNodes())
			{
				if (a_root->GetObjectByName(v.bsname_cme) ||
				    a_root->GetObjectByName(v.bsname_mov) ||
				    a_root->GetObjectByName(v.bsname_node))
				{
					continue;
				}

				auto target = FindNode(a_root, v.name_parent);
				if (!target)
				{
					continue;
				}

				auto it = std::find_if(
					v.skel.begin(),
					v.skel.end(),
					[&](auto& a_v) noexcept {
						return a_v.match.test(a_id);
					});

				if (it != v.skel.end())
				{
					//_DMESSAGE("creating %s on %zX", v.name_node.c_str(), a_id.signature());

					AttachExtraNodes(target, v, *it);
				}
			}
		}
	}
}