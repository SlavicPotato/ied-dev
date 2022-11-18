#include "pch.h"

#include "JSONSettingsI3DIParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::I3DI>::Parse(
			const Json::Value&         a_in,
			Data::SettingHolder::I3DI& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			a_out.enableWeapons = data.get("enable_weapons", true).asBool();

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::I3DI>::Create(
			const Data::SettingHolder::I3DI& a_data,
			Json::Value&                     a_out) const
		{
			auto& data = a_out["data"];

			data["enable_weapons"] = a_data.enableWeapons;

			a_out["version"] = CURRENT_VERSION;
		}

	}
}