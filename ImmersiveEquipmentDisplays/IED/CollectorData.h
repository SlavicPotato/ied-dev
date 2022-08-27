#pragma once

#include "ConfigData.h"

#if defined(IED_USE_MIMALLOC_COLLECTOR)
#	include <ext/stl_allocator_mi.h>
#endif

namespace IED
{
	namespace Data
	{
		enum class InventoryPresenceFlags : std::uint8_t
		{
			kNone = 0,

			kSet = 1ui8 << 0
		};

		DEFINE_ENUM_CLASS_BITWISE(InventoryPresenceFlags);

		struct collectorData_t
		{
			inline collectorData_t(
				Actor* a_actor) noexcept :
				actor{ a_actor }
			{
			}

			struct extraItemData_t
			{
				ObjectTypeExtra type{ ObjectTypeExtra::kNone };
				ObjectSlotExtra slot{ ObjectSlotExtra::kNone };
				ObjectSlotExtra slotLeft{ ObjectSlotExtra::kNone };
			};

			struct itemData_t
			{
				[[nodiscard]] inline constexpr bool is_equipped() const noexcept
				{
					return equipped || equippedLeft;
				}

				TESForm*             form;
				ObjectType           type{ Data::ObjectType::kMax };
				ObjectTypeExtra      typeExtra{ ObjectTypeExtra::kNone };
				std::int64_t         count{ 0 };
				mutable std::int64_t sharedCount{ 0 };
				bool                 equipped{ false };
				bool                 equippedLeft{ false };
				bool                 favorited{ false };
				bool                 cannotWear{ false };
				extraItemData_t      extraEquipped;
			};

			[[nodiscard]] inline constexpr bool IsSlotEquipped(ObjectSlotExtra a_slot) const noexcept
			{
				assert(a_slot < ObjectSlotExtra::kMax);
				return (equippedTypeFlags[stl::underlying(a_slot)] & InventoryPresenceFlags::kSet) == InventoryPresenceFlags::kSet;
			}

			using container_type =
#if defined(IED_USE_MIMALLOC_COLLECTOR)
				std::unordered_map<
					Game::FormID,
					itemData_t,
					std::hash<Game::FormID>,
					std::equal_to<Game::FormID>,
					stl::mi_allocator<
						std::pair<
							const Game::FormID,
							itemData_t>>>
#else
				stl::unordered_map<
					Game::FormID,
					itemData_t>
#endif
				;

			container_type forms;

#if defined(IED_USE_MIMALLOC_COLLECTOR)
			std::vector<
				const itemData_t*,
				stl::mi_allocator<const itemData_t*>>
#else
			stl::vector<const itemData_t*>
#endif
				equippedForms;

			InventoryPresenceFlags equippedTypeFlags[stl::underlying(ObjectSlotExtra::kMax)]{ InventoryPresenceFlags::kNone };
			std::int64_t           typeCount[stl::underlying(ObjectTypeExtra::kMax)]{ 0 };

			Actor* actor;
		};
	}
}