#include "pch.h"

#include "JSONKeyToggleStateEntry.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<KB::KeyToggleStateEntry>::Parse(
			const Json::Value&       a_in,
			KB::KeyToggleStateEntry& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			a_out.flags     = static_cast<std::uint8_t>(data.get("f", stl::underlying(KB::KeyToggleStateEntryFlags::kNone)).asUInt());
			a_out.key       = data.get("k", 0).asUInt();
			a_out.comboKey  = data.get("c", 0).asUInt();
			a_out.SetNumStates(data.get("s", 1).asUInt());

			return true;
		}

		template <>
		void Parser<KB::KeyToggleStateEntry>::Create(
			const KB::KeyToggleStateEntry& a_in,
			Json::Value&                   a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			data["f"] = a_in.flags.underlying();
			data["k"] = a_in.key;
			data["c"] = a_in.comboKey;
			data["s"] = a_in.numStates;

			a_out["version"] = CURRENT_VERSION;
		}

	}
}