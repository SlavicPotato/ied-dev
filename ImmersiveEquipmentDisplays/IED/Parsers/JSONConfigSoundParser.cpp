#include "pch.h"

#include "JSONConfigSoundPairParser.h"
#include "JSONConfigSoundParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::ConfigSound<Game::FormID>>::Parse(
			const Json::Value& a_in,
			Data::ConfigSound<Game::FormID>& a_out) const
		{
			JSON_PARSE_VERSION();

			Parser<Data::ConfigSound<Game::FormID>::soundPair_t> pparser(m_state);

			auto& data = a_in["data"];

			if (auto& forms = data["forms"])
			{
				if (!pparser.Parse(forms["armor"], a_out.armor))
				{
					return false;
				}

				if (!pparser.Parse(forms["arrow"], a_out.arrow))
				{
					return false;
				}

				if (!pparser.Parse(forms["weapon"], a_out.weapon))
				{
					return false;
				}

				if (!pparser.Parse(forms["generic"], a_out.gen))
				{
					return false;
				}
			}

			a_out.enabled = data.get("enable", false).asBool();
			a_out.npc = data.get("npc", false).asBool();

			return true;
		}

		template <>
		void Parser<Data::ConfigSound<Game::FormID>>::Create(
			const Data::ConfigSound<Game::FormID>& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));
			auto& forms = (data["forms"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::ConfigSound<Game::FormID>::soundPair_t> pparser(m_state);

			pparser.Create(a_data.armor, forms["armor"]);
			pparser.Create(a_data.arrow, forms["arrow"]);
			pparser.Create(a_data.weapon, forms["weapon"]);
			pparser.Create(a_data.gen, forms["generic"]);

			data["enable"] = a_data.enabled;
			data["npc"] = a_data.npc;

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::ConfigSound<Game::FormID>>::GetDefault(
			Data::ConfigSound<Game::FormID>& a_out) const
		{
		}
	}
}