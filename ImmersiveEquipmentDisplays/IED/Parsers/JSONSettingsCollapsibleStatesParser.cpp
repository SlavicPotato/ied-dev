#include "pch.h"

#include "JSONSettingsCollapsibleStatesParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<UI::UIData::UICollapsibleStates>::Parse(
			const Json::Value& a_in,
			UI::UIData::UICollapsibleStates& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

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
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			for (auto& e : a_data.data())
			{
				Json::Value v;

				v.append(e.first);
				v.append(e.second);

				data.append(v);
			}

			a_out["version"] = 1u;
		}

		template <>
		void Parser<UI::UIData::UICollapsibleStates>::GetDefault(
			UI::UIData::UICollapsibleStates& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED