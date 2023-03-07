#include "pch.h"

#include "JSONConfigStoreOutfitParser.h"

#include "JSONConfigMapOutfitParser.h"

#include "IED/Parsers/JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::OM::configStoreOutfit_t>::Parse(
			const Json::Value&             a_in,
			Data::OM::configStoreOutfit_t& a_out) const
		{
			JSON_PARSE_VERSION()

			return ParseConfigStore<
				Data::OM::configOutfitEntryHolder_t,
				Data::OM::configStoreOutfit_t::map_type>(
				a_in,
				a_out,
				version,
				m_state);
		}

		template <>
		void Parser<Data::OM::configStoreOutfit_t>::Create(
			const Data::OM::configStoreOutfit_t& a_data,
			Json::Value&                         a_out) const
		{
			CreateConfigStore<
				Data::OM::configOutfitEntryHolder_t,
				Data::OM::configStoreOutfit_t::map_type>(
				a_data,
				a_out,
				CURRENT_VERSION,
				m_state);
		}

	}
}