#include "pch.h"

#include "JSONConfigMapSlotParser.h"
#include "JSONConfigSlotHolderParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configMapSlot_t>::Parse(
			const Json::Value& a_in,
			Data::configMapSlot_t& a_out) const
		{
			JSON_PARSE_VERSION()

			ParseConfigMap<Data::configSlotHolder_t>(a_in, a_out, version, m_state);

			return true;
		}

		template <>
		void Parser<Data::configMapSlot_t>::Create(
			const Data::configMapSlot_t& a_data,
			Json::Value& a_out) const
		{
			CreateConfigMap<Data::configSlotHolder_t>(a_data, a_out, CURRENT_VERSION, m_state);
		}

		template <>
		void Parser<Data::configMapSlot_t>::GetDefault(
			Data::configMapSlot_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED