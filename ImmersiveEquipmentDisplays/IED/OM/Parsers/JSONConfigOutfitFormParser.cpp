#include "pch.h"

#include "JSONConfigOutfitFormParser.h"

#include "IED/Parsers/JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::OM::configOutfitForm_t>::Parse(
			const Json::Value&            a_in,
			Data::OM::configOutfitForm_t& a_out,
			const std::uint32_t           a_version) const
		{
			auto formParser = make<Game::FormID>();

			if (auto& items = a_in["items"])
			{
				for (auto& e : items)
				{
					Data::configFormZeroMissing_t tmp;

					if (!formParser.Parse(e, tmp))
					{
						continue;
					}

					if (!tmp)
					{
						continue;
					}

					a_out.items.emplace_back(std::move(tmp));
				}
			}

			return true;
		}

		template <>
		void Parser<Data::OM::configOutfitForm_t>::Create(
			const Data::OM::configOutfitForm_t& a_data,
			Json::Value&                        a_out) const
		{
			auto formParser = make<Game::FormID>();

			auto& data = (a_out["items"] = Json::Value(Json::ValueType::arrayValue));

			for (auto& e : a_data.items)
			{
				formParser.Create(e, data.append(Json::ValueType::objectValue));
			}
		}
	}
}