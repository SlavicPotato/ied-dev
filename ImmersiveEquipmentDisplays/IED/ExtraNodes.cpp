#include "pch.h"

#include "ExtraNodes.h"

#include "SkeletonID.h"
#include "StringHolder.h"

namespace IED
{
	namespace SkeletonExtensions
	{
		using namespace ::Util::Node;

		namespace detail
		{
			static NiTransform get_transform_impl(
				NiNode*                                                a_root,
				const NodeOverrideData::extraNodeEntrySkelTransform_t& a_skelXfrm) noexcept
			{
				if (!a_skelXfrm.bsReadFromObj.empty())
				{
					if (const auto srcObj = a_root->GetObjectByName(a_skelXfrm.bsReadFromObj))
					{
						return srcObj->m_localTransform;
					}
					else
					{
						return {};
					}
				}
				else
				{
					return a_skelXfrm.xfrm;
				}
			}
			
			static NiTransform get_transform(
				NiNode*                                                a_root,
				const NodeOverrideData::extraNodeEntrySkelTransform_t& a_skelXfrm) noexcept
			{
				auto result = get_transform_impl(a_root, a_skelXfrm);

				if (a_skelXfrm.invert)
				{
					result.Invert();
				}

				return result;
			}

			static void create_and_attach(
				NiNode*                                       a_root,
				NiNode*                                       a_target,
				const NodeOverrideData::extraNodeEntry_t&     a_entry,
				const NodeOverrideData::extraNodeEntrySkel_t& a_skelEntry) noexcept
			{
				const auto cme = CreateAttachmentNode(a_entry.bsname_cme);

				a_target->AttachChild(cme, false);

				const auto mov = CreateAttachmentNode(a_entry.names[0].second);

				mov->m_localTransform = get_transform(a_root, a_skelEntry.sxfrms[0]);

				cme->AttachChild(mov, true);

				const auto node = CreateAttachmentNode(a_entry.names[1].second);

				node->m_localTransform = get_transform(a_root, a_skelEntry.sxfrms[1]);

				mov->AttachChild(node, true);

				UpdateDownwardPass(cme);
			}
		}

		void CreateExtraMovNodes(
			NiNode*           a_root,
			const SkeletonID& a_id) noexcept
		{
			using namespace ::Util::Node;

			for (auto& v : NodeOverrideData::GetExtraMovNodes())
			{
				if (a_root->GetObjectByName(v.bsname_cme) ||
				    a_root->GetObjectByName(v.names[0].second) ||
				    a_root->GetObjectByName(v.names[1].second))
				{
					continue;
				}

				const auto target = GetNodeByName(a_root, v.name_parent);
				if (!target)
				{
					continue;
				}

				const auto it = std::find_if(
					v.skel.begin(),
					v.skel.end(),
					[&](auto& a_v) noexcept {
						return a_v.match.test(a_id);
					});

				if (it != v.skel.end())
				{
					//_DMESSAGE("creating %s on %zX", v.name_node.c_str(), a_id.signature());

					detail::create_and_attach(a_root, target, v, *it);
				}
			}
		}
	}
}