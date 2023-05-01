#include "pch.h"

#include "JSONImGuiStyleHolderParser.h"

#include "JSONImGuiStyleParser.h"

namespace IED
{
	static constexpr std::uint32_t CURRENT_VERSION = 1;

	namespace Serialization
	{
		template <>
		bool Parser<Data::ImGuiStyleHolder>::Parse(
			const Json::Value&      a_in,
			Data::ImGuiStyleHolder& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			a_out.flags = data.get("flags", stl::underlying(Data::ImGuiStyleHolderFlags::kNone)).asUInt();

			Parser<ImGuiStyle> parser(m_state);

			if (!parser.Parse(data["style"], a_out.style))
			{
				return false;
			}

			return true;
		}

		template <>
		void Parser<Data::ImGuiStyleHolder>::Create(
			const Data::ImGuiStyleHolder& a_data,
			Json::Value&                  a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::objectValue));

			data["flags"] = a_data.flags.underlying();

			Parser<ImGuiStyle> parser(m_state);
			parser.Create(a_data.style, data["style"]);

			a_out["version"] = CURRENT_VERSION;
		}

	}
}