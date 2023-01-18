#include "pch.h"

#include "StringHolder.h"

#include "Data.h"

namespace IED
{
	StringHolder StringHolder::m_Instance;

	namespace detail
	{
		inline auto make_slot_names() noexcept
		{
			return stl::make_array<stl::fixed_string, stl::underlying(Data::ObjectSlot::kMax)>([&]<std::size_t I>() {
				constexpr auto slotid = static_cast<Data::ObjectSlot>(I);
				return stl::fixed_string::make_tuple(Data::GetSlotName(slotid));
			});
		}

	}

	StringHolder::StringHolder() :
		papyrusRestrictedPlugins{
			stl::fixed_string::make_tuple("Skyrim.esm"),
			stl::fixed_string::make_tuple("Update.esm"),
			stl::fixed_string::make_tuple("Dragonborn.esm"),
			stl::fixed_string::make_tuple("Dawnguard.esm"),
			stl::fixed_string::make_tuple("HearthFires.esm")

		},
		slotNames{ detail::make_slot_names() }
	{
	}

	std::unique_ptr<BSStringHolder> BSStringHolder::m_Instance;

	BSStringHolder::BSStringHolder() :
		m_sheathNodes{
			SheathNodeEntry{ NINODE_SWORD, NINODE_MOV_DEFAULT_SWORD, NINODE_CME_DEFAULT_SWORD },
			SheathNodeEntry{ NINODE_AXE, NINODE_MOV_DEFAULT_AXE, NINODE_CME_DEFAULT_AXE },
			SheathNodeEntry{ NINODE_MACE, NINODE_MOV_DEFAULT_MACE, NINODE_CME_DEFAULT_MACE },
			SheathNodeEntry{ NINODE_DAGGER, NINODE_MOV_DEFAULT_DAGGER, NINODE_CME_DEFAULT_DAGGER },
			SheathNodeEntry{ NINODE_WEAPON_BACK, NINODE_MOV_DEFAULT_BACK, NINODE_CME_DEFAULT_BACK },
			SheathNodeEntry{ NINODE_BOW, NINODE_MOV_DEFAULT_BOW, NINODE_CME_DEFAULT_BOW },
			SheathNodeEntry{ NINODE_QUIVER, NINODE_MOV_DEFAULT_QUIVER, NINODE_CME_DEFAULT_QUIVER }
		},
		m_arrows{
			NINODE_ARROW_1,
			NINODE_ARROW_2,
			NINODE_ARROW_3,
			NINODE_ARROW_4,
			NINODE_ARROW_5,
			NINODE_ARROW_6,
		}
	{
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
		const BSFixedString& a_name) const noexcept
	{
		return std::find_if(
				   m_sheathNodes.begin(),
				   m_sheathNodes.end(),
				   [&](auto& a_v) noexcept {
					   return a_v.name == a_name;
				   }) != m_sheathNodes.end();
	}

}