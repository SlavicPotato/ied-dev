#include "pch.h"

#include "JSONSettingsCollapsibleStatesParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<UI::UIData::UICollapsibleStates>::Parse(
			const Json::Value&               a_in,
			UI::UIData::UICollapsibleStates& a_out) const
		{
			JSON_PARSE_VERSION()

			for (auto& e : a_in["data"])
			{
				static_assert(sizeof(Json::Value::UInt) <= sizeof(ImGuiID));

				a_out.insert(e[0].asUInt(), e[1].asBool());
			}

			return true;
		}

		template <>
		void Parser<UI::UIData::UICollapsibleStates>::Create(
			const UI::UIData::UICollapsibleStates& a_data,
			Json::Value&                           a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			for (auto& [i, e] : a_data.data())
			{
				auto& v = data.append(Json::Value(Json::ValueType::arrayValue));

				v.append(i);
				v.append(e);
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}