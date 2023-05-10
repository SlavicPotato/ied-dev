#include "pch.h"

#include "SkeletonID.h"

#include "IED/NodeOverrideData.h"
#include "IED/StringHolder.h"

#include <ext/Node.h>

namespace IED
{
	SkeletonID::SkeletonID(NiNode* a_root) noexcept
	{
		using namespace ::Util::Node;

		using hasher = stl::hasher<
			stl::fnv_hasher<
				std::uint64_t,
				stl::fnv_variant::fnv1a>,
			stl::charproc_toupper_l1s>;

		const auto sh = BSStringHolder::GetSingleton();

		assert(sh != nullptr);

		stl::flag<PresenceFlags> pflags{ PresenceFlags::kNone };

		hasher h;

		h.append_string(a_root->m_name.data());

		if (auto rtti = a_root->GetRTTI(); rtti && rtti->name)
		{
			h.append_string(rtti->name);
		}

		if (auto extra = a_root->GetExtraDataSafe<NiIntegerExtraData>(
				sh->m_skeletonID))
		{
			m_id.emplace(extra->m_data);

			h.append_type(extra->m_data);
		}

		if (auto extra = a_root->GetExtraDataSafe<NiIntegerExtraData>(
				sh->m_bsx))
		{
			h.append_type(extra->m_data);
		}

		if (a_root->GetExtraDataSafe(sh->m_BSBoneLOD))
		{
			pflags.set(PresenceFlags::kHasBoneLODExtraData);
		}

		if (a_root->GetExtraDataSafe(sh->m_BBX))
		{
			pflags.set(PresenceFlags::kHasBoundExtraData);
		}

		if (auto npcRoot = GetNodeByName(a_root, sh->m_npcroot))
		{
			pflags.set(PresenceFlags::kHasNPCRootNode);

			if (auto parent = npcRoot->m_parent; parent && parent != a_root)
			{
				if (auto extra = parent->GetExtraDataSafe<NiFloatExtraData>(
						sh->m_XPMSE))
				{
					m_xpmse_version.emplace(extra->m_data);

					h.append_type(extra->m_data);
				}

				if (auto extra = parent->GetExtraDataSafe<NiStringExtraData>(
						sh->m_rigVersion);
				    extra && extra->m_pString)
				{
					h.append_string(extra->m_pString);
				}

				if (auto extra = parent->GetExtraDataSafe<NiStringExtraData>(
						sh->m_rigPerspective);
				    extra && extra->m_pString)
				{
					h.append_string(extra->m_pString);
				}

				if (auto extra = parent->GetExtraDataSafe<NiStringExtraData>(
						sh->m_species);
				    extra && extra->m_pString)
				{
					h.append_string(extra->m_pString);
				}
			}
		}

		h.append_type(pflags.value);

		m_signature = h.finish();
	}
}