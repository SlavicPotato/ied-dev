#include "pch.h"

#include "JSONConfigSexParser.h"
#include "JSONSettingsActorParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::EditorPanelActorSettings>::Parse(
			const Json::Value& a_in,
			Data::SettingHolder::EditorPanelActorSettings& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::ConfigSex> sexParser(m_state);

			if (!sexParser.Parse(data, a_out.sex, version))
			{
				return false;
			}

			a_out.autoSelectSex = data.get("auto_select_sex", true).asBool();
			a_out.showAll = data.get("show_all", true).asBool();
			a_out.selectCrosshairActor =
				data.get("select_crosshair_actor", true).asBool();

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::EditorPanelActorSettings>::Create(
			const Data::SettingHolder::EditorPanelActorSettings& a_data,
			Json::Value& a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::ConfigSex> sexParser(m_state);

			sexParser.Create(a_data.sex, data);

			data["auto_select_sex"] = a_data.autoSelectSex;
			data["show_all"] = a_data.showAll;
			data["select_crosshair_actor"] = a_data.selectCrosshairActor;

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::SettingHolder::EditorPanelActorSettings>::GetDefault(
			Data::SettingHolder::EditorPanelActorSettings& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED