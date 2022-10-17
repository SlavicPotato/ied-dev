#include "pch.h"

#include "JSONConfigConditionalVariableEntryListParser.h"

#include "JSONConfigConditionalVariableEntryParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConditionalVariablesEntryList_t>::Parse(
			const Json::Value&                           a_in,
			Data::configConditionalVariablesEntryList_t& a_out) const
		{
			Parser<Data::configConditionalVariablesEntry_t> parser(m_state);

			auto& data = a_in["data"];

			for (auto it = data.begin(); it != data.end(); ++it)
			{
				auto r = a_out.try_emplace(it.key().asString());

				if (!parser.Parse((*it), r.first->second))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configConditionalVariablesEntryList_t>::Create(
			const Data::configConditionalVariablesEntryList_t& a_data,
			Json::Value&                                       a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::configConditionalVariablesEntry_t> parser(m_state);

			for (auto& e : a_data)
			{
				parser.Create(e.second, data[*e.first]);
			}
		}

	}
}