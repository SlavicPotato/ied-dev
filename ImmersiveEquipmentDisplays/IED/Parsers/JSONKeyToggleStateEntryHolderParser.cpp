#include "pch.h"

#include "JSONKeyToggleStateEntryHolderParser.h"

#include "JSONKeyToggleStateEntry.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<KB::KeyToggleStateEntryHolder>::Parse(
			const Json::Value&             a_in,
			KB::KeyToggleStateEntryHolder& a_out) const
		{
			JSON_PARSE_VERSION()

			Parser<KB::KeyToggleStateEntry> parser(m_state);

			auto& data = a_in["data"];

			a_out.flags = data.get("flags", stl::underlying(KB::KeyToggleStateEntryHolderFlags::kNone)).asUInt();

			auto& entries = data["entries"];

			for (auto it = entries.begin(); it != entries.end(); ++it)
			{
				stl::fixed_string k = it.key().asString();

				if (k.empty())
				{
					SetHasErrors();
					Error(__FUNCTION__ ": invalid ID (zero len)");
				}

				const auto r = a_out.entries.try_emplace(std::move(k));

				if (r.second)
				{
					if (!parser.Parse(*it, r.first->second))
					{
						return false;
					}
				}
				else
				{
					SetHasErrors();
					Error(__FUNCTION__ ": duplicate ID '%s'", r.first->first.c_str());
				}
			}

			return true;
		}

		template <>
		void Parser<KB::KeyToggleStateEntryHolder>::Create(
			const KB::KeyToggleStateEntryHolder& a_in,
			Json::Value&                         a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			data["flags"] = a_in.flags.underlying();

			auto& entries = (data["entries"] = Json::Value(Json::ValueType::objectValue));

			Parser<KB::KeyToggleStateEntry> parser(m_state);

			for (auto& e : a_in.entries)
			{
				auto& v = entries[*e.first];

				parser.Create(e.second, v);
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}