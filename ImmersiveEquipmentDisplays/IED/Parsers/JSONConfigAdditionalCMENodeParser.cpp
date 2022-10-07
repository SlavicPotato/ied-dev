#include "pch.h"

#include "JSONConfigAdditionalCMENodeParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configAdditionalCMENode_t>::Parse(
			const Json::Value&               a_in,
			Data::configAdditionalCMENode_t& a_out) const
		{
			a_out.node = a_in["node"].asString();
			a_out.desc = a_in["desc"].asString();

			return true;
		}

		template <>
		void Parser<Data::configAdditionalCMENode_t>::Create(
			const Data::configAdditionalCMENode_t& a_data,
			Json::Value&                           a_out) const
		{
			throw std::exception("not implemented");
		}

	}
}