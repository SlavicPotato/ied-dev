#pragma once

#include "IED/ConfigStore.h"
#include "IED/Data.h"

#include "ObjectManagerCommon.h"

namespace IED
{
	struct targetNodes_t
	{
		NiPointer<NiNode> rootNode;
		NiPointer<NiNode> ref;
	};

	class INode
	{
	private:
		struct findResult_t
		{
			NiPointer<NiNode> parent;
			NiPointer<NiNode> ref;
		};

	public:
		static constexpr std::size_t NODE_NAME_BUFFER_SIZE = MAX_PATH;

		static bool CreateTargetNode(
			const Data::configBaseValues_t& a_entry,
			const Data::NodeDescriptor&     a_node,
			NiNode*                         a_root,
			targetNodes_t&                  a_out) noexcept;

		static bool AttachObjectToTargetNode(
			const Data::NodeDescriptor& a_node,
			bool                        a_atmReference,
			NiNode*                     a_root,
			NiAVObject*                 a_object,
			NiPointer<NiNode>&          a_newRef) noexcept;

		static void UpdateObjectTransform(
			const Data::cacheTransform_t& a_trnsf,
			NiAVObject*                   a_node,
			NiNode*                       a_refNode) noexcept;

		static void UpdateObjectTransform(
			const Data::configTransform_t& a_trnsf,
			NiAVObject*                    a_object) noexcept;

		static void GetArmorNodeName(
			Game::FormID a_formid,
			Game::FormID a_arma,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept;

		static void GetShieldNodeName(
			Game::FormID a_formid,
			Game::FormID a_arma,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept;

		static void GetWeaponNodeName(
			Game::FormID a_formid,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept;

		static void GetLightNodeName(
			Game::FormID a_formid,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept;

		static void GetMiscNodeName(
			Game::FormID a_formid,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept;

		static void GetAmmoNodeName(
			Game::FormID a_formid,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]) noexcept;

		static void UpdateRootConditional(Actor *a_actor, NiNode* a_root) noexcept;

	protected:
	private:
		static findResult_t FindNodes(
			const Data::NodeDescriptor& a_node,
			bool                        a_atmReference,
			NiNode*                     a_root) noexcept;

		static BSFixedString GetTargetNodeName(
			const Data::NodeDescriptor& a_node,
			bool                        a_atmReference) noexcept;

		using fUnk1412BAFB0_t                = NiAVObject* (*)(RE::ShadowSceneNode* a_shadowSceneNode, NiAVObject* a_object, bool) noexcept;
		inline static const auto fUnk12BAFB0 = IAL::Address<fUnk1412BAFB0_t>(99712, 106349);

	public:
		static void UpdateRoot(NiNode* a_root) noexcept;
	};
}