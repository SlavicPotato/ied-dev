#pragma once

#include "CommonParams.h"

namespace IED
{
	struct nodeOverrideParams_t :
		CommonParams
	{
	public:
		struct BipedInfoEntry
		{
			TESForm*      item;
			BIPED_OBJECT  bip{ BIPED_OBJECT::kNone };
			float         weaponAdjust{ 0.0f };
			std::uint64_t __pad;
		};

		using BipedInfoKey = std::pair<Game::FormID, BIPED_OBJECT>;

		using item_container_type = stl::cache_aligned::flat_map<Game::FormID, BipedInfoEntry>;

		static_assert(sizeof(item_container_type::value_type) == 0x20);

		template <class... Args>
		constexpr nodeOverrideParams_t(
			item_container_type& a_container,
			Args&&... a_args) noexcept :
			itemData(a_container),
			CommonParams(std::forward<Args>(a_args)...)
		{
		}

		constexpr auto get_biped_has_armor() noexcept
		{
			if (!bipedHasArmor)
			{
				bipedHasArmor = equipped_armor_visitor([](auto*) { return true; });
			}

			return *bipedHasArmor;
		}

		SKMP_FORCEINLINE constexpr auto& get_item_data() noexcept
		{
			if (!hasItemData)
			{
				make_item_data();
				hasItemData = true;
			}

			return itemData;
		}

		float get_weapon_adjust() noexcept;

		constexpr float get_weight_adjust() noexcept
		{
			if (!weightAdjust)
			{
				weightAdjust.emplace((actor->GetWeight() * 0.01f) * 0.5f);
			}

			return *weightAdjust;
		}

		constexpr void clear_matched_items() noexcept
		{
			matchedSlotFlags = 0;
		}

		constexpr void set_matched_item(BIPED_OBJECT a_object) noexcept
		{
			matchedSlotFlags |= 1ui64 << stl::underlying(a_object);
		}

		constexpr float get_matched_weapon_adjust() const noexcept
		{
			float result = 0.0f;

			if (hasItemData)
			{
				for (auto& e : itemData)
				{
					if (matchedSlotFlags & (1ui64 << stl::underlying(e.second.bip)))
					{
						result += e.second.weaponAdjust;
					}
				}
			}

			return result;
		}

		template <class Tf>
		constexpr bool equipped_armor_visitor(
			Tf a_func)                                                         //
			noexcept(std::is_nothrow_invocable_r_v<bool, Tf, TESObjectARMO*>)  //
			requires(std::is_invocable_r_v<bool, Tf, TESObjectARMO*>)
		{
			const auto bip = get_biped();
			if (!bip)
			{
				return false;
			}

			const auto skin       = get_actor_skin();
			const auto shieldSlot = get_npc_shield_slot();

			using enum_type = std::underlying_type_t<BIPED_OBJECT>;

			for (enum_type i = stl::underlying(BIPED_OBJECT::kHead); i < stl::underlying(BIPED_OBJECT::kEditorTotal); i++)
			{
				const auto slot = static_cast<BIPED_OBJECT>(i);

				if (slot == shieldSlot)
				{
					continue;
				}

				switch (slot)
				{
					// ??
				case BIPED_OBJECT::kDecapitateHead:
				case BIPED_OBJECT::kDecapitate:
				case BIPED_OBJECT::kFX01:
					continue;
				}

				const auto& e = bip->objects[i];

				if (!e.item ||
				    e.item == e.addon ||
				    e.item == skin)
				{
					continue;
				}

				if (const auto armor = e.item->As<TESObjectARMO>())
				{
					if (a_func(armor) == true)
					{
						return true;
					}
				}
			}

			return false;
		}

		bool has_pending_loads() noexcept;

	private:
		void make_item_data() noexcept;

		item_container_type& itemData;
		bool                 hasItemData{ false };
		std::optional<float> weaponAdjust;
		std::optional<float> weightAdjust;
		std::optional<bool>  bipedHasArmor;
		std::optional<bool>  hasPendingLoads;
		std::uint64_t        matchedSlotFlags{ 0 };
	};

}