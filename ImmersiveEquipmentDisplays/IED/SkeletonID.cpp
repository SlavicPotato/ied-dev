#include "pch.h"

#include "SkeletonID.h"

#include "IED/NodeOverrideData.h"
#include "IED/StringHolder.h"

#include <ext/Node.h>

namespace IED
{
	SkeletonID::SkeletonID(NiNode* a_root) noexcept
	{
		using namespace hash::fnv1;
		using namespace ::Util::Node;

		const auto sh = BSStringHolder::GetSingleton();

		assert(sh != nullptr);

		stl::flag<PresenceFlags> pflags{ PresenceFlags::kNone };

		auto signature = fnv_offset_basis;

		signature = _append_hash_istring_fnv1a(
			signature,
			a_root->m_name.data());

		if (auto rtti = a_root->GetRTTI(); rtti && rtti->name)
		{
			signature = _append_hash_istring_fnv1a(
				signature,
				rtti->name);
		}

		if (auto extra = a_root->GetExtraDataSafe<NiIntegerExtraData>(
				sh->m_skeletonID))
		{
			m_id.emplace(extra->m_data);

			signature = _append_hash_fnv1a(
				signature,
				extra->m_data);
		}

		if (auto extra = a_root->GetExtraDataSafe<NiIntegerExtraData>(
				sh->m_bsx))
		{
			signature = _append_hash_fnv1a(
				signature,
				extra->m_data);
		}

		if (a_root->GetIndexOf(sh->m_BSBoneLOD) > -1)
		{
			pflags.set(PresenceFlags::kHasBoneLODExtraData);
		}

		if (a_root->GetIndexOf(sh->m_BBX) > -1)
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

					signature = _append_hash_fnv1a(
						signature,
						extra->m_data);
				}

				if (auto extra = parent->GetExtraDataSafe<NiStringExtraData>(
						sh->m_rigVersion);
				    extra && extra->m_pString)
				{
					signature = _append_hash_istring_fnv1a(
						signature,
						extra->m_pString);
				}

				if (auto extra = parent->GetExtraDataSafe<NiStringExtraData>(
						sh->m_rigPerspective);
				    extra && extra->m_pString)
				{
					signature = _append_hash_istring_fnv1a(
						signature,
						extra->m_pString);
				}

				if (auto extra = parent->GetExtraDataSafe<NiStringExtraData>(
						sh->m_species);
				    extra && extra->m_pString)
				{
					signature = _append_hash_istring_fnv1a(
						signature,
						extra->m_pString);
				}
			}
		}

		m_signature = _append_hash_fnv1a(signature, pflags);
	}
}