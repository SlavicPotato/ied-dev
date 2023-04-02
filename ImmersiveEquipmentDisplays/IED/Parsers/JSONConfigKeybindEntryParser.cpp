#include "pch.h"

#include "JSONConfigKeybindEntryParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configKeybindEntry_t>::Parse(
			const Json::Value&          a_in,
			Data::configKeybindEntry_t& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			a_out.flags     = data.get("f", stl::underlying(Data::KeybindEntryFlags::kNone)).asUInt();
			a_out.key       = data.get("k", 0).asUInt();
			a_out.comboKey  = data.get("c", 0).asUInt();
			a_out.numStates = data.get("s", 1).asUInt();

			return true;
		}

		template <>
		void Parser<Data::configKeybindEntry_t>::Create(
			const Data::configKeybindEntry_t& a_in,
			Json::Value&                      a_out) const
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