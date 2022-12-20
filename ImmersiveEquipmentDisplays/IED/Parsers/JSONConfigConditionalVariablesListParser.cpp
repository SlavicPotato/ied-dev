#include "pch.h"

#include "JSONConfigConditionalVariablesListParser.h"

#include "JSONConfigConditionalVariableParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConditionalVariablesList_t>::Parse(
			const Json::Value&                      a_in,
			Data::configConditionalVariablesList_t& a_out) const
		{
			Parser<Data::configConditionalVariable_t> parser(m_state);

			auto& data = a_in["data"];

			for (auto& e : data)
			{
				if (!parser.Parse(e, a_out.emplace_back()))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configConditionalVariablesList_t>::Create(
			const Data::configConditionalVariablesList_t& a_data,
			Json::Value&                                  a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			Parser<Data::configConditionalVariable_t> parser(m_state);

			for (auto& e : a_data)
			{
				parser.Create(e, data.append(Json::Value()));
			}
		}
		
		template <>
		void Parser<Data::configConditionalVariablesList_t>::Create(
			const Data::configConditionalVariablesList_t& a_data,
			Json::Value&                                  a_out,
			std::uint32_t                                 a_type) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			Parser<Data::configConditionalVariable_t> parser(m_state);

			for (auto& e : a_data)
			{
				parser.Create(e, data.append(Json::Value()), a_type);
			}
		}

	}
}