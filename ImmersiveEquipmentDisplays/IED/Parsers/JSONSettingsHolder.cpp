#include "pch.h"

#include "JSONConfigKeyPairParser.h"
#include "JSONConfigSoundParser.h"
#include "JSONNodeMapParser.h"
#include "JSONSettingsHolder.h"
#include "JSONSettingsUserInterfaceParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::Settings>::Parse(
			const Json::Value&             a_in,
			Data::SettingHolder::Settings& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::SettingHolder::UserInterface> uiParser(m_state);
			Parser<Data::ConfigKeyPair>                controlsParser(m_state);
			Parser<Data::ConfigSound<Game::FormID>>    soundParser(m_state);

			if (!uiParser.Parse(data["ui"], a_out.ui))
			{
				return false;
			}

			if (auto& keys = data["player_block_keys"])
			{
				if (!controlsParser.Parse(
						data["player_block_keys"],
						*a_out.playerBlockKeys))
				{
					return false;
				}

				a_out.playerBlockKeys.mark(true);
			}

			if (!soundParser.Parse(
					data["sound"],
					a_out.sound))
			{
				return false;
			}

			a_out.toggleKeepLoaded            = data.get("toggle_keep_loaded", false).asBool();
			a_out.hideEquipped                = data.get("hide_equipped", false).asBool();
			a_out.disableNPCSlots             = data.get("disable_npc_slots", false).asBool();
			a_out.removeFavRestriction        = data.get("remove_fav_restriction", false).asBool();
			a_out.enableXP32AA                = data.get("enable_xp32_aa", false).asBool();
			a_out.XP32AABowAtk                = data.get("xp32_aa_bow_atk", false).asBool();
			a_out.XP32AABowIdle               = data.get("xp32_aa_bow_idle", false).asBool();
			a_out.placementRandomization      = data.get("placement_randomization", false).asBool();
			a_out.hkWeaponAnimations          = data.get("hk_weap_anims", false).asBool();
			a_out.animEventForwarding         = data.get("anim_event_forwarding", false).asBool();
			a_out.effectShaderParallelUpdates = data.get("es_parallel_updates", false).asBool();

			auto& logLevel = data["log_level"];

			if (!logLevel.empty())
			{
				a_out.logLevel = static_cast<LogLevel>(logLevel.asUInt());
			}

			a_out.odbLevel = static_cast<ObjectDatabaseLevel>(
				data.get("odb_level", stl::underlying(ObjectDatabaseLevel::kNone)).asUInt());

			a_out.language = data["language"].asString();

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::Settings>::Create(
			const Data::SettingHolder::Settings& a_data,
			Json::Value&                         a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::SettingHolder::UserInterface> uiParser(m_state);
			Parser<Data::ConfigKeyPair>                controlsParser(m_state);
			Parser<Data::ConfigSound<Game::FormID>>    soundParser(m_state);

			uiParser.Create(a_data.ui, data["ui"]);

			if (a_data.playerBlockKeys)
			{
				controlsParser.Create(*a_data.playerBlockKeys, data["player_block_keys"]);
			}

			soundParser.Create(a_data.sound, data["sound"]);

			data["toggle_keep_loaded"]      = a_data.toggleKeepLoaded;
			data["hide_equipped"]           = a_data.hideEquipped;
			data["disable_npc_slots"]       = a_data.disableNPCSlots;
			data["remove_fav_restriction"]  = a_data.removeFavRestriction;
			data["enable_xp32_aa"]          = a_data.enableXP32AA;
			data["xp32_aa_bow_atk"]         = a_data.XP32AABowAtk;
			data["xp32_aa_bow_idle"]        = a_data.XP32AABowIdle;
			data["placement_randomization"] = a_data.placementRandomization;
			data["hk_weap_anims"]           = a_data.hkWeaponAnimations;
			data["anim_event_forwarding"]   = a_data.animEventForwarding;
			data["es_parallel_updates"]     = a_data.effectShaderParallelUpdates;

			if (a_data.logLevel)
			{
				data["log_level"] = stl::underlying(*a_data.logLevel);
			}

			data["odb_level"] = stl::underlying(a_data.odbLevel);

			data["language"] = *a_data.language;

			a_out["version"] = CURRENT_VERSION;
		}

	}
}