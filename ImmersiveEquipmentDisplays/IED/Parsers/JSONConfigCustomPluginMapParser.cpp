#include "pch.h"

#include "JSONConfigCustomHolderParser.h"
#include "JSONConfigCustomPluginMapParser.h"
#include "JSONConfigMapCustomParser.h"

#include "IED/StringHolder.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		template <>
		bool Parser<Data::configCustomPluginMap_t>::Parse(
			const Json::Value&             a_in,
			Data::configCustomPluginMap_t& a_out) const
		{
			if (a_in)
			{
				Parser<Data::configCustomHolder_t> pholder(m_state);

				auto r = a_out.try_emplace(StringHolder::GetSingleton().IED);

				if (!pholder.Parse(a_in, r.first->second))
				{
					Error("%s: failed parsing record data", __FUNCTION__);
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configCustomPluginMap_t>::Create(
			const Data::configCustomPluginMap_t& a_data,
			Json::Value&                         a_out) const
		{
			auto it = a_data.find(StringHolder::GetSingleton().IED);
			if (it != a_data.end())
			{
				Parser<Data::configCustomHolder_t> pholder(m_state);

				pholder.Create(it->second, a_out);
			}
		}

	}
}