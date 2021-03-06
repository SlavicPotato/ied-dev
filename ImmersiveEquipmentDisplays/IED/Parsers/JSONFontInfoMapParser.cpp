#include "pch.h"

#include "JSONFontGlyphDataParser.h"
#include "JSONFontInfoEntryParser.h"
#include "JSONFontInfoMapParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<fontInfoMap_t>::Parse(
			const Json::Value& a_in,
			fontInfoMap_t&     a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<fontInfoEntry_t>  parser(m_state);
			Parser<fontGlyphRange_t> rangeParser(m_state);

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

			Parser<fontGlyphData_t> gparser(m_state);

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
			Json::Value&         a_out) const
		{
		}

	}
}