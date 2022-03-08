#include "pch.h"

#include "JSONConfigOverrideModelGroupEntryParser.h"
#include "JSONConfigOverrideModelGroupParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configModelGroup_t>::Parse(
			const Json::Value& a_in,
			Data::configModelGroup_t& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			if (auto& entries = data["entries"])
			{
				Parser<Data::configModelGroupEntry_t> eparser(m_state);

				for (auto it = entries.begin(); it != entries.end(); ++it)
				{
					auto key = it.key().asString();

					Data::configModelGroupEntry_t tmp;

					if (!eparser.Parse(*it, tmp))
					{
						Error("%s: failed to parse entry '%s'", key.c_str());
						return false;
					}

					a_out.entries.try_emplace(std::move(key), std::move(tmp));
				}
			}

			a_out.flags = static_cast<Data::ConfigModelGroupFlags>(
				data.get("flags", stl::underlying(Data::ConfigModelGroupFlags::kNone)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::configModelGroup_t>::Create(
			const Data::configModelGroup_t& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			if (!a_data.entries.empty())
			{
				auto& entries = (data["entries"] = Json::Value(Json::ValueType::objectValue));

				Parser<Data::configModelGroupEntry_t> eparser(m_state);

				for (auto& [i, e] : a_data.entries)
				{
					eparser.Create(e, entries[i]);
				}
			}

			data["flags"] = stl::underlying(a_data.flags.value);

			a_out["version"] = CURRENT_VERSION;
		}

	}
}