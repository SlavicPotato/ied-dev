#include "pch.h"

#include "ConfigINI.h"

namespace IED
{
	using namespace Data;

	void ConfigKeyCombo::Parse(const std::string& a_input)
	{
		stl::vector<std::uint32_t> e;
		stl::split_string(a_input, '+', e, true, true);

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
		INIConfReader reader(a_path);

		m_toggleBlockKeys.Parse(
			reader.GetValue(SECT_GENERAL, "TogglePlayerDisplayKeys", ""));
		m_closeLogFile =
			reader.GetBoolValue(SECT_GENERAL, "LogCloseAfterInit", false);
		m_forceDefaultConfig =
			reader.GetBoolValue(SECT_GENERAL, "ForceDefaultConfig", false);
		m_immediateFavUpdate =
			reader.GetBoolValue(SECT_GENERAL, "ImmediateUpdateOnFav", false);
		m_applyTransformOverrides =
			reader.GetBoolValue(SECT_GENERAL, "XP32LeftHandRotationFix", true);

		m_bipedSlotCacheMaxSize =
			std::max(reader.GetLongValue(SECT_BIPCACHE, "MaxSize", 0l), 0l);
		m_bipedSlotCacheMaxForms =
			std::max(reader.GetLongValue(SECT_BIPCACHE, "MaxFormsPerSlot", 16l), 0l);

		m_nodeOverrideEnabled =
			reader.GetBoolValue(SECT_NODE_OVERRIDE, "Enable", true);
		m_nodeOverridePlayerEnabled =
			reader.GetBoolValue(SECT_NODE_OVERRIDE, "EnablePlayer", true);
		m_weaponAdjustDisable =
			reader.GetBoolValue(SECT_NODE_OVERRIDE, "DisableVanillaWeaponAdjust", true);
		m_weaponAdjustForceDisable =
			reader.GetBoolValue(SECT_NODE_OVERRIDE, "ForceDisableVanillaWeaponAdjust", false);
		m_weaponAdjustFix =
			reader.GetBoolValue(SECT_NODE_OVERRIDE, "WeaponAdjustFix", true);
		m_enableEarlyLoadHooks =
			reader.GetBoolValue(SECT_NODE_OVERRIDE, "EnableEarlyLoadHooks", true);

		if (m_weaponAdjustForceDisable)
		{
			m_weaponAdjustDisable = true;
		}

		m_disableNPCProcessing =
			reader.GetBoolValue(SECT_DEBUG, "DisableNPCProcessing", false);
		m_forceFlushSaveData =
			reader.GetBoolValue(SECT_DEBUG, "ForceFlushSaveData", false);

		m_logLevel = ILog::LookupLogLevel(reader.GetValue(SECT_GENERAL, "LogLevel", "message"));

		m_enableUI             = reader.GetBoolValue(SECT_GUI, "Enabled", true);
		m_dpiAwareness         = reader.GetBoolValue(SECT_GUI, "EnableProcessDPIAwareness", false);
		m_forceUIOpenKeys      = reader.GetBoolValue(SECT_GUI, "OverrideToggleKeys", false);
		m_enableUIRestrictions = reader.GetBoolValue(SECT_GUI, "EnableRestrictions", false);
		m_enableInMenus        = reader.GetBoolValue(SECT_GUI, "EnableInMenus", false);
		m_disableIntroBanner   = reader.GetBoolValue(SECT_GUI, "DisableIntroBanner", false);
		m_introBannerVOffset   = static_cast<float>(reader.GetDoubleValue(SECT_GUI, "IntroBannerVerticalOffset", 110.0f));

		m_UIOpenKeys.Parse(reader.GetValue(SECT_GUI, "ToggleKeys", "0x0E"));

		auto r = m_sound.data.try_emplace(TESObjectWEAP::kTypeID);

		ParseForm(
			reader.GetValue(SECT_SOUND, "WeaponEquipSD", ""),
			r.first->second.first);
		ParseForm(
			reader.GetValue(SECT_SOUND, "WeaponUnequipSD", ""),
			r.first->second.second);

		r = m_sound.data.try_emplace(TESForm::kTypeID);

		ParseForm(
			reader.GetValue(SECT_SOUND, "GenericEquipSD", ""),
			r.first->second.first);
		ParseForm(
			reader.GetValue(SECT_SOUND, "GenericUnequipSD", ""),
			r.first->second.second);

		r = m_sound.data.try_emplace(TESObjectARMO::kTypeID);

		ParseForm(
			reader.GetValue(SECT_SOUND, "ArmorEquipSD", ""),
			r.first->second.first);
		ParseForm(
			reader.GetValue(SECT_SOUND, "ArmorUnequipSD", ""),
			r.first->second.second);

		r = m_sound.data.try_emplace(TESAmmo::kTypeID);

		ParseForm(
			reader.GetValue(SECT_SOUND, "ArrowEquipSD", ""),
			r.first->second.first);
		ParseForm(
			reader.GetValue(SECT_SOUND, "ArrowUnequipSD", ""),
			r.first->second.second);

		m_agManualMode = reader.GetLongValue(SECT_ANIM, "Mode", 0);

		if (m_agManualMode > 0)
		{
			m_agInfo.crc = static_cast<std::int32_t>(reader.GetLongValue(SECT_ANIM, "CRC", 0));

			m_agInfo.set_base(
				AnimationWeaponType::Sword,
				static_cast<std::int32_t>(reader.GetLongValue(SECT_ANIM, "GroupBaseSword", 0)));

			m_agInfo.set_base(
				AnimationWeaponType::Axe,
				static_cast<std::int32_t>(reader.GetLongValue(SECT_ANIM, "GroupBaseAxe", 0)));

			m_agInfo.set_base(
				AnimationWeaponType::Dagger,
				static_cast<std::int32_t>(reader.GetLongValue(SECT_ANIM, "GroupBaseDagger", 0)));

			m_agInfo.set_base(
				AnimationWeaponType::Mace,
				static_cast<std::int32_t>(reader.GetLongValue(SECT_ANIM, "GroupBaseMace", 0)));

			m_agInfo.set_base(
				AnimationWeaponType::TwoHandedSword,
				static_cast<std::int32_t>(reader.GetLongValue(SECT_ANIM, "GroupBase2HSword", 0)));

			m_agInfo.set_base(
				AnimationWeaponType::TwoHandedAxe,
				static_cast<std::int32_t>(reader.GetLongValue(SECT_ANIM, "GroupBase2HAxe", 0)));

			m_agInfo.set_base(
				AnimationWeaponType::Bow,
				static_cast<std::int32_t>(reader.GetLongValue(SECT_ANIM, "GroupBaseBow", 0)));

			m_agInfo.set_base_extra(
				AnimationExtraGroup::BowAttack,
				static_cast<std::int32_t>(reader.GetLongValue(SECT_ANIM, "GroupBaseBowAttack", 0)));

			m_agInfo.set_base_extra(
				AnimationExtraGroup::BowIdle,
				static_cast<std::int32_t>(reader.GetLongValue(SECT_ANIM, "GroupBaseBowIdle", 0)));
		}

		m_behaviorGraphAnims = reader.GetBoolValue(SECT_ANIM, "EnableBehaviorGraphControlledAnimations", true);

		m_enableLights           = reader.GetBoolValue(SECT_LIGHTS, "Enable", true);
		m_lightNPCCellAttachFix  = reader.GetBoolValue(SECT_LIGHTS, "VanillaLightCellAttachFix", false);
		m_lightNPCVanillaUpdates = reader.GetBoolValue(SECT_LIGHTS, "VanillaLightUpdates", false);

		m_effectShaderFlickerFix = reader.GetBoolValue(SECT_EFFECT_SHADERS, "FlickerFix", false);

		m_clearRPCOnSceneMove           = reader.GetBoolValue(SECT_MISCELLANEOUS, "ClearPlayerRandomPercentContainerOnSceneMove", false);
		m_interiorAmbientLightThreshold = static_cast<float>(reader.GetDoubleValue(SECT_MISCELLANEOUS, "InteriorAmbientLightThreshold", 0.35f));

		m_backgroundObjectLoader = reader.GetBoolValue(SECT_OBJECTDB, "BackgroundLoader", false);
		m_objectLoaderThreads = static_cast<std::uint32_t>(std::max(reader.GetLongValue(SECT_OBJECTDB, "WorkerThreads", 0), 0l));

		m_loaded = reader.is_loaded();

		return m_loaded;
	}
}