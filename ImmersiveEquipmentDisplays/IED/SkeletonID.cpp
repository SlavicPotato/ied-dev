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

		auto signature = hasher::traits::initial_value;

		signature = hasher::hash_string(
			a_root->m_name.data(),
			signature);

		if (auto rtti = a_root->GetRTTI(); rtti && rtti->name)
		{
			signature = hasher::hash_string(
				rtti->name,
				signature);
		}

		if (auto extra = a_root->GetExtraDataSafe<NiIntegerExtraData>(
				sh->m_skeletonID))
		{
			m_id.emplace(extra->m_data);

			signature = hasher::hash_bytes(
				extra->m_data,
				signature);
		}

		if (auto extra = a_root->GetExtraDataSafe<NiIntegerExtraData>(
				sh->m_bsx))
		{
			signature = hasher::hash_bytes(
				extra->m_data,
				signature);
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

					signature = hasher::hash_bytes(
						extra->m_data,
						signature);
				}

				if (auto extra = parent->GetExtraDataSafe<NiStringExtraData>(
						sh->m_rigVersion);
				    extra && extra->m_pString)
				{
					signature = hasher::hash_string(
						extra->m_pString,
						signature);
				}

				if (auto extra = parent->GetExtraDataSafe<NiStringExtraData>(
						sh->m_rigPerspective);
				    extra && extra->m_pString)
				{
					signature = hasher::hash_string(
						extra->m_pString,
						signature);
				}

				if (auto extra = parent->GetExtraDataSafe<NiStringExtraData>(
						sh->m_species);
				    extra && extra->m_pString)
				{
					signature = hasher::hash_string(
						extra->m_pString,
						signature);
				}
			}
		}

		m_signature = hasher::hash_bytes(pflags.value, signature);
	}
}