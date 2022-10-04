#include "pch.h"

#include "SkeletonID.h"

#include "IED/NodeOverrideData.h"
#include "IED/StringHolder.h"

#include <ext/Node.h>

namespace IED
{
	SkeletonID::SkeletonID(NiNode* a_root)
	{
		using namespace hash::fnv1;
		using namespace ::Util::Node;

		auto sh = BSStringHolder::GetSingleton();

		assert(sh != nullptr);

		m_signature = _append_hash_istring_fnv1a(
			m_signature,
			a_root->m_name.data());

		if (auto rtti = a_root->GetRTTI(); rtti && rtti->name)
		{
			m_signature = _append_hash_istring_fnv1a(
				m_signature,
				rtti->name);
		}

		if (auto extra = a_root->GetExtraData<NiIntegerExtraData>(
				sh->m_skeletonID))
		{
			m_id = extra->m_data;

			m_signature = _append_hash_fnv1a(
				m_signature,
				extra->m_data);
		}

		if (auto extra = a_root->GetExtraData<NiIntegerExtraData>(
				sh->m_bsx))
		{
			m_bsx = extra->m_data;

			m_signature = _append_hash_fnv1a(
				m_signature,
				extra->m_data);
		}

		if (a_root->GetIndexOf(sh->m_BSBoneLOD) > -1)
		{
			m_pflags.set(PresenceFlags::kHasBoneLODExtraData);
		}

		if (a_root->GetIndexOf(sh->m_BBX) > -1)
		{
			m_pflags.set(PresenceFlags::kHasBoundExtraData);
		}

		if (auto npcRoot = FindNode(a_root, sh->m_npcroot))
		{
			m_pflags.set(PresenceFlags::kHasNPCRootNode);

			if (auto parent = npcRoot->m_parent; parent && parent != a_root)
			{
				if (auto extra = parent->GetExtraData<NiFloatExtraData>(
						sh->m_XPMSE))
				{
					m_xpmse_version = extra->m_data;

					m_signature = _append_hash_fnv1a(
						m_signature,
						extra->m_data);
				}

				if (auto extra = parent->GetExtraData<NiStringExtraData>(
						sh->m_rigVersion);
				    extra && extra->m_pString)
				{
					m_signature = _append_hash_istring_fnv1a(
						m_signature,
						extra->m_pString);
				}

				if (auto extra = parent->GetExtraData<NiStringExtraData>(
						sh->m_rigPerspective);
				    extra && extra->m_pString)
				{
					m_signature = _append_hash_istring_fnv1a(
						m_signature,
						extra->m_pString);
				}

				if (auto extra = parent->GetExtraData<NiStringExtraData>(
						sh->m_species);
				    extra && extra->m_pString)
				{
					m_signature = _append_hash_istring_fnv1a(
						m_signature,
						extra->m_pString);
				}
			}
		}

		m_signature = _append_hash_fnv1a(m_signature, m_pflags);
	}
}