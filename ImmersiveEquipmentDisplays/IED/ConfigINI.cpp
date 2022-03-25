#include "pch.h"

#include "ConfigINI.h"

namespace IED
{
	using namespace Data;

	void ConfigKeyCombo::Parse(const std::string& a_input)
	{
		std::vector<std::uint32_t> e;
		StrHelpers::SplitString(a_input, '+', e, true, true);

		*this = {};

		auto n = e.size();

		if (n > 1)
		{
			comboKey = e[0];
			key      = e[1];
		}
		else if (n == 1)
		{
			key = e[0];
		}
	}

	ConfigINI::ConfigINI(const std::string& a_path)
	{
		Load(a_path);
	}

	bool ConfigINI::Load(const std::string& a_path)
	{
		parsers_t parsers{ a_path };

		m_toggleBlockKeys.Parse(
			parsers.reader.GetValue(SECT_GENERAL, "TogglePlayerDisplayKeys", ""));
		m_closeLogFile =
			parsers.reader.GetBoolValue(SECT_GENERAL, "LogCloseAfterInit", false);
		m_forceDefaultConfig =
			parsers.reader.GetBoolValue(SECT_GENERAL, "ForceDefaultConfig", false);
		m_immediateFavUpdate =
			parsers.reader.GetBoolValue(SECT_GENERAL, "ImmediateUpdateOnFav", false);

		m_nodeOverrideEnabled =
			parsers.reader.GetBoolValue(SECT_NODE_OVERRIDE, "Enable", true);
		m_nodeOverridePlayerEnabled =
			parsers.reader.GetBoolValue(SECT_NODE_OVERRIDE, "EnablePlayer", true);
		m_weaponAdjustDisable =
			parsers.reader.GetBoolValue(SECT_NODE_OVERRIDE, "DisableVanillaWeaponAdjust", true);
		m_weaponAdjustFix =
			parsers.reader.GetBoolValue(SECT_NODE_OVERRIDE, "WeaponAdjustFix", true);

		m_disableNPCProcessing =
			parsers.reader.GetBoolValue(SECT_DEBUG, "DisableNPCProcessing", false);

		m_logLevel = ILog::TranslateLogLevel(parsers.reader.GetValue(SECT_GENERAL, "LogLevel", "debug"));

		m_taskPoolBudget = parsers.reader.GetLongValue(SECT_GENERAL, "TaskPoolBudget", 0);

		m_enableUI             = parsers.reader.GetBoolValue(SECT_GUI, "Enabled", true);
		m_dpiAwareness         = parsers.reader.GetBoolValue(SECT_GUI, "EnableProcessDPIAwareness", false);
		m_forceUIOpenKeys      = parsers.reader.GetBoolValue(SECT_GUI, "OverrideToggleKeys", false);
		m_enableUIRestrictions = parsers.reader.GetBoolValue(SECT_GUI, "EnableRestrictions", false);
		m_UIScaling            = parsers.reader.GetBoolValue(SECT_GUI, "EnableScaling", true);
		m_enableInMenus        = parsers.reader.GetBoolValue(SECT_GUI, "EnableInMenus", false);
		m_disableIntroBanner   = parsers.reader.GetBoolValue(SECT_GUI, "DisableIntroBanner", false);
		m_introBannerVOffset   = static_cast<float>(parsers.reader.GetDoubleValue(SECT_GUI, "IntroBannerVerticalOffset", 110.0f));

		m_UIOpenKeys.Parse(parsers.reader.GetValue(SECT_GUI, "ToggleKeys", "0x0E"));

		m_effectShaders = parsers.reader.GetBoolValue("Testing", "EffectShaders", false);

		{
			auto r = m_sound.data.try_emplace(TESObjectWEAP::kTypeID);

			ParseForm(
				parsers.reader.GetValue(SECT_SOUND, "WeaponEquipSD", ""),
				r.first->second.first);
			ParseForm(
				parsers.reader.GetValue(SECT_SOUND, "WeaponUnequipSD", ""),
				r.first->second.second);
		}

		{
			auto r = m_sound.data.try_emplace(TESForm::kTypeID);

			ParseForm(
				parsers.reader.GetValue(SECT_SOUND, "GenericEquipSD", ""),
				r.first->second.first);
			ParseForm(
				parsers.reader.GetValue(SECT_SOUND, "GenericUnequipSD", ""),
				r.first->second.second);
		}

		{
			auto r = m_sound.data.try_emplace(TESObjectARMO::kTypeID);

			ParseForm(
				parsers.reader.GetValue(SECT_SOUND, "ArmorEquipSD", ""),
				r.first->second.first);
			ParseForm(
				parsers.reader.GetValue(SECT_SOUND, "ArmorUnequipSD", ""),
				r.first->second.second);
		}

		{
			auto r = m_sound.data.try_emplace(TESAmmo::kTypeID);

			ParseForm(
				parsers.reader.GetValue(SECT_SOUND, "ArrowEquipSD", ""),
				r.first->second.first);
			ParseForm(
				parsers.reader.GetValue(SECT_SOUND, "ArrowUnequipSD", ""),
				r.first->second.second);
		}

		m_loaded = parsers.reader.is_loaded();

		return m_loaded;
	}

}