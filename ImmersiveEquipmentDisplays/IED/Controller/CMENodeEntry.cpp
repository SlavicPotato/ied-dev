#include "pch.h"

#include "CMENodeEntry.h"

#include "IED/StringHolder.h"

namespace IED
{
	bool CMENodeEntry::find_visible_geometry(
		NiAVObject*           a_object,
		const BSStringHolder* a_sh) noexcept
	{
		using namespace ::Util::Node;

		auto result = Traverse(a_object, [a_sh](NiAVObject* a_object) {
			if (a_object->IsHidden())
			{
				return VisitorControl::kSkip;
			}

			if (a_sh)
			{
				if (a_object->m_name == a_sh->m_scb ||
				    a_object->m_name == a_sh->m_scbLeft)
				{
					return VisitorControl::kSkip;
				}
			}

			return a_object->AsGeometry() ?
			           VisitorControl::kStop :
			           VisitorControl::kContinue;
		});

		return result == VisitorControl::kStop;
	}

	bool CMENodeEntry::has_visible_geometry(
		const BSStringHolder* a_sh) const noexcept
	{
		return find_visible_geometry(thirdPerson.node, a_sh);
	}

	bool CMENodeEntry::has_visible_object(
		NiAVObject* a_findObject) const noexcept
	{
		using namespace ::Util::Node;

		struct
		{
			NiAVObject* findObject;
			bool        result{ false };
		} args{
			a_findObject
		};

		Traverse(thirdPerson.node, [&args](NiAVObject* a_object) {
			if (args.findObject == a_object)
			{  // object found, verify is has visible geometry

				auto visitorResult = Traverse(a_object, [](NiAVObject* a_object) {
					return a_object->IsHidden() ?
					           VisitorControl::kSkip :
					           (a_object->AsGeometry() ?
					                VisitorControl::kStop :
					                VisitorControl::kContinue);
				});

				args.result = (visitorResult == VisitorControl::kStop);

				return VisitorControl::kStop;
			}
			else
			{
				return a_object->IsHidden() ?
				           VisitorControl::kSkip :
				           VisitorControl::kContinue;
			}
		});

		return args.result;
	}

	CMENodeEntry::Node::Node(
		NiNode*            a_node,
		const NiTransform& a_xfrm) noexcept :
		node(a_node),
		orig(a_xfrm)
	{
	}

	CMENodeEntry::Node::Node(
		NiNode*                          a_node,
		const SkeletonCache::ActorEntry& a_sce,
		const stl::fixed_string&         a_name) noexcept :
		node(a_node),
		orig(a_sce.GetCachedOrZeroTransform(a_name))
	{
	}

	CMENodeEntry::Node::Node(
		NiNode*                          a_root,
		const SkeletonCache::ActorEntry& a_sce,
		const stl::fixed_string&         a_name,
		const BSFixedString&             a_bsname) noexcept :
		node(a_root ? ::Util::Node::GetNodeByName(a_root, a_bsname) : nullptr)
	{
		if (node)
		{
			orig = a_sce.GetCachedOrZeroTransform(a_name);
		}
	}

}