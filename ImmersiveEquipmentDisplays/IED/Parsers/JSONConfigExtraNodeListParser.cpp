#include "pch.h"

#include "JSONConfigExtraNodeListParser.h"

#include "JSONConfigExtraNodeEntryParser.h"

#include "IED/NodeMap.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraNodeList_t>::Parse(
			const Json::Value&           a_in,
			Data::configExtraNodeList_t& a_out) const
		{
			Parser<Data::configExtraNodeEntry_t> entparser(m_state);

			for (auto& e : a_in)
			{
				Data::configExtraNodeEntry_t tmp;

				if (!entparser.Parse(e, tmp))
				{
					//Error("%s: parsing entry '%s' failed", __FUNCTION__, key.c_str());
					return false;
				}

				if (!IED::Data::NodeMap::ValidateNodeName(tmp.name))
				{
					throw std::exception("illegal node name");
				}

				a_out.emplace_back(std::move(tmp));
			}

			return true;
		}

		template <>
		void Parser<Data::configExtraNodeList_t>::Create(
			const Data::configExtraNodeList_t& a_data,
			Json::Value&                       a_out) const
		{
			throw std::exception("not implemented");
		}
	}
}