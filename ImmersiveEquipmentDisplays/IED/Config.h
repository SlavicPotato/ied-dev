#pragma once

#include "ConfigCommon.h"

#include "ConfigParsers.h"

namespace IED
{
	struct ConfigKeyCombo : public Data::ConfigKeyPair
	{
	public:
		ConfigKeyCombo() = default;
		void Parse(const std::string& a_input);

		[[nodiscard]] inline constexpr bool Has() const noexcept
		{
			return static_cast<bool>(key);
		}

		[[nodiscard]] inline constexpr auto GetKey() const noexcept
		{
			return key;
		}

		[[nodiscard]] inline constexpr auto GetComboKey() const noexcept
		{
			return comboKey;
		}

		[[nodiscard]] inline constexpr explicit operator bool() const noexcept
		{
			return static_cast<bool>(key);
		}
	};

	class ConfigINI
	{
		inline static constexpr auto SECT_GENERAL = "General";
		inline static constexpr auto SECT_DEBUG = "Debug";
		inline static constexpr auto SECT_GUI = "GUI";
		inline static constexpr auto SECT_NODE_OVERRIDE = "NodeOverride";
		inline static constexpr auto SECT_SOUND = "Sound";

		struct parsers_t
		{
			INIReader reader;
		};

	public:
		ConfigINI() = default;
		ConfigINI(const std::string& a_path);

		bool Load(const std::string& a_path);

		inline constexpr bool IsLoaded() const noexcept
		{
			return m_loaded;
		}

		ConfigKeyCombo m_toggleBlockKeys;

		bool m_enableUI{ true };
		bool m_dpiAwareness{ false };
		ConfigKeyCombo m_UIOpenKeys;
		bool m_forceUIOpenKeys{ false };
		bool m_enableUIRestrictions{ false };
		long long m_taskPoolBudget{ 0 };
		bool m_closeLogFile{ false };
		bool m_UIScaling{ true };
		bool m_nodeOverrideEnabled{ false };
		bool m_nodeOverridePlayerEnabled{ false };
		bool m_weaponAdjustDisable{ false };
		bool m_weaponAdjustFix{ true };
		bool m_forceDefaultConfig{ false };
		bool m_disableNPCProcessing{ false };
		bool m_immediateFavUpdate{ false };
		LogLevel m_logLevel{ LogLevel::Message };

		bool m_enableInMenus{ false };

		Data::ConfigSound<Data::ConfigForm> m_sound;

	private:
		bool m_loaded{ false };
	};
}  // namespace IED