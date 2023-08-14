#include "pch.h"

#include "JSONEffectShaderDataParser.h"
#include "JSONEffectShaderHolderParser.h"
#include "JSONEquipmentOverrideConditionListParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configEffectShaderHolder_t>::Parse(
			const Json::Value&                a_in,
			Data::configEffectShaderHolder_t& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			a_out.flags       = data.get("flags", stl::underlying(Data::configEffectShaderHolder_t::DEFAULT_FLAGS)).asUInt();
			a_out.description = data["desc"].asString();

			if (auto& cond = data["cond"])
			{
				Parser<Data::equipmentOverrideConditionList_t> mlParser(m_state);

				if (!mlParser.Parse(cond, a_out.conditions.list))
				{
					return false;
				}
			}

			if (auto& esdata = data["esd"])
			{
				Parser<Data::configEffectShaderData_t> parser(m_state);

				for (auto it = esdata.begin(); it != esdata.end(); ++it)
				{
					parser.Parse(*it, a_out.data.try_emplace(it.key().asString()).first->second);
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configEffectShaderHolder_t>::Create(
			const Data::configEffectShaderHolder_t& a_data,
			Json::Value&                            a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			data["flags"] = a_data.flags.underlying();
			data["desc"]  = a_data.description;

			if (!a_data.conditions.list.empty())
			{
				Parser<Data::equipmentOverrideConditionList_t> mlParser(m_state);

				mlParser.Create(a_data.conditions.list, data["cond"]);
			}

			if (!a_data.data.empty())
			{
				Parser<Data::configEffectShaderData_t> parser(m_state);

				auto& esdata = (data["esd"] = Json::Value(Json::ValueType::objectValue));

				for (auto& e : a_data.data)
				{
					parser.Create(e.second, esdata[*e.first]);
				}
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}