#pragma once

#include "ConfigData.h"

#include "ConfigParsers.h"

#include "Controller/AnimationGroupInfo.h"

#include <ext/INIConfReader.h>

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
		inline static constexpr auto SECT_GENERAL       = "General";
		inline static constexpr auto SECT_DEBUG         = "Debug";
		inline static constexpr auto SECT_GUI           = "GUI";
		inline static constexpr auto SECT_NODE_OVERRIDE = "NodeOverride";
		inline static constexpr auto SECT_SOUND         = "Sound";
		inline static constexpr auto SECT_ANIM          = "Animation";
		inline static constexpr auto SECT_EXPERIMENTAL  = "Experimental";

		struct parsers_t
		{
			INIConfReader reader;
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

		bool           m_enableUI{ true };
		bool           m_dpiAwareness{ false };
		ConfigKeyCombo m_UIOpenKeys;
		bool           m_forceUIOpenKeys{ false };
		bool           m_enableUIRestrictions{ false };
		long long      m_taskPoolBudget{ 0 };
		bool           m_closeLogFile{ false };
		bool           m_UIScaling{ true };
		bool           m_nodeOverrideEnabled{ false };
		bool           m_nodeOverridePlayerEnabled{ false };
		bool           m_weaponAdjustDisable{ false };
		bool           m_weaponAdjustFix{ true };
		bool           m_forceDefaultConfig{ false };
		bool           m_disableNPCProcessing{ false };
		bool           m_immediateFavUpdate{ false };
		bool           m_effectShaders{ false };
		bool           m_disableIntroBanner{ true };
		bool           m_applyTransformOverrides{ true };
		bool           m_enableCorpseScatter{ false };
		float          m_introBannerVOffset{ 110.0f };
		LogLevel       m_logLevel{ LogLevel::Message };

		long               m_agManualMode{ 0 };
		AnimationGroupInfo m_agInfo;

		bool m_enableInMenus{ false };

		Data::ConfigSound<Data::ConfigForm> m_sound;

	private:
		bool m_loaded{ false };
	};
}