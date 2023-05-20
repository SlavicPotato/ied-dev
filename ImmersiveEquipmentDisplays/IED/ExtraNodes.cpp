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

				NiNode* p = cme;

				static_assert(
					stl::array_size_v<decltype(a_entry.names)> ==
					stl::array_size_v<decltype(a_skelEntry.sxfrms)>);

				for (std::size_t i = 0; i < std::size(a_entry.names); i++)
				{
					auto& name = a_entry.names[i];
					auto& xfrm = a_skelEntry.sxfrms[i];

					const auto n        = CreateAttachmentNode(name.second);
					n->m_localTransform = get_transform(a_root, xfrm);
					p->AttachChild(n, true);

					p = n;
				}

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

				const auto it = v.skel.find(a_id, a_root);
				if (it != v.skel.end())
				{
					//_DMESSAGE("creating %s on %zX", v.name_node.c_str(), a_id.signature());

					detail::create_and_attach(a_root, target, v, *it);
				}
			}
		}
	}
}