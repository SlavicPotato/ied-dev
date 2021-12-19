#include "pch.h"

#include "JSONFontGlyphDataParser.h"
#include "JSONFontInfoEntryParser.h"
#include "JSONFontInfoMapParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<fontInfoMap_t>::Parse(
			const Json::Value& a_in,
			fontInfoMap_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			auto& data = a_in["data"];

			Parser<fontInfoEntry_t> parser;
			Parser<fontGlyphRange_t> rangeParser;

			for (auto it = data.begin(); it != data.end(); ++it)
			{
				auto k = it.key().asString();

				fontInfoEntry_t tmp;

				if (!parser.Parse(*it, tmp, version))
				{
					continue;
				}

				a_out.fonts.emplace(k, std::move(tmp));
			}

			auto& def = a_in["default_font"];

			Parser<fontGlyphData_t> gparser;

			if (!gparser.Parse(
					def["glyphs"],
					a_out.default_glyph_data))
			{
				return false;
			}

			a_out.default_font_size = std::max(def.get("size", 13.0f).asFloat(), 1.0f);

			return true;
		}

		template <>
		void Parser<fontInfoMap_t>::Create(
			const fontInfoMap_t& a_data,
			Json::Value& a_out) const
		{
			/*auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<fontInfoEntry_t> parser;

			for (auto& e : a_data)
			{
				Json::Value tmp;

				parser.Create(e.second, tmp);

				data[*e.first] = std::move(tmp);
			}

			a_out["version"] = 1u;*/
		}

		template <>
		void Parser<fontInfoMap_t>::GetDefault(
			fontInfoMap_t& a_out) const
		{}

	}
}