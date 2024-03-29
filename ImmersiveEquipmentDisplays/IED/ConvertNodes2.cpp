#include "pch.h"

#include "ConvertNodes2.h"

#include "NodeOverrideData.h"
#include "SkeletonID.h"
#include "StringHolder.h"

namespace IED
{
	namespace SkeletonExtensions
	{
		using namespace ::Util::Node;

		void RunConvertNodes2(NiNode* a_root, const SkeletonID& a_id) noexcept
		{
			for (auto& e : NodeOverrideData::GetConvertNodes2())
			{
				if (!e.match.test(a_id))
				{
					continue;
				}

				for (auto& f : BSStringHolder::GetSingleton()->GetSheathNodes())
				{
					const auto it = e.entries.find(f.name.c_str());
					if (it == e.entries.end())
					{
						continue;
					}

					const auto target = GetNodeByName(a_root, f.name);
					if (!target)
					{
						continue;
					}

					const auto parent = target->m_parent;
					if (!parent)
					{
						continue;
					}

					if (stl::strnicmp(parent->m_name.data(), "MOV ", 4) == 0)
					{
						continue;
					}

					if (a_root->GetObjectByName(f.cme) ||
					    a_root->GetObjectByName(f.mov))
					{
						continue;
					}

					const auto cme = CreateAttachmentNode(f.cme);

					NiPointer<NiAVObject> replacedObject;

					parent->SetAt1(target->parentIndex, cme, replacedObject);

					assert(
						target == replacedObject &&
						target->m_parent == nullptr);

					const auto mov = CreateAttachmentNode(f.mov);

					mov->m_localTransform            = it->second.xfrmMOV.to_nitransform();
					replacedObject->m_localTransform = it->second.xfrmNode.to_nitransform();

					cme->AttachChild(mov, true);
					mov->AttachChild(replacedObject, true);

					UpdateDownwardPass(cme);
				}
			}
		}
	}
}