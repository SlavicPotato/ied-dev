#include "pch.h"

#include "JSONConfigOutfitEntryParser.h"

#include "JSONConfigOutfitParser.h"

#include "IED/Parsers/JSONConfigOverrideListParser.h"
#include "IED/Parsers/JSONConfigOverrideParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::OM::configOutfitEntry_t>::Parse(
			const Json::Value&             a_in,
			Data::OM::configOutfitEntry_t& a_out,
			const std::uint32_t            a_version) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			if (!make<Data::OM::configOutfit_t>().Parse(data, a_out, a_version))
			{
				return false;
			}

			a_out.flags = data.get("eflags", stl::underlying(Data::OM::configOutfitEntry_t::DEFAULT_FLAGS)).asUInt();

			ParserTemplateVA<Data::configOverrideList_t, Data::OM::configOutfit_t> ooListParser(m_state);

			if (!ooListParser.Parse(data["ovr"], a_out.overrides))
			{
				return false;
			}

			return true;
		}

		template <>
		void Parser<Data::OM::configOutfitEntry_t>::Create(
			const Data::OM::configOutfitEntry_t& a_data,
			Json::Value&                         a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			make<Data::OM::configOutfit_t>().Create(a_data, data);

			data["eflags"] = a_data.flags.underlying();

			ParserTemplateVA<Data::configOverrideList_t, Data::OM::configOutfit_t> ooListParser(m_state);

			ooListParser.Create(a_data.overrides, data["ovr"]);
		}
	}
}