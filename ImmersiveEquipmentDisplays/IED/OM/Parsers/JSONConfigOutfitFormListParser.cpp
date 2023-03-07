#include "pch.h"

#include "JSONConfigOutfitFormListParser.h"

#include "JSONConfigOutfitFormParser.h"

#include "IED/Parsers/JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::OM::configOutfitFormList_t>::Parse(
			const Json::Value&                a_in,
			Data::OM::configOutfitFormList_t& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			auto parser = make<Data::OM::configOutfitForm_t>();

			for (auto it = data.begin(); it != data.end(); ++it)
			{
				stl::fixed_string key = it.key().asString();

				Data::OM::configOutfitForm_t tmp;

				if (!parser.Parse(*it, tmp, version))
				{
					return false;
				}

				a_out.try_emplace(key, std::move(tmp));
			}

			return true;
		}

		template <>
		void Parser<Data::OM::configOutfitFormList_t>::Create(
			const Data::OM::configOutfitFormList_t& a_data,
			Json::Value&                            a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			auto parser = make<Data::OM::configOutfitForm_t>();

			for (auto& e : a_data)
			{
				parser.Create(e.second, data[*e.first]);
			}
		}
	}
}