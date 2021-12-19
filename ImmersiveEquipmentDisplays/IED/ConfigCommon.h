#pragma once

namespace IED
{
	namespace Data
	{
		enum class ObjectSlot : std::uint32_t
		{
			k1HSword,
			k1HSwordLeft,
			k1HAxe,
			k1HAxeLeft,
			k2HSword,
			k2HSwordLeft,
			k2HAxe,
			k2HAxeLeft,
			kDagger,
			kDaggerLeft,
			kMace,
			kMaceLeft,
			kStaff,
			kStaffLeft,
			kBow,
			kCrossBow,
			kShield,
			kTorch,

			kMax
		};

		enum class ObjectType : std::uint32_t
		{
			k1HSword,
			k1HAxe,
			k2HSword,
			k2HAxe,
			kDagger,
			kMace,
			kBow,
			kStaff,
			kCrossBow,
			kShield,
			kTorch,

			kMax
		};

		enum class ObjectTypeExtra : std::uint8_t
		{
			kNone = static_cast<std::underlying_type_t<ObjectTypeExtra>>(-1),

			k1HSword = 0,
			k1HAxe,
			k2HSword,
			k2HAxe,
			kDagger,
			kMace,
			kBow,
			kStaff,
			kCrossBow,
			kShield,
			kTorch,
			kArmor,
			kAmmo,
			kSpell,

			kMax
		};

		enum class ObjectSlotExtra : std::uint8_t
		{
			kNone = static_cast<std::underlying_type_t<ObjectTypeExtra>>(-1),

			k1HSword = 0,
			k1HSwordLeft,
			k1HAxe,
			k1HAxeLeft,
			k2HSword,
			k2HSwordLeft,
			k2HAxe,
			k2HAxeLeft,
			kDagger,
			kDaggerLeft,
			kMace,
			kMaceLeft,
			kStaff,
			kStaffLeft,
			kBow,
			kCrossBow,
			kShield,
			kTorch,
			kArmor,
			kDefaultTotal = kArmor,
			kAmmo,
			kSpell,
			kSpellLeft,

			kMax
		};

		enum class ConfigSex : std::uint32_t
		{
			Male,
			Female
		};

		enum class ConfigClass : std::uint32_t
		{
			Race = 0,
			NPC = 1,
			Actor = 2,
			Global = 3
		};

		static inline constexpr std::underlying_type_t<ConfigClass> CONFIG_CLASS_MAX = 4;

		enum class GlobalConfigType : std::uint32_t
		{
			Player = 0,
			NPC = 1
		};

		enum class SelectionMode
		{
			kLastEquipped,
			kStrongest,
			kRandom
		};

		inline static constexpr auto GetOppositeSex(Data::ConfigSex a_sex) noexcept
		{
			return a_sex == Data::ConfigSex::Female ?
                       Data::ConfigSex::Male :
                       Data::ConfigSex::Female;
		}

		using ConfigForm = IPluginInfo::formPair_t;

		struct ConfigEntrySound
		{
			using soundPair_t =
				std::pair<SetObjectWrapper<ConfigForm>, SetObjectWrapper<ConfigForm>>;

			bool enabled{ false };
			bool npc{ false };
			soundPair_t gen;
			soundPair_t weapon;
			soundPair_t armor;
			soundPair_t arrow;
		};

		struct ConfigKeyPair
		{
			[[nodiscard]] inline constexpr bool Has() const noexcept
			{
				return static_cast<bool>(key);
			}

			[[nodiscard]] inline constexpr explicit operator bool() const noexcept
			{
				return static_cast<bool>(key);
			}

			std::uint32_t key{ 0 };
			std::uint32_t comboKey{ 0 };
		};

	}  // namespace Data
}  // namespace IED
