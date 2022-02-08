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
			const Json::Value& a_in,
			Data::SettingHolder::Settings& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::SettingHolder::UserInterface> uiParser(m_state);
			Parser<Data::ConfigKeyPair> controlsParser(m_state);
			Parser<Data::ConfigSound<Game::FormID>> soundParser(m_state);

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

			a_out.toggleKeepLoaded = data.get("toggle_keep_loaded", false).asBool();
			a_out.hideEquipped = data.get("hide_equipped", false).asBool();
			a_out.disableNPCSlots = data.get("disable_npc_slots", false).asBool();
			a_out.removeFavRestriction = data.get("remove_fav_restriction", false).asBool();

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
			Json::Value& a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::SettingHolder::UserInterface> uiParser(m_state);
			Parser<Data::ConfigKeyPair> controlsParser(m_state);
			Parser<Data::ConfigSound<Game::FormID>> soundParser(m_state);

			uiParser.Create(a_data.ui, data["ui"]);

			if (a_data.playerBlockKeys)
			{
				controlsParser.Create(*a_data.playerBlockKeys, data["player_block_keys"]);
			}

			soundParser.Create(a_data.sound, data["sound"]);

			data["toggle_keep_loaded"] = a_data.toggleKeepLoaded;
			data["hide_equipped"] = a_data.hideEquipped;
			data["disable_npc_slots"] = a_data.disableNPCSlots;
			data["remove_fav_restriction"] = a_data.removeFavRestriction;

			if (a_data.logLevel)
			{
				data["log_level"] = stl::underlying(*a_data.logLevel);
			}

			data["odb_level"] = stl::underlying(a_data.odbLevel);

			data["language"] = *a_data.language;

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::SettingHolder>::GetDefault(Data::SettingHolder& a_out) const
		{
		}

	}  // namespace Serialization
}  // namespace IED