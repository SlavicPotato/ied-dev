#include "pch.h"

#include "JSONConfigAdditionalCMENodeListParser.h"

#include "JSONConfigAdditionalCMENodeParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configAdditionalCMENodeList_t>::Parse(
			const Json::Value&               a_in,
			Data::configAdditionalCMENodeList_t& a_out) const
		{
			Parser<Data::configAdditionalCMENode_t> parser(m_state);

			for (auto& e : a_in)
			{
				if (!parser.Parse(e, a_out.emplace_back()))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configAdditionalCMENodeList_t>::Create(
			const Data::configAdditionalCMENodeList_t& a_data,
			Json::Value&                           a_out) const
		{
			throw std::exception("not implemented");
		}

	}
}