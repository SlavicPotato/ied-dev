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

	/*BSStringHolder::BSStringHolder()
	{
		m_sheathNodes.reserve(7);
		m_sheathNodes.emplace(NINODE_AXE);
		m_sheathNodes.emplace(NINODE_MACE);
		m_sheathNodes.emplace(NINODE_SWORD);
		m_sheathNodes.emplace(NINODE_DAGGER);
		m_sheathNodes.emplace(NINODE_WEAPON_BACK);
		m_sheathNodes.emplace(NINODE_BOW);
		m_sheathNodes.emplace(NINODE_QUIVER);
	}*/

	std::unique_ptr<BSStringHolder> BSStringHolder::m_Instance;

	BSStringHolder::BSStringHolder()
	{
		for (auto& e : ANIM_EVENTS)
		{
			m_animEventFilter.emplace(e);
		}
	}

	void BSStringHolder::Create()
	{
		if (!m_Instance)
		{
			ASSERT(StringCache::IsInitialized());

			m_Instance = std::make_unique<BSStringHolder>();
		}
	}

}