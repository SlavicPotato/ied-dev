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

		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configStoreCustom_t>::Parse(
			const Json::Value&         a_in,
			Data::configStoreCustom_t& a_out) const
		{
			JSON_PARSE_VERSION()

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
			Json::Value&                     a_out) const
		{
			CreateConfigStore<
				configCustomPluginMap_t,
				configMapCustom_t>(
				a_data,
				a_out,
				CURRENT_VERSION,
				m_state);
		}

	}
}