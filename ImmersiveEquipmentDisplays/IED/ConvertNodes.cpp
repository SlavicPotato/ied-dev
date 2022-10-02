#include "pch.h"

#include "ConvertNodes.h"

#include "NodeOverrideData.h"
#include "SkeletonID.h"
#include "StringHolder.h"

#include <ext/Node.h>

namespace IED
{
	namespace ConvertNodes
	{
		using namespace ::Util::Node;

		void ConvertVanillaSheathsToXP32(NiAVObject* a_root)
		{
			if (!a_root)
			{
				return;
			}

			auto& skeletonIDs = NodeOverrideData::GetConvertNodes();

			if (skeletonIDs.empty())
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

			if (!skeletonIDs.contains(*id.get_id()))
			{
				return;
			}

			for (auto& e : sh->GetSheathNodes())
			{
				auto target = ::Util::Node::FindNode(npcroot, e.name);
				if (!target)
				{
					continue;
				}

				auto parent = target->m_parent;
				if (!parent)
				{
					continue;
				}

				if (npcroot->GetObjectByName(e.cme) ||
				    npcroot->GetObjectByName(e.mov))
				{
					continue;
				}

				auto cme = CreateAttachmentNode(e.cme);

				NiPointer<NiAVObject> replacedObject;

				parent->SetAt1(target->parentIndex, cme, replacedObject);

				ASSERT(
					target == replacedObject &&
					target->m_parent == nullptr);

				auto mov = CreateAttachmentNode(e.mov);

				cme->AttachChild(mov, true);

				mov->AttachChild(replacedObject, true);

				UpdateDownwardPass(cme);
			}
		}
	}
}