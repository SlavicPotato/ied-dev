#pragma once

namespace IED
{
	namespace Data
	{
		enum class ObjectSlot : std::uint32_t
		{
			k1HSword     = 0,
			k1HSwordLeft = 1,
			k1HAxe       = 2,
			k1HAxeLeft   = 3,
			k2HSword     = 4,
			k2HSwordLeft = 5,
			k2HAxe       = 6,
			k2HAxeLeft   = 7,
			kDagger      = 8,
			kDaggerLeft  = 9,
			kMace        = 10,
			kMaceLeft    = 11,
			kStaff       = 12,
			kStaffLeft   = 13,
			kBow         = 14,
			kCrossBow    = 15,
			kShield      = 16,
			kTorch       = 17,
			kAmmo        = 18,

			kMax
		};

		enum class ObjectSlotBits : std::uint32_t
		{
			kNone = 0,

			k1HSword     = 1u << 0,
			k1HSwordLeft = 1u << 1,
			k1HAxe       = 1u << 2,
			k1HAxeLeft   = 1u << 3,
			k2HSword     = 1u << 4,
			k2HSwordLeft = 1u << 5,
			k2HAxe       = 1u << 6,
			k2HAxeLeft   = 1u << 7,
			kDagger      = 1u << 8,
			kDaggerLeft  = 1u << 9,
			kMace        = 1u << 10,
			kMaceLeft    = 1u << 11,
			kStaff       = 1u << 12,
			kStaffLeft   = 1u << 13,
			kBow         = 1u << 14,
			kCrossBow    = 1u << 15,
			kShield      = 1u << 16,
			kTorch       = 1u << 17,
			kAmmo        = 1u << 18,

			kAll = static_cast<std::underlying_type_t<ObjectSlotBits>>(-1)
		};

		enum class ObjectType : std::uint32_t
		{
			k1HSword  = 0,
			k1HAxe    = 1,
			k2HSword  = 2,
			k2HAxe    = 3,
			kDagger   = 4,
			kMace     = 5,
			kBow      = 6,
			kStaff    = 7,
			kCrossBow = 8,
			kShield   = 9,
			kTorch    = 10,
			kAmmo     = 11,

			kMax
		};

		enum class ObjectTypeExtra : std::uint8_t
		{
			kNone = static_cast<std::underlying_type_t<ObjectTypeExtra>>(-1),

			k1HSword      = 0,
			k1HAxe        = 1,
			k2HSword      = 2,
			k2HAxe        = 3,
			kDagger       = 4,
			kMace         = 5,
			kBow          = 6,
			kStaff        = 7,
			kCrossBow     = 8,
			kShield       = 9,
			kTorch        = 10,
			kArmor        = 11,
			kAmmo         = 12,
			kSpell        = 13,

			kMax
		};

		enum class ObjectSlotExtra : std::uint8_t
		{
			kNone = static_cast<std::underlying_type_t<ObjectTypeExtra>>(-1),

			k1HSword      = 0,
			k1HSwordLeft  = 1,
			k1HAxe        = 2,
			k1HAxeLeft    = 3,
			k2HSword      = 4,
			k2HSwordLeft  = 5,
			k2HAxe        = 6,
			k2HAxeLeft    = 7,
			kDagger       = 8,
			kDaggerLeft   = 9,
			kMace         = 10,
			kMaceLeft     = 11,
			kStaff        = 12,
			kStaffLeft    = 13,
			kBow          = 14,
			kCrossBow     = 15,
			kShield       = 16,
			kTorch        = 17,
			kArmor        = 18,
			kAmmo         = 19,
			kSpell        = 20,
			kSpellLeft    = 21,

			kMax
		};

		enum class ConfigSex : std::uint32_t
		{
			Male   = 0,
			Female = 1
		};

		enum class ConfigClass : std::uint32_t
		{
			Race   = 0,
			NPC    = 1,
			Actor  = 2,
			Global = 3
		};

		static inline constexpr std::underlying_type_t<ConfigClass> CONFIG_CLASS_MAX = 4;

		enum class GlobalConfigType : std::uint32_t
		{
			Player = 0,
			NPC    = 1
		};

		enum class SelectionMode
		{
			kLastEquipped = 0,
			kStrongest    = 1,
			kRandom       = 2
		};

		inline static constexpr auto GetOppositeSex(Data::ConfigSex a_sex) noexcept
		{
			return a_sex == Data::ConfigSex::Female ?
			           Data::ConfigSex::Male :
                       Data::ConfigSex::Female;
		}

		using ConfigForm = IPluginInfoA::formPair_t;

		template <
			class T,
			class form_descriptor_type = stl::strip_type<T>>
		requires stl::is_any_same_v<
			form_descriptor_type,
			ConfigForm,
			Game::FormID>
		struct ConfigSound
		{
			using soundPair_t = std::pair<
				stl::optional<form_descriptor_type>,
				stl::optional<form_descriptor_type>>;

			bool enabled{ false };
			bool npc{ false };

			stl::map<std::uint8_t, soundPair_t> data;
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

	}
}
