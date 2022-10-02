#include "pch.h"

#include "StringHolder.h"

#include "Data.h"

namespace IED
{
	StringHolder StringHolder::m_Instance;

	StringHolder::StringHolder()
	{
		using enum_type = std::underlying_type_t<Data::ObjectSlot>;

		for (enum_type i = 0; i < std::size(slotNames); i++)
		{
			slotNames[i] = Data::GetSlotName(static_cast<Data::ObjectSlot>(i));
		}
	}

	std::unique_ptr<BSStringHolder> BSStringHolder::m_Instance;

	BSStringHolder::BSStringHolder()
	{
		/*for (auto& e : ANIM_EVENTS)
		{
			m_animEventFilter.emplace(e);
		}*/

		m_sheathNodes.emplace_back(NINODE_SWORD, NINODE_MOV_DEFAULT_SWORD, NINODE_CME_DEFAULT_SWORD);
		m_sheathNodes.emplace_back(NINODE_AXE, NINODE_MOV_DEFAULT_AXE, NINODE_CME_DEFAULT_AXE);
		m_sheathNodes.emplace_back(NINODE_MACE, NINODE_MOV_DEFAULT_MACE, NINODE_CME_DEFAULT_MACE);
		m_sheathNodes.emplace_back(NINODE_DAGGER, NINODE_MOV_DEFAULT_DAGGER, NINODE_CME_DEFAULT_DAGGER);
		m_sheathNodes.emplace_back(NINODE_WEAPON_BACK, NINODE_MOV_DEFAULT_BACK, NINODE_CME_DEFAULT_BACK);
		m_sheathNodes.emplace_back(NINODE_BOW, NINODE_MOV_DEFAULT_BOW, NINODE_CME_DEFAULT_BOW);
		m_sheathNodes.emplace_back(NINODE_QUIVER, NINODE_MOV_DEFAULT_QUIVER, NINODE_CME_DEFAULT_QUIVER);
	}

	void BSStringHolder::Create()
	{
		if (!m_Instance)
		{
			ASSERT(StringCache::IsInitialized());

			m_Instance = std::make_unique<BSStringHolder>();
		}
	}

	bool BSStringHolder::IsVanillaSheathNode(
		const BSFixedString& a_name) const
	{
		return std::find_if(
				   m_sheathNodes.begin(),
				   m_sheathNodes.end(),
				   [&](auto& a_v) {
					   return a_v.name == a_name;
				   }) != m_sheathNodes.end();
	}

}