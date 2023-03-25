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

		[[nodiscard]] constexpr bool Has() const noexcept
		{
			return static_cast<bool>(key);
		}

		[[nodiscard]] constexpr auto GetKey() const noexcept
		{
			return key;
		}

		[[nodiscard]] constexpr auto GetComboKey() const noexcept
		{
			return comboKey;
		}

		[[nodiscard]] constexpr explicit operator bool() const noexcept
		{
			return static_cast<bool>(key);
		}
	};

	class ConfigINI :
		public stl::intrusive_ref_counted
	{
		static constexpr auto SECT_GENERAL        = "General";
		static constexpr auto SECT_DEBUG          = "Debug";
		static constexpr auto SECT_GUI            = "GUI";
		static constexpr auto SECT_NODE_OVERRIDE  = "NodeOverride";
		static constexpr auto SECT_SOUND          = "Sound";
		static constexpr auto SECT_ANIM           = "Animation";
		static constexpr auto SECT_EXPERIMENTAL   = "Experimental";
		static constexpr auto SECT_BIPCACHE       = "BipedSlotCache";
		static constexpr auto SECT_EFFECT_SHADERS = "EffectShaders";
		static constexpr auto SECT_MISCELLANEOUS  = "Miscellaneous";
		static constexpr auto SECT_LIGHTS         = "Lights";
		static constexpr auto SECT_OBJECTDB       = "ObjectDatabase";

	public:
		ConfigINI() = default;
		ConfigINI(const std::string& a_path);

		bool Load(const std::string& a_path);

		constexpr bool IsLoaded() const noexcept
		{
			return m_loaded;
		}

		ConfigKeyCombo m_toggleBlockKeys;
		ConfigKeyCombo m_UIOpenKeys;

		bool          m_enableUI{ true };
		bool          m_dpiAwareness{ false };
		bool          m_forceUIOpenKeys{ false };
		bool          m_enableUIRestrictions{ false };
		bool          m_closeLogFile{ false };
		bool          m_nodeOverrideEnabled{ false };
		bool          m_nodeOverridePlayerEnabled{ false };
		bool          m_weaponAdjustDisable{ true };
		bool          m_weaponAdjustForceDisable{ false };
		bool          m_weaponAdjustFix{ true };
		bool          m_forceDefaultConfig{ false };
		bool          m_disableNPCProcessing{ false };
		bool          m_immediateFavUpdate{ false };
		bool          m_disableIntroBanner{ false };
		bool          m_applyTransformOverrides{ true };
		bool          m_enableEarlyLoadHooks{ true };
		bool          m_behaviorGraphAnims{ false };
		bool          m_forceFlushSaveData{ false };
		bool          m_enableLights{ true };
		bool          m_effectShaderFlickerFix{ false };
		bool          m_enableInMenus{ false };
		bool          m_clearRPCOnSceneMove{ false };
		bool          m_lightNPCCellAttachFix{ false };
		bool          m_lightNPCVanillaUpdates{ false };
		std::uint32_t m_bipedSlotCacheMaxSize{ 2000 };
		std::uint32_t m_bipedSlotCacheMaxForms{ 16 };
		float         m_introBannerVOffset{ 110.0f };
		float         m_interiorAmbientLightThreshold{ 0.35f };
		LogLevel      m_logLevel{ LogLevel::Message };
		bool          m_backgroundObjectLoader{ false };
		std::uint32_t m_objectLoaderThreads{ 0 };

		long               m_agManualMode{ 0 };
		AnimationGroupInfo m_agInfo;

		Data::ConfigSound<Data::ConfigForm> m_sound;

	private:
		bool m_loaded{ false };
	};
}