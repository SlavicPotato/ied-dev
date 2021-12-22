#include "pch.h"

#include "JSONConfigNodeOverrideHolderParser.h"
#include "JSONConfigMapNodeOverrideParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		template <>
		bool Parser<Data::configMapNodeOverrides_t>::Parse(
			const Json::Value& a_in,
			Data::configMapNodeOverrides_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			ParseConfigMap<Data::configNodeOverrideHolder_t>(a_in, a_out, version, m_state);

			return true;
		}

		template <>
		void Parser<Data::configMapNodeOverrides_t>::Create(
			const Data::configMapNodeOverrides_t& a_data,
			Json::Value& a_out) const
		{
			CreateConfigMap<Data::configNodeOverrideHolder_t>(a_data, a_out, 1u, m_state);
		}

		template <>
		void Parser<Data::configMapNodeOverrides_t>::GetDefault(
			Data::configMapNodeOverrides_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED