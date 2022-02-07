#include "pch.h"

#include "Config.h"
#include "StringHolder.h"

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
			key = e[1];
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
			parsers.reader.Get(SECT_GENERAL, "TogglePlayerDisplayKeys", ""));
		m_closeLogFile =
			parsers.reader.Get(SECT_GENERAL, "LogCloseAfterInit", false);
		m_forceDefaultConfig =
			parsers.reader.Get(SECT_GENERAL, "ForceDefaultConfig", false);

		m_nodeOverrideEnabled =
			parsers.reader.Get(SECT_NODE_OVERRIDE, "Enable", true);
		m_nodeOverridePlayerEnabled =
			parsers.reader.Get(SECT_NODE_OVERRIDE, "EnablePlayer", true);
		m_weaponAdjustDisable =
			parsers.reader.Get(SECT_NODE_OVERRIDE, "DisableVanillaWeaponAdjust", true);
		m_weaponAdjustFix =
			parsers.reader.Get(SECT_NODE_OVERRIDE, "WeaponAdjustFix", true);

		m_disableNPCProcessing =
			parsers.reader.Get(SECT_DEBUG, "DisableNPCProcessing", false);

		m_logLevel = ILog::TranslateLogLevel(parsers.reader.Get(SECT_GENERAL, "LogLevel", "debug"));

		m_taskPoolBudget = parsers.reader.Get<long long>(SECT_GENERAL, "TaskPoolBudget", 0);

		m_enableUI = parsers.reader.Get(SECT_GUI, "Enabled", true);
		m_dpiAwareness = parsers.reader.Get(SECT_GUI, "EnableProcessDPIAwareness", false);
		m_UIOpenKeys.Parse(parsers.reader.Get(SECT_GUI, "ToggleKeys", "0x0E"));
		m_forceUIOpenKeys = parsers.reader.Get(SECT_GUI, "OverrideToggleKeys", false);
		m_enableUIRestrictions = parsers.reader.Get(SECT_GUI, "EnableRestrictions", false);
		m_UIScaling = parsers.reader.Get(SECT_GUI, "EnableScaling", true);
		m_enableInMenus = parsers.reader.Get(SECT_GUI, "EnableInMenus", false);

		ParseForm(
			parsers.reader.Get(SECT_SOUND, "WeaponEquipSD", ""),
			m_sound.weapon.first);
		ParseForm(
			parsers.reader.Get(SECT_SOUND, "WeaponUnequipSD", ""),
			m_sound.weapon.second);
		ParseForm(
			parsers.reader.Get(SECT_SOUND, "GenericEquipSD", ""),
			m_sound.gen.first);
		ParseForm(
			parsers.reader.Get(SECT_SOUND, "GenericUnequipSD", ""),
			m_sound.gen.second);
		ParseForm(
			parsers.reader.Get(SECT_SOUND, "ArmorEquipSD", ""),
			m_sound.armor.first);
		ParseForm(
			parsers.reader.Get(SECT_SOUND, "ArmorUnequipSD", ""),
			m_sound.armor.second);
		ParseForm(
			parsers.reader.Get(SECT_SOUND, "ArrowEquipSD", ""),
			m_sound.arrow.first);
		ParseForm(
			parsers.reader.Get(SECT_SOUND, "ArrowUnequipSD", ""),
			m_sound.arrow.second);
		
		return (m_loaded = (parsers.reader.ParseError() == 0));
	}

}  // namespace IED