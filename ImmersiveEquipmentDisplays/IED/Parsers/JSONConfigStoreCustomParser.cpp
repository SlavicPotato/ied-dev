#include "pch.h"

#include "JSONConfigCustomPluginMapParser.h"
#include "JSONConfigMapCustomParser.h"
#include "JSONConfigStoreCustomParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		template <>
		bool Parser<Data::configStoreCustom_t>::Parse(
			const Json::Value& a_in,
			Data::configStoreCustom_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			return ParseConfigStore<
				configCustomPluginMap_t,
				configMapCustom_t>(
				a_in,
				a_out,
				version,
				m_state);
		}

		template <>
		void Parser<Data::configStoreCustom_t>::Create(
			const Data::configStoreCustom_t& a_data,
			Json::Value& a_out) const
		{
			CreateConfigStore<
				configCustomPluginMap_t,
				configMapCustom_t>(
				a_data,
				a_out,
				1u,
				m_state);
		}

		template <>
		void Parser<Data::configStoreCustom_t>::GetDefault(
			Data::configStoreCustom_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED