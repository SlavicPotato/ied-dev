#include "pch.h"

#include "JSONConfigSoundPairParser.h"
#include "JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::ConfigSound<Game::FormID>::soundPair_t>::Parse(
			const Json::Value& a_in,
			Data::ConfigSound<Game::FormID>::soundPair_t& a_out) const
		{
			JSON_PARSE_VERSION();

			Parser<Game::FormID> formParser(m_state);

			auto& data = a_in["data"];

			Game::FormID tmp;

			if (formParser.Parse(data["equip"], tmp))
			{
				a_out.first = tmp;
			}

			if (formParser.Parse(data["unequip"], tmp))
			{
				a_out.second = tmp;
			}

			return true;
		}

		template <>
		void Parser<Data::ConfigSound<Game::FormID>::soundPair_t>::Create(
			const Data::ConfigSound<Game::FormID>::soundPair_t& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Game::FormID> formParser(m_state);

			if (a_data.first)
			{
				formParser.Create(*a_data.first, data["equip"]);
			}

			if (a_data.second)
			{
				formParser.Create(*a_data.second, data["unequip"]);
			}

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::ConfigSound<Game::FormID>::soundPair_t>::GetDefault(
			Data::ConfigSound<Game::FormID>::soundPair_t& a_out) const
		{
		}
	}
}