#include "pch.h"

#include "JSONNodeMapParser.h"
#include "JSONSettingsControlsParser.h"
#include "JSONSettingsHolder.h"
#include "JSONSettingsUserInterfaceParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::Settings>::Parse(
			const Json::Value& a_in,
			Data::SettingHolder::Settings& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			auto& data = a_in["data"];

			Parser<Data::SettingHolder::UserInterface> uiParser;
			Parser<Data::SettingHolder::Controls> controlsParser;

			if (!uiParser.Parse(data["ui"], a_out.ui))
			{
				return false;
			}

			if (!controlsParser.Parse(
					data["player_block_keys"],
					a_out.playerBlockKeys,
					version))
			{
				return false;
			}

			a_out.toggleKeepLoaded = data.get("toggle_keep_loaded", false).asBool();
			a_out.playSound = data.get("play_sound", true).asBool();
			a_out.playSoundNPC = data.get("play_sound_npc", false).asBool();
			a_out.hideEquipped = data.get("hide_equipped", false).asBool();

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

			Parser<Data::SettingHolder::UserInterface> uiParser;
			Parser<Data::SettingHolder::Controls> controlsParser;

			uiParser.Create(a_data.ui, data["ui"]);
			controlsParser.Create(a_data.playerBlockKeys, data["player_block_keys"]);

			data["toggle_keep_loaded"] = a_data.toggleKeepLoaded;
			data["play_sound"] = a_data.playSound;
			data["play_sound_npc"] = a_data.playSoundNPC;
			data["hide_equipped"] = a_data.hideEquipped;

			if (a_data.logLevel)
			{
				data["log_level"] = stl::underlying(*a_data.logLevel);
			}

			data["odb_level"] = stl::underlying(a_data.odbLevel);

			data["language"] = *a_data.language;

			a_out["version"] = 1u;
		}

		template <>
		void Parser<Data::SettingHolder>::GetDefault(Data::SettingHolder& a_out) const
		{
		}

	}  // namespace Serialization
}  // namespace IED