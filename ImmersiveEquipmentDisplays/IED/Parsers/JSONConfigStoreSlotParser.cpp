#include "pch.h"

#include "JSONConfigMapSlotParser.h"
#include "JSONConfigSlotHolderParser.h"
#include "JSONConfigStoreSlotParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configStoreSlot_t>::Parse(
			const Json::Value& a_in,
			Data::configStoreSlot_t& a_out) const
		{
			JSON_PARSE_VERSION()

			return ParseConfigStore<
				configSlotHolder_t,
				configMapSlot_t>(
				a_in,
				a_out,
				version,
				m_state);
		}

		template <>
		void Parser<Data::configStoreSlot_t>::Create(
			const Data::configStoreSlot_t& a_data,
			Json::Value& a_out) const
		{
			CreateConfigStore<
				configSlotHolder_t,
				configMapSlot_t>(
				a_data,
				a_out,
				CURRENT_VERSION,
				m_state);
		}

		template <>
		void Parser<Data::configStoreSlot_t>::GetDefault(
			Data::configStoreSlot_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED