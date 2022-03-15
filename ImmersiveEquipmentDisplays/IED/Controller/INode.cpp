#include "pch.h"

#include "IED/EngineExtensions.h"
#include "INode.h"

#include <ext/Node.h>

namespace IED
{
	using namespace ::Util::Node;

	static void UpdateNodeDataImpl(
		NiAVObject*                   a_node,
		const Data::cacheTransform_t& a_trnsf)
	{
		if (a_trnsf.scale)
		{
			a_node->m_localTransform.scale = *a_trnsf.scale;
		}
		else
		{
			a_node->m_localTransform.scale = 1.0f;
		}

		if (a_trnsf.position)
		{
			a_node->m_localTransform.pos = *a_trnsf.position;
		}
		else
		{
			a_node->m_localTransform.pos = {};
		}

		if (a_trnsf.rotation)
		{
			a_node->m_localTransform.rot = *a_trnsf.rotation;
		}
		else
		{
			a_node->m_localTransform.rot.Identity();
		}
	}

	static void UpdateNodeDataImpl(
		NiAVObject*                   a_node,
		NiNode*                       a_refNode,
		const Data::cacheTransform_t& a_trnsf)
	{
		if (a_trnsf.scale)
		{
			a_node->m_localTransform.scale = std::clamp(
				a_refNode->m_localTransform.scale * *a_trnsf.scale,
				0.01f,
				100.0f);
		}
		else
		{
			a_node->m_localTransform.scale = a_refNode->m_localTransform.scale;
		}

		if (a_trnsf.rotation)
		{
			a_node->m_localTransform.rot =
				a_refNode->m_localTransform.rot * *a_trnsf.rotation;
		}
		else
		{
			a_node->m_localTransform.rot = a_refNode->m_localTransform.rot;
		}

		if (a_trnsf.position)
		{
			a_node->m_localTransform.pos =
				a_refNode->m_localTransform * *a_trnsf.position;
		}
		else
		{
			a_node->m_localTransform.pos = a_refNode->m_localTransform.pos;
		}
	}

	auto INode::FindNodes(
		const Data::NodeDescriptor& a_node,
		bool                        a_atmReference,
		NiNode*                     a_root)
		-> findResult_t
	{
		BSFixedString nodeName(a_node.name.c_str());

		auto obj = a_root->GetObjectByName(nodeName);

		if (!obj)
		{
			return {};
		}

		if (a_node.managed() || a_atmReference)
		{
			if (auto refNode = obj->GetAsNiNode())
			{
				return { refNode->m_parent, refNode };
			}
			else
			{
				return {};
			}
		}
		else
		{
			return { obj->GetAsNiNode() };
		}
	}

	BSFixedString INode::GetTargetNodeName(
		const Data::NodeDescriptor& a_node,
		bool                        a_atmReference)
	{
		char tmp[MAX_PATH];

		auto fmt = (a_node.managed() || a_atmReference) ? "IED R %s" : "IED P %s";

		stl::snprintf(tmp, fmt, a_node.name.c_str());

		return tmp;
	}

	void INode::UpdateDownwardPass(NiAVObject* a_object)
	{
		NiAVObject::ControllerUpdateContext ctx{
			//static_cast<float>(*EngineExtensions::m_unkglob0),
			0,
			0
		};
		a_object->UpdateDownwardPass(ctx, nullptr);
	}

	bool INode::CreateTargetNode(
		const Data::configBaseValues_t& a_entry,
		const Data::NodeDescriptor&     a_node,
		NiNode*                         a_root,
		nodesRef_t&                     a_out)
	{
		if (!a_node)
		{
			return false;
		}

		bool atmReference = a_entry.flags.test(Data::BaseFlags::kReferenceMode);

		auto nodes = FindNodes(
			a_node,
			atmReference,
			a_root);

		if (!nodes.parent)
		{
			return false;
		}

		auto targetNodeName(GetTargetNodeName(
			a_node,
			atmReference));

		auto node = FindChildNode(nodes.parent, targetNodeName);
		if (!node)
		{
			node = CreateAttachmentNode(std::move(targetNodeName));
			nodes.parent->AttachChild(node, true);
			UpdateDownwardPass(node);
		}

		a_out.obj = node;
		a_out.ref = nodes.ref;

		return true;
	}

	bool INode::AttachObjectToTargetNode(
		const Data::NodeDescriptor& a_node,
		bool                        a_atmReference,
		NiNode*                     a_root,
		NiAVObject*                 a_object,
		NiPointer<NiNode>&          a_newRef)
	{
		if (!a_node || !a_object)
		{
			return false;
		}

		auto nodes = FindNodes(
			a_node,
			a_atmReference,
			a_root);

		if (!nodes.parent)
		{
			return false;
		}

		auto targetNodeName(GetTargetNodeName(a_node, a_atmReference));

		auto targetNode = FindChildNode(nodes.parent, targetNodeName);
		if (!targetNode)
		{
			targetNode = CreateAttachmentNode(std::move(targetNodeName));
			nodes.parent->AttachChild(targetNode, true);
			UpdateDownwardPass(targetNode);
		}

		a_newRef = nodes.ref;

		if (a_object->m_parent &&
		    a_object->m_parent != targetNode)
		{
			targetNode->AttachChild(a_object, true);
			UpdateDownwardPass(a_object);
		}

		return true;
	}

	void INode::UpdateObjectTransform(
		const Data::cacheTransform_t& a_trnsf,
		NiAVObject*                   a_object,
		NiNode*                       a_refNode)
	{
		if (!a_object)
		{
			return;
		}

		if (a_refNode)
		{
			UpdateNodeDataImpl(a_object, a_refNode, a_trnsf);
		}
		else
		{
			UpdateNodeDataImpl(a_object, a_trnsf);
		}
	}

	void INode::GetArmorNodeName(
		Game::FormID a_armor,
		Game::FormID a_arma,
		char (&a_out)[NODE_NAME_BUFFER_SIZE])
	{
		if (a_arma)
		{
			stl::snprintf(
				a_out,
				"IED ARMOR [%.8X/%.8X]",
				a_armor.get(),
				a_arma.get());
		}
		else
		{
			stl::snprintf(
				a_out,
				"IED ARMOR [%.8X]",
				a_armor.get());
		}
	}

	void INode::GetWeaponNodeName(
		Game::FormID a_weapon,
		char (&a_out)[NODE_NAME_BUFFER_SIZE])
	{
		stl::snprintf(a_out, "IED WEAPON [%.8X]", a_weapon.get());
	}

	void INode::GetMiscNodeName(
		Game::FormID a_formid,
		char (&a_out)[NODE_NAME_BUFFER_SIZE])
	{
		stl::snprintf(a_out, "IED MISC [%.8X]", a_formid.get());
	}

}