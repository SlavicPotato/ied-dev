#pragma once

#include "ConfigData.h"

namespace IED
{
	namespace Data
	{
		struct CollectorData
		{
			enum class ItemFlags : std::uint8_t
			{
				kNone = 0,

				kEquipped     = 1ui8 << 0,
				kEquippedLeft = 1ui8 << 1,
				kFavorite     = 1ui8 << 2,
				kCannotWear   = 1ui8 << 3,

				kEquippedMask = kEquipped | kEquippedLeft
			};

			struct ItemExtraData
			{
				struct ExtraEquipped
				{
					ObjectSlotExtra slot{ ObjectSlotExtra::kNone };
					ObjectSlotExtra slotLeft{ ObjectSlotExtra::kNone };
				};

				ObjectType           type;
				ObjectTypeExtra      typeExtra;
				stl::flag<ItemFlags> flags{ ItemFlags::kNone };
				ExtraEquipped        equipped;
			};

			struct ItemData
			{
				ItemData() noexcept = delete;

				template <class T>
				constexpr ItemData(
					TESForm*             a_form,
					const ItemExtraData& a_extra,
					T                    a_count)  //
					noexcept    //
					requires(std::is_integral_v<T>)
					:
					form(a_form),
					extra(a_extra),
					itemCount(a_count),
					sharedCount(a_count){};

				constexpr ItemData(
					TESForm*             a_form,
					const ItemExtraData& a_extra) noexcept :
					form(a_form),
					extra(a_extra){};

				template <class F>
				constexpr ItemData(
					TESForm* a_form,
					F        a_getextra)                                              //
					noexcept(std::is_nothrow_invocable_r_v<ItemExtraData, F>)  //
					requires(std::is_invocable_r_v<ItemExtraData, F>)
					:
					form(a_form),
					extra(a_getextra()){};

				[[nodiscard]] constexpr bool is_equipped_right() const noexcept
				{
					return extra.flags.test(ItemFlags::kEquipped);
				}

				[[nodiscard]] constexpr bool is_equipped_left() const noexcept
				{
					return extra.flags.test(ItemFlags::kEquippedLeft);
				}

				[[nodiscard]] constexpr bool is_equipped() const noexcept
				{
					return extra.flags.test_any(ItemFlags::kEquippedMask);
				}

				[[nodiscard]] constexpr std::uint32_t get_equip_count() const noexcept;

				[[nodiscard]] constexpr bool is_favorited() const noexcept
				{
					return extra.flags.test(ItemFlags::kFavorite);
				}

				[[nodiscard]] constexpr bool cannot_wear() const noexcept
				{
					return extra.flags.test(ItemFlags::kCannotWear);
				}

				constexpr void consume_one() const noexcept
				{
					if (extra.type == Data::ObjectType::kAmmo)
					{
						sharedCount = std::min(sharedCount, 0);
					}
					else
					{
						const auto tmp = std::max(
							sharedCount,
							std::numeric_limits<std::int32_t>::min() + 1);

						sharedCount = tmp - 1;
					}
				}

				TESForm*             form;
				ItemExtraData        extra;
				std::int32_t         itemCount{ 0 };
				mutable std::int32_t sharedCount{ 0 };
			};

			[[nodiscard]] constexpr bool IsSlotEquipped(std::underlying_type_t<ObjectSlotExtra> a_slot) const noexcept
			{
				assert(a_slot < stl::underlying(ObjectSlotExtra::kMax));
				return static_cast<bool>(equippedSlots & (1ui32 << a_slot));
			}

			[[nodiscard]] constexpr bool IsSlotEquipped(ObjectSlotExtra a_slot) const noexcept
			{
				return IsSlotEquipped(stl::underlying(a_slot));
			}

			[[nodiscard]] constexpr bool IsTypePresent(std::underlying_type_t<ObjectTypeExtra> a_type) const noexcept
			{
				assert(a_slot < stl::underlying(ObjectTypeExtra::kMax));
				return static_cast<bool>(presentTypes & (1ui32 << a_type));
			}

			[[nodiscard]] constexpr bool IsTypePresent(ObjectTypeExtra a_type) const noexcept
			{
				return IsTypePresent(stl::underlying(a_type));
			}

			[[nodiscard]] constexpr void SetSlotEquipped(ObjectSlotExtra a_slot) noexcept
			{
				assert(a_slot < ObjectSlotExtra::kMax);
				equippedSlots |= (1ui32 << stl::underlying(a_slot));
			}

			[[nodiscard]] constexpr void SetTypePresent(ObjectTypeExtra a_type) noexcept
			{
				assert(a_slot < ObjectTypeExtra::kMax);
				presentTypes |= (1ui32 << stl::underlying(a_type));
			}

			using container_type = stl::flat_map<
				Game::FormID,
				ItemData,
				std::less_equal<Game::FormID>,
				stl::cache_aligned_allocator<std::pair<Game::FormID, ItemData>>>;

			static_assert(stl::L1_CACHE_LINE_SIZE % sizeof(container_type::value_type) == 0);

			struct EquippedItemData
			{
				EquippedItemData() noexcept = delete;

				constexpr EquippedItemData(const ItemData& a_rhs) noexcept :
					form(a_rhs.form),
					extraEquipped(a_rhs.extra.equipped)
				{
				}

				TESForm*                     form;
				ItemExtraData::ExtraEquipped extraEquipped;
			};

			using eq_container_type = std::vector<EquippedItemData>;

			constexpr CollectorData(
				container_type&    a_container,
				eq_container_type& a_equippedContainer)  //
				noexcept(
					std::is_nothrow_invocable_v<decltype(&container_type::clear), container_type>&&
						std::is_nothrow_invocable_v<decltype(&eq_container_type::clear), eq_container_type>) :
				forms(a_container),
				equippedForms(a_equippedContainer)
			{
				a_container.clear();
				a_equippedContainer.clear();
			}

			container_type&    forms;
			eq_container_type& equippedForms;
			std::uint32_t      equippedSlots{ 0 };
			std::uint32_t      presentTypes{ 0 };
		};

		DEFINE_ENUM_CLASS_BITWISE(CollectorData::ItemFlags);

		constexpr std::uint32_t CollectorData::ItemData::get_equip_count() const noexcept
		{
			static_assert(stl::underlying(ItemFlags::kEquipped) == 1);
			static_assert(stl::underlying(ItemFlags::kEquippedLeft) == 2);

			return static_cast<std::uint32_t>(extra.flags.value & ItemFlags::kEquipped) +
			       static_cast<std::uint32_t>((extra.flags.value >> 1) & ItemFlags::kEquipped);
		}

	}
}