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
			const Json::Value& a_in,
			Data::configCustomPluginMap_t& a_out) const
		{
			Parser<Data::configCustomHolder_t> pholder(m_state);

			if (a_in.empty())
			{
				return true;
			}

			Data::configCustomHolder_t tmp;

			if (!pholder.Parse(a_in, tmp))
			{
				Error("%s: failed parsing record data", __FUNCTION__);
				return false;
			}

			a_out.try_emplace(StringHolder::GetSingleton().IED, std::move(tmp));

			return true;
		}

		template <>
		void Parser<Data::configCustomPluginMap_t>::Create(
			const Data::configCustomPluginMap_t& a_data,
			Json::Value& a_out) const
		{
			Parser<Data::configCustomHolder_t> pholder(m_state);

			auto it = a_data.find(StringHolder::GetSingleton().IED);
			if (it != a_data.end())
			{
				pholder.Create(it->second, a_out);
			}
		}

		template <>
		void Parser<Data::configCustomPluginMap_t>::GetDefault(
			Data::configCustomPluginMap_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED