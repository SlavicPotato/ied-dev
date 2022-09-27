#include "pch.h"

#include "JSONSettingsActorParser.h"

#include "JSONConfigSexParser.h"
#include "JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::EditorPanelActorSettings>::Parse(
			const Json::Value&                             a_in,
			Data::SettingHolder::EditorPanelActorSettings& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::ConfigSex> sexParser(m_state);
			Parser<Game::FormID>    fp(m_state);

			if (!sexParser.Parse(data, a_out.sex, version))
			{
				return false;
			}

			a_out.autoSelectSex = data.get("auto_select_sex", true).asBool();
			a_out.showAll       = data.get("show_all", true).asBool();

			fp.Parse(data["last_selected"], a_out.lastSelected);

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::EditorPanelActorSettings>::Create(
			const Data::SettingHolder::EditorPanelActorSettings& a_data,
			Json::Value&                                         a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::ConfigSex> sexParser(m_state);
			Parser<Game::FormID>    fp(m_state);

			sexParser.Create(a_data.sex, data);

			data["auto_select_sex"] = a_data.autoSelectSex;
			data["show_all"]        = a_data.showAll;

			fp.Create(a_data.lastSelected, data["last_selected"]);

			a_out["version"] = CURRENT_VERSION;
		}

	}
}