#include "pch.h"

#include "JSONConfigExtraNodeListParser.h"

#include "JSONConfigExtraNodeEntryParser.h"

#include "IED/NodeMap.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraNodeMap_t>::Parse(
			const Json::Value&          a_in,
			Data::configExtraNodeMap_t& a_out) const
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

				if (!Data::NodeMap::ValidateNodeName(tmp.name))
				{
					throw std::exception("illegal node name");
				}

				auto r = a_out.emplace(tmp.name, std::move(tmp));

				if (!r.second)
				{
					Warning("%s: duplicate entry - '%s'", __FUNCTION__, tmp.name.c_str());
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configExtraNodeMap_t>::Create(
			const Data::configExtraNodeMap_t& a_data,
			Json::Value&                      a_out) const
		{
			//Parser<Data::configExtraNodeEntry_t> entparser(m_state);
		}
	}
}