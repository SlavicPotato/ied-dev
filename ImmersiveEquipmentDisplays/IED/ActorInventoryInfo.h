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

		kNonPlayable  = 1u << 0,
		kEquipped     = 1u << 1,
		kEquippedLeft = 1u << 2,

		kEquippedMask = kEquipped | kEquippedLeft
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
			[[nodiscard]] constexpr auto& GetName(const Base& a_base) const noexcept
			{
				return name ? *name : a_base.name;
			}

			[[nodiscard]] constexpr auto& GetEnchantment(const Base& a_base) const noexcept
			{
				return enchantment ? enchantment : a_base.enchantment;
			}

			stl::flag<InventoryInfoExtraFlags> flags{ InventoryInfoExtraFlags::kNone };
			std::optional<std::uint16_t>       uid;
			Game::FormID                       owner;
			Game::FormID                       originalRefr;
			std::optional<std::string>         name;
			std::optional<Enchantment>         enchantment;
			float                              health{ 1.0f };
		};

		struct Base
		{
			[[nodiscard]] constexpr auto IsEquipped() const noexcept
			{
				return flags.test_any(InventoryInfoBaseFlags::kEquippedMask);
			}

			Game::FormID                      formid;
			std::uint8_t                      type;
			stl::flag<InventoryInfoBaseFlags> flags{ InventoryInfoBaseFlags::kNone };
			std::string                       name;
			std::optional<Enchantment>        enchantment;
			std::uint32_t                     baseCount{ 0 };
			std::int32_t                      countDelta{ 0 };
			std::int32_t                      count{ 0 };
			stl::vector<
				ExtraData,
#if defined(IED_USE_MIMALLOC_COLLECTOR)
				stl::mi_allocator
#else
				stl::container_allocator
#endif
				<ExtraData>>
				extraList;
		};

		void Update(
			TESContainer&                          a_container,
			RE::BSSimpleList<InventoryEntryData*>* a_dataList);

		using map_type =
			std::unordered_map<
				Game::FormID,
				Base,
				std::hash<Game::FormID>,
				std::equal_to<Game::FormID>,
#if defined(IED_USE_MIMALLOC_COLLECTOR)
				stl::mi_allocator
#else
				stl::container_allocator
#endif
				<std::pair<const Game::FormID, Base>>>;

		using vector_type =
			std::vector<
				const typename map_type::value_type*,
#if defined(IED_USE_MIMALLOC_COLLECTOR)
				stl::mi_allocator
#else
				stl::container_allocator
#endif
				<const typename map_type::value_type*>>;

		stl::vectormap<
			Game::FormID,
			Base,
			map_type,
			vector_type>
			items;

	private:
		Base& AddOrGetBaseItem(TESBoundObject* a_item);

		void Process(TESContainer::Entry* a_entry);
		void Process(InventoryEntryData* a_entryData);
	};
}