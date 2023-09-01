#include "pch.h"

#include "ConvertNodes.h"

#include "NodeOverrideData.h"
#include "SkeletonID.h"
#include "StringHolder.h"

namespace IED
{
	namespace SkeletonExtensions
	{
		using namespace ::Util::Node;

		std::size_t ConvertVanillaSheathsToXP32(NiNode* a_root) noexcept
		{
			std::size_t result = 0;

			for (auto& e : BSStringHolder::GetSingleton()->GetSheathNodes())
			{
				const auto target = GetNodeByName(a_root, e.name);
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

				if (a_root->GetObjectByName(e.cme) ||
				    a_root->GetObjectByName(e.mov))
				{
					continue;
				}

				const auto cme = CreateAttachmentNode(e.cme);

				NiPointer<NiAVObject> replacedObject;

				parent->SetAt1(target->parentIndex, cme, replacedObject);

				assert(
					target == replacedObject &&
					target->m_parent == nullptr);

				auto mov = CreateAttachmentNode(e.mov);

				cme->AttachChild(mov, true);

				mov->AttachChild(replacedObject, true);

				UpdateDownwardPass(cme);

				result++;
			}

			return result;
		}
	}
}