#pragma once

#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/Inventory.h"

namespace IED
{
	struct equippedItemInfo_t
	{
		TESForm*         left;
		TESForm*         right;
		Data::ObjectSlot leftSlot;
		Data::ObjectSlot rightSlot;
	};

	class IEquipment
	{
	protected:
		struct selectedItem_t
		{
			stl::optional<SlotItemCandidates::storage_type::iterator> item;

			void consume(SlotItemCandidates::storage_type& a_candidates) const;

			[[nodiscard]] inline constexpr auto* operator->() const noexcept
			{
				return std::addressof(*(*item));
			}

			[[nodiscard]] inline constexpr explicit operator bool() const noexcept
			{
				return item.has();
			}
		};

	public:
		static equippedItemInfo_t CreateEquippedItemInfo(ActorProcessManager* a_pm);

		static selectedItem_t SelectItem(
			Actor*                            a_actor,
			const Data::configSlot_t&         a_config,
			SlotItemCandidates::storage_type& a_candidates,
			Game::FormID                      a_lastEquipped);
	};

}