#include "pch.h"

#include "JSONConfigCustomPluginMapParser.h"
#include "JSONConfigMapCustomParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		template <>
		bool Parser<Data::configMapCustom_t>::Parse(
			const Json::Value& a_in,
			Data::configMapCustom_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			ParseConfigMap<Data::configCustomPluginMap_t>(a_in, a_out, version);

			return true;
		}

		template <>
		void Parser<Data::configMapCustom_t>::Create(
			const Data::configMapCustom_t& a_data,
			Json::Value& a_out) const
		{
			CreateConfigMap<Data::configCustomPluginMap_t>(a_data, a_out, 1u);
		}

		template <>
		void Parser<Data::configMapCustom_t>::GetDefault(
			Data::configMapCustom_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED