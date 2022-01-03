#pragma once

#include "ConfigCommon.h"

namespace IED
{
	namespace Data
	{
		enum class EquippedTypeFlags : std::uint8_t
		{
			kNone = 0,

			kSet = 1ui8 << 0
		};

		DEFINE_ENUM_CLASS_BITWISE(EquippedTypeFlags);

		struct collectorData_t
		{
			inline collectorData_t(
				Actor* a_actor,
				TESRace* a_race) noexcept :
				actor{ a_actor },
				race{ a_race }
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
				inline explicit itemData_t(
					TESForm* a_form) :
					form{ a_form }
				{
				}

				inline explicit itemData_t(
					TESForm* a_form,
					Data::ObjectType a_type) :
					form{ a_form },
					type{ a_type }
				{
				}

				inline constexpr bool is_equipped() const noexcept
				{
					return equipped || equippedLeft;
				}

				TESForm* form;
				ObjectType type{ Data::ObjectType::kMax };
				std::int64_t count{ 0 };
				std::int64_t sharedCount{ 0 };
				bool equipped{ false };
				bool equippedLeft{ false };
				bool favorited{ false };
				bool cannotWear{ false };
				extraItemData_t extraEquipped;
			};

			using container_type = std::unordered_map<Game::FormID, itemData_t>;
			using slot_container_type = EquippedTypeFlags[stl::underlying(ObjectSlotExtra::kMax)];

			container_type forms;
			slot_container_type slots{};

			Actor* actor;
			TESRace* race;
		};
	}
}