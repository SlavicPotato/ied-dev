#include "pch.h"

#include "JSONConfigMapNodeOverrideParser.h"
#include "JSONConfigNodeOverrideHolderParser.h"
#include "JSONConfigStoreNodeOverrideParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configStoreNodeOverride_t>::Parse(
			const Json::Value& a_in,
			Data::configStoreNodeOverride_t& a_out) const
		{
			JSON_PARSE_VERSION()

			return ParseConfigStore<
				configNodeOverrideHolder_t,
				configMapNodeOverrides_t>(
				a_in,
				a_out,
				version,
				m_state);
		}

		template <>
		void Parser<Data::configStoreNodeOverride_t>::Create(
			const Data::configStoreNodeOverride_t& a_data,
			Json::Value& a_out) const
		{
			CreateConfigStore<
				configNodeOverrideHolder_t,
				configMapNodeOverrides_t>(
				a_data,
				a_out,
				CURRENT_VERSION,
				m_state);
		}

		template <>
		void Parser<Data::configStoreNodeOverride_t>::GetDefault(
			Data::configStoreNodeOverride_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED