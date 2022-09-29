#pragma once

namespace IED
{
	enum class InventoryInfoExtraFlags : std::uint32_t
	{
		kNone = 0,

		kEquipped     = 1u << 0,
		kEquippedLeft = 1u << 1,
		kFavorite     = 1u << 2,
		kCannotWear   = 1u << 3,
		kHasHealth    = 1u << 5,
		kHasPoison    = 1u << 6,
		kShouldWear   = 1u << 7,

		kEquippedMask = kEquipped | kEquippedLeft
	};

	DEFINE_ENUM_CLASS_BITWISE(InventoryInfoExtraFlags)

	enum class InventoryInfoBaseFlags : std::uint32_t
	{
		kNone = 0,

		kNonPlayable = 1u << 0,
	};

	DEFINE_ENUM_CLASS_BITWISE(InventoryInfoBaseFlags)

	struct actorInventoryInfo_t
	{
	public:
		struct Base;

		struct Enchantment
		{
			Game::FormID               formid;
			std::optional<std::string> name;
		};

		struct ExtraData
		{
			[[nodiscard]] inline constexpr auto& GetName(const Base& a_base) const
			{
				return name ? *name : a_base.name;
			}

			[[nodiscard]] inline constexpr auto& GetEnchantment(const Base& a_base) const
			{
				return enchantment ? enchantment : a_base.enchantment;
			}

			stl::flag<InventoryInfoExtraFlags> flags{ InventoryInfoExtraFlags::kNone };
			std::optional<std::uint16_t>       uid;
			std::uint32_t                      itemId{ 0 };
			Game::FormID                       owner;
			std::optional<std::string>         name;
			std::optional<Enchantment>         enchantment;
			float                              health{ 1.0f };
		};

		struct Base
		{
			Game::FormID                      formid;
			std::uint8_t                      type;
			stl::flag<InventoryInfoBaseFlags> flags{ InventoryInfoBaseFlags::kNone };
			std::string                       name;
			std::optional<Enchantment>        enchantment;
			std::uint32_t                     baseCount{ 0 };
			std::int32_t                      deltaCount{ 0 };
			std::int64_t                      count{ 0 };
			stl::vector<ExtraData>            extraList;
		};

		void Update(
			TESContainer&                          a_container,
			RE::BSSimpleList<InventoryEntryData*>* a_dataList);

		stl::map<Game::FormID, Base> items;

	private:
		Base& AddOrGetBaseItem(TESBoundObject* a_item);

		void Process(TESContainer::Entry* a_entry);
		void Process(InventoryEntryData* a_entryData);
	};
}