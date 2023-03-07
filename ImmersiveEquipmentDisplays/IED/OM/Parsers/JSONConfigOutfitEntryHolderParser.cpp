#include "pch.h"

#include "JSONConfigOutfitEntryHolderParser.h"

#include "JSONConfigOutfitEntryParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::OM::configOutfitEntryHolder_t>::Parse(
			const Json::Value&                   a_in,
			Data::OM::configOutfitEntryHolder_t& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			auto parser = make<Data::OM::configOutfitEntry_t>();

			auto& d = data["data"];

			parserDesc_t<Data::OM::configOutfitEntry_t> desc[]{
				{ "m", a_out.data(IED::Data::ConfigSex::Male) },
				{ "f", a_out.data(IED::Data::ConfigSex::Female) }
			};

			for (auto& e : desc)
			{
				if (!parser.Parse(d[e.member], e.data, version))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::OM::configOutfitEntryHolder_t>::Create(
			const Data::OM::configOutfitEntryHolder_t& a_data,
			Json::Value&                               a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			auto parser = make<Data::OM::configOutfitEntry_t>();

			parserDesc_t<const Data::OM::configOutfitEntry_t> desc[]{
				{ "m", a_data.data(IED::Data::ConfigSex::Male) },
				{ "f", a_data.data(IED::Data::ConfigSex::Female) }
			};

			auto& d = data["data"];

			for (auto& f : desc)
			{
				parser.Create(f.data, d[f.member]);
			}
			
			a_out["version"] = CURRENT_VERSION;
		}
	}
}