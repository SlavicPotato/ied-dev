#pragma once

#include "IED/ConfigStore.h"
#include "IED/Data.h"

#include "ObjectManagerCommon.h"

namespace IED
{
	struct nodesRef_t
	{
		NiPointer<NiNode> rootNode;
		NiPointer<NiNode> ref;
		NiPointer<NiNode> object;
		NiPointer<NiNode> physics;

		SKMP_FORCEINLINE constexpr bool IsReferenceMovedOrOphaned() const noexcept
		{
			if (auto objParent = rootNode->m_parent)
			{
				if (auto objParentParent = objParent->m_parent)
				{
					if (auto refParent = ref->m_parent)
					{
						return refParent != objParentParent;
					}
					else
					{
						return true;
					}
				}
			}

			return false;
		}

		[[nodiscard]] inline constexpr bool HasPhysicsNode() const noexcept
		{
			return static_cast<bool>(physics.get());
		}
	};

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
		inline static constexpr std::size_t NODE_NAME_BUFFER_SIZE = MAX_PATH;

		static bool CreateTargetNode(
			const Data::configBaseValues_t& a_entry,
			const Data::NodeDescriptor&     a_node,
			NiNode*                         a_root,
			targetNodes_t&                  a_out);

		static bool AttachObjectToTargetNode(
			const Data::NodeDescriptor& a_node,
			bool                        a_atmReference,
			NiNode*                     a_root,
			NiAVObject*                 a_object,
			NiPointer<NiNode>&          a_newRef);

		static void UpdateObjectTransform(
			const Data::cacheTransform_t& a_trnsf,
			NiAVObject*                   a_node,
			NiNode*                       a_refNode);

		static void UpdateObjectTransform(
			const Data::configTransform_t& a_trnsf,
			NiAVObject*                    a_object);

		static void GetArmorNodeName(
			Game::FormID a_formid,
			Game::FormID a_arma,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]);

		static void GetWeaponNodeName(
			Game::FormID a_formid,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]);

		static void GetLightNodeName(
			Game::FormID a_formid,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]);
		
		static void GetMiscNodeName(
			Game::FormID a_formid,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]);

		static void GetAmmoNodeName(
			Game::FormID a_formid,
			char (&a_out)[NODE_NAME_BUFFER_SIZE]);

		static void UpdateNodeIfGamePaused(NiNode* a_root);

	protected:
	private:
		static findResult_t FindNodes(
			const Data::NodeDescriptor& a_node,
			bool                        a_atmReference,
			NiNode*                     a_root);

		static BSFixedString GetTargetNodeName(
			const Data::NodeDescriptor& a_node,
			bool                        a_atmReference);
	};
}