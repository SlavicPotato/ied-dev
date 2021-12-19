#include "pch.h"

#include "JSONConfigNodeOverrideHolderParser.h"
#include "JSONConfigMapNodeOverrideParser.h"
#include "JSONConfigStoreNodeOverrideParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		template <>
		bool Parser<Data::configStoreNodeOverride_t>::Parse(
			const Json::Value& a_in,
			Data::configStoreNodeOverride_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			return ParseConfigStore<configNodeOverrideHolder_t, configMapNodeOverrides_t>(a_in, a_out, version);
		}

		template <>
		void Parser<Data::configStoreNodeOverride_t>::Create(
			const Data::configStoreNodeOverride_t& a_data,
			Json::Value& a_out) const
		{
			CreateConfigStore<configNodeOverrideHolder_t, configMapNodeOverrides_t>(a_data, a_out, 1u);
		}

		template <>
		void Parser<Data::configStoreNodeOverride_t>::GetDefault(
			Data::configStoreNodeOverride_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED