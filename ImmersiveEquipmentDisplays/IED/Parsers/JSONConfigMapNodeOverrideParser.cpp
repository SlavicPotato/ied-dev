#include "pch.h"

#include "JSONConfigMapNodeOverrideParser.h"
#include "JSONConfigNodeOverrideHolderParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configMapNodeOverrides_t>::Parse(
			const Json::Value& a_in,
			Data::configMapNodeOverrides_t& a_out) const
		{
			JSON_PARSE_VERSION()

			ParseConfigMap<Data::configNodeOverrideHolder_t>(a_in, a_out, version, m_state);

			return true;
		}

		template <>
		void Parser<Data::configMapNodeOverrides_t>::Create(
			const Data::configMapNodeOverrides_t& a_data,
			Json::Value& a_out) const
		{
			CreateConfigMap<Data::configNodeOverrideHolder_t>(
				a_data,
				a_out,
				CURRENT_VERSION,
				m_state);
		}

		template <>
		void Parser<Data::configMapNodeOverrides_t>::GetDefault(
			Data::configMapNodeOverrides_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED