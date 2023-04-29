#include "pch.h"

#include "IED/EngineExtensions.h"

#include "INode.h"

namespace IED
{
	using namespace ::Util::Node;

	static void UpdateNodeDataImpl(
		NiAVObject*                   a_node,
		const Data::cacheTransform_t& a_trnsf) noexcept
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
		NiAVObject*                    a_node,
		const Data::configTransform_t& a_trnsf) noexcept
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

		if (a_trnsf.rotationMatrix)
		{
			a_node->m_localTransform.rot = *a_trnsf.rotationMatrix;
		}
		else
		{
			a_node->m_localTransform.rot.Identity();
		}
	}

	static void UpdateNodeDataImpl(
		NiAVObject*                   a_node,
		NiNode*                       a_refNode,
		const Data::cacheTransform_t& a_trnsf) noexcept
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

	void INode::UpdateRootConditional(Actor* a_actor, NiNode* a_root) noexcept
	{
		bool update = !a_actor->IsAIEnabled() || Game::Main::GetSingleton()->freezeTime;

		if (!update)
		{
			if (Game::InPausedMenu())
			{
				static constexpr UIStringHolder::STRING_INDICES menus[] = {
					UIStringHolder::STRING_INDICES::kinventoryMenu,
					UIStringHolder::STRING_INDICES::kcontainerMenu,
					UIStringHolder::STRING_INDICES::kfavoritesMenu,
					UIStringHolder::STRING_INDICES::kbarterMenu,
					UIStringHolder::STRING_INDICES::kgiftMenu,
					UIStringHolder::STRING_INDICES::kconsole,
				};

				update = Game::IsAnyMenuOpen(menus);
			}
		}

		if (update)
		{
			UpdateRoot(a_root);
		}
	}

	auto INode::FindNodes(
		const Data::NodeDescriptor& a_node,
		bool                        a_atmReference,
		NiNode*                     a_root) noexcept
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
			if (auto refNode = obj->AsNode())
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
			return { obj->AsNode() };
		}
	}

	BSFixedString INode::GetTargetNodeName(
		const Data::NodeDescriptor& a_node,
		bool                        a_atmReference) noexcept
	{
		char tmp[MAX_PATH];

		const auto fmt = (a_node.managed() || a_atmReference) ? "OBJECT R %s" : "OBJECT P %s";

		stl::snprintf(tmp, fmt, a_node.name.c_str());

		return tmp;
	}

	void INode::UpdateRoot(NiNode* a_root) noexcept
	{
		a_root->UpdateWorldBound();

		NiAVObject::ControllerUpdateContext ctx{ 0, 0x2000 };
		a_root->Update(ctx);

		fUnk12BAFB0(RE::ShadowSceneNode::GetSingleton(), a_root, false);
	}

	bool INode::CreateTargetNode(
		const Data::configBaseValues_t& a_entry,
		const Data::NodeDescriptor&     a_node,
		NiNode*                         a_root,
		targetNodes_t&                  a_out) noexcept
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

		auto targetNodeName = GetTargetNodeName(
			a_node,
			atmReference);

		auto node = FindChildNode(nodes.parent.get(), targetNodeName);
		if (!node)
		{
			node = CreateAttachmentNode(std::move(targetNodeName));
			nodes.parent->AttachChild(node, true);
			UpdateDownwardPass(node);
		}

		a_out.rootNode = node;
		a_out.ref      = std::move(nodes.ref);

		return true;
	}

	bool INode::AttachObjectToTargetNode(
		const Data::NodeDescriptor& a_node,
		bool                        a_atmReference,
		NiNode*                     a_root,
		NiAVObject*                 a_object,
		NiPointer<NiNode>&          a_newRef) noexcept
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

		auto targetNodeName = GetTargetNodeName(a_node, a_atmReference);

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
		NiNode*                       a_refNode) noexcept
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

	void INode::UpdateObjectTransform(
		const Data::configTransform_t& a_trnsf,
		NiAVObject*                    a_object) noexcept
	{
		if (a_object)
		{
			UpdateNodeDataImpl(a_object, a_trnsf);
		}
	}

	void INode::GetArmorNodeName(
		Game::FormID a_armor,
		Game::FormID a_arma,
		char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept
	{
		if (a_arma)
		{
			stl::snprintf(
				a_out,
				"OBJECT ARMOR [%.8X/%.8X]",
				a_armor.get(),
				a_arma.get());
		}
		else
		{
			stl::snprintf(
				a_out,
				"OBJECT ARMOR [%.8X]",
				a_armor.get());
		}
	}

	void INode::GetShieldNodeName(
		Game::FormID a_armor,
		Game::FormID a_arma,
		char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept
	{
		if (a_arma)
		{
			stl::snprintf(
				a_out,
				"OBJECT SHIELD [%.8X/%.8X]",
				a_armor.get(),
				a_arma.get());
		}
		else
		{
			stl::snprintf(
				a_out,
				"OBJECT SHIELD [%.8X]",
				a_armor.get());
		}
	}

	void INode::GetWeaponNodeName(
		Game::FormID a_weapon,
		char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept
	{
		stl::snprintf(a_out, "OBJECT WEAPON [%.8X]", a_weapon.get());
	}

	void INode::GetLightNodeName(
		Game::FormID a_formid,
		char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept
	{
		stl::snprintf(a_out, "OBJECT LIGHT [%.8X]", a_formid.get());
	}

	void INode::GetMiscNodeName(
		Game::FormID a_formid,
		char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept
	{
		stl::snprintf(a_out, "OBJECT MISC [%.8X]", a_formid.get());
	}

	void INode::GetAmmoNodeName(
		Game::FormID a_formid,
		char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept
	{
		stl::snprintf(a_out, "OBJECT AMMO [%.8X]", a_formid.get());
	}

}