#include "pch.h"

#include "StringHolder.h"

#include "Data.h"

namespace IED
{
	StringHolder StringHolder::m_Instance;

	namespace detail
	{
		template <
			class T       = stl::fixed_string,
			std::size_t N = stl::underlying(Data::ObjectSlot::kMax)>
		constexpr auto make_slot_names()
		{
			return stl::make_array<T, N>([&]<std::size_t I>() {
				constexpr auto slotid = static_cast<Data::ObjectSlot>(I);
				return stl::fixed_string::make_tuple(Data::GetSlotName(slotid));
			});
		}

	}

	StringHolder::StringHolder() :
		papyrusRestrictedPlugins(

			// can't call consteval funcs directly inside an initializer list due to a msvc bug, this is a workaround

			stl::make_array(
				stl::fixed_string::make_tuple("Skyrim.esm"),
				stl::fixed_string::make_tuple("Update.esm"),
				stl::fixed_string::make_tuple("Dragonborn.esm"),
				stl::fixed_string::make_tuple("Dawnguard.esm"),
				stl::fixed_string::make_tuple("HearthFires.esm"))

				),
		slotNames{ detail::make_slot_names() }
	{
	}

	std::unique_ptr<BSStringHolder> BSStringHolder::m_Instance;

	BSStringHolder::BSStringHolder() :
		m_sheathNodes(std::initializer_list<SheatheNodeEntryInit>{

			{ NINODE_SWORD, NINODE_MOV_DEFAULT_SWORD, NINODE_CME_DEFAULT_SWORD },
			{ NINODE_AXE, NINODE_MOV_DEFAULT_AXE, NINODE_CME_DEFAULT_AXE },
			{ NINODE_MACE, NINODE_MOV_DEFAULT_MACE, NINODE_CME_DEFAULT_MACE },
			{ NINODE_DAGGER, NINODE_MOV_DEFAULT_DAGGER, NINODE_CME_DEFAULT_DAGGER },
			{ NINODE_WEAPON_BACK, NINODE_MOV_DEFAULT_BACK, NINODE_CME_DEFAULT_BACK },
			{ NINODE_BOW, NINODE_MOV_DEFAULT_BOW, NINODE_CME_DEFAULT_BOW },
			{ NINODE_QUIVER, NINODE_MOV_DEFAULT_QUIVER, NINODE_CME_DEFAULT_QUIVER }

		}),
		m_arrows{
			NINODE_ARROW_1,
			NINODE_ARROW_2,
			NINODE_ARROW_3,
			NINODE_ARROW_4,
			NINODE_ARROW_5
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