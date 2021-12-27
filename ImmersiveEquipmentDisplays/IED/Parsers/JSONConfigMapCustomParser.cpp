#include "pch.h"

#include "JSONConfigCustomPluginMapParser.h"
#include "JSONConfigMapCustomParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configMapCustom_t>::Parse(
			const Json::Value& a_in,
			Data::configMapCustom_t& a_out) const
		{
			JSON_PARSE_VERSION()

			ParseConfigMap<
				Data::configCustomPluginMap_t>(
				a_in,
				a_out,
				version,
				m_state);

			return true;
		}

		template <>
		void Parser<Data::configMapCustom_t>::Create(
			const Data::configMapCustom_t& a_data,
			Json::Value& a_out) const
		{
			CreateConfigMap<
				Data::configCustomPluginMap_t>(
				a_data,
				a_out,
				CURRENT_VERSION,
				m_state);
		}

		template <>
		void Parser<Data::configMapCustom_t>::GetDefault(
			Data::configMapCustom_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED