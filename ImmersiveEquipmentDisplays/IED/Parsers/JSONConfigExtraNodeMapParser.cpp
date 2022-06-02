#include "pch.h"

#include "JSONConfigExtraNodeMapParser.h"

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

			for (auto it = a_in.begin(); it != a_in.end(); ++it)
			{
				std::string key = it.key().asString();

				if (!IED::Data::NodeMap::ValidateNodeName(key))
				{
					throw std::exception("illegal node name");
				}

				Data::configExtraNodeEntry_t tmp;

				if (!entparser.Parse(*it, tmp))
				{
					//Error("%s: parsing entry '%s' failed", __FUNCTION__, key.c_str());
					return false;
				}

				auto r = a_out.emplace(key, std::move(tmp));

				// fixed_string is case insensitive
				if (!r.second)
				{
					Warning("%s: duplicate entry - '%s'", __FUNCTION__, key.c_str());
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